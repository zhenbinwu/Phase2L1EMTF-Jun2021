#include "L1Trigger/Phase2L1EMTF/interface/TrackFormatter.h"

#include <algorithm>  // provides std::copy, std::transform
#include <cmath>
#include <iostream>
#include <set>

using namespace emtf::phase2;

struct TrackFormatter::find_hw_pt {};

struct TrackFormatter::find_hw_eta {};

struct TrackFormatter::find_hw_phi {};

struct TrackFormatter::find_hw_d0 {};

struct TrackFormatter::find_hw_z0 {};

struct TrackFormatter::find_hw_beta {};

struct TrackFormatter::find_hw_charge {};

struct TrackFormatter::find_hw_qual {};

struct TrackFormatter::find_emtf_pt_no_calib {
  // This should be implemented as a HLS FW module
  constexpr int operator()(int trk_invpt) const {
    constexpr int W_IN = 14;
    constexpr int I_IN = 1;
    constexpr int W_OUT = 15;
    constexpr int I_OUT = 11;
    // Avoid division by zero
    if (trk_invpt == 0)
      return trk_invpt;
    // Find reciprocal
    const float eps_trk_invpt = std::ldexp(1.0f, -1 * (W_IN - I_IN));
    const float eps_trk_pt = std::ldexp(1.0f, -1 * (W_OUT - I_OUT));
    float x_f32 = std::abs(eps_trk_invpt * trk_invpt);
    float y_f32 = 1.0f / x_f32;
    // round away from zero
    return static_cast<int>(std::round(y_f32 / eps_trk_pt));
  }
};

struct TrackFormatter::find_emtf_pt {
  // This should be implemented as a HLS FW module
  constexpr int operator()(int trk_invpt) const {
    constexpr int W_OUT = 15;
    constexpr int I_OUT = 11;
    // Call find_emtf_pt_no_calib()
    int trk_pt_no_calib = find_emtf_pt_no_calib{}(trk_invpt);
    // Calibration
    const int last_binx = 30;  // do not use the entries beyond this bin
                               // last_binx must be less than nbinsx
    const int nbinsx = lut.size();
    const float xmin = 0.;
    const float xmax = 60.;
    const float bin_size = (xmax - xmin) / nbinsx;  // bin size
    const float eps_trk_pt = std::ldexp(1.0f, -1 * (W_OUT - I_OUT));
    float x_f32 = eps_trk_pt * trk_pt_no_calib;
    int binx = static_cast<int>((x_f32 - xmin) / bin_size);  // no rounding
    int binx_safe = std::min(binx, last_binx);
    float x0 = static_cast<float>(binx_safe) * bin_size;
    float x1 = static_cast<float>(binx_safe + 1) * bin_size;
    float y0 = lut.at(binx_safe);
    float y1 = lut.at(binx_safe + 1);
    float y_f32 = 0.;
    if (binx < last_binx) {
      y_f32 = (x_f32 - x0) / (x1 - x0) * (y1 - y0) + y0;  // interpolate
    } else {
      y_f32 = x_f32 / x0 * y0;  // extrapolate
    }
    emtf_assert(x_f32 <= y_f32);
    // round away from zero
    return static_cast<int>(std::round(y_f32 / eps_trk_pt));
  }

  typedef std::array<float, 60> lut_type;
  static const lut_type lut;
};

const TrackFormatter::find_emtf_pt::lut_type TrackFormatter::find_emtf_pt::lut = {
    {0.000,  1.050,  2.194,  3.386,  4.620,  5.891,  7.197,  8.536,  9.904,  11.303, 12.732, 14.194,
     15.692, 17.229, 18.810, 20.439, 22.122, 23.860, 25.655, 27.505, 29.406, 31.349, 33.322, 35.310,
     37.296, 39.260, 41.184, 43.053, 44.853, 46.575, 48.215, 49.773, 51.251, 52.656, 53.995, 55.274,
     56.503, 57.688, 58.834, 59.949, 61.036, 62.100, 63.144, 64.171, 65.183, 66.183, 67.173, 68.153,
     69.125, 70.091, 71.050, 72.004, 72.954, 73.899, 74.841, 75.780, 76.716, 77.649, 78.580, 79.510}};

struct TrackFormatter::find_emtf_mode_v1 {
  constexpr int operator()(const seg_valid_array_t& x) const {
    int mode = 0;
    if (x[0] or x[9] or x[1] or x[5] or x[11]) {  // ME1/1, GE1/1, ME1/2, RE1/2, ME0
      mode |= (1 << 3);
    }
    if (x[2] or x[10] or x[6]) {  // ME2, GE2/1, RE2/2
      mode |= (1 << 2);
    }
    if (x[3] or x[7]) {  // ME3, RE3
      mode |= (1 << 1);
    }
    if (x[4] or x[8]) {  // ME4, RE4
      mode |= (1 << 0);
    }
    return mode;
  }
};

struct TrackFormatter::find_emtf_mode_v2 {
  // SingleMu (12)
  // - at least one station-1 segment (ME1/1, GE1/1, ME1/2, RE1/2, ME0)
  //   with one of the following requirements on stations 2,3,4
  //   a. if there is ME1/2 or RE1/2,
  //      i.  if there is ME1/2, require 1 more CSC station
  //      ii. else, require 1 more CSC station + 1 more station
  //   b. if there is ME1/1 or GE1/1,
  //      i.  if there is ME1/1, require 1 more CSC station + 1 more station
  //      ii. else, require 2 more CSC stations
  //   c. if there is ME0,
  //      i.  if there is ME1/1, require 1 more station in stations 3,4
  //      ii. else, require 1 more CSC station + 1 more station
  //
  // DoubleMu (8)
  // - at least one station-1 segment (ME1/1, GE1/1, ME1/2, RE1/2, ME0)
  //   with one of the following requirements on stations 2,3,4
  //   a. if there is ME1/1 or ME1/2, require 1 more station
  //   b. if there is GE1/1 or RE1/2, require 1 more CSC station
  //   c. if there is ME0,
  //      i.  if there is ME1/1, require 1 more station
  //      ii. else, require 1 more CSC station
  //
  // TripleMu (4)
  // - at least two stations
  //   a. if there is ME1/1 or ME1/2, require 1 more station
  //   b. if there is GE1/1 or RE1/2, require 1 more CSC station
  //   c. if there is ME0,
  //      i.  if there is ME1/1, require 1 more station
  //      ii. else, require 1 more CSC station
  //   d. else, require 2 more CSC stations
  //
  // SingleHit (0)
  // - at least one station
  //
  // Note that SingleMu, DoubleMu, TripleMu, SingleHit are mutually-exclusive categories.
  constexpr int operator()(const seg_valid_array_t& x) const {
    int mode = 0;
    int cnt_ye11 = x[0] + x[9];                                          // ME1/1, GE1/1
    int cnt_ye12 = x[1] + x[5];                                          // ME1/2, RE1/2
    int cnt_ye22 = x[2] + x[10] + x[6];                                  // ME2, GE2/1, RE2/2
    int cnt_ye23 = x[3] + x[7];                                          // ME3, RE3
    int cnt_ye24 = x[4] + x[8];                                          // ME4, RE4
    int cnt_ye2a = (cnt_ye22 != 0) + (cnt_ye23 != 0) + (cnt_ye24 != 0);  //
    int cnt_ye2b = (cnt_ye23 != 0) + (cnt_ye24 != 0);                    //
    int cnt_me11 = x[0];                                                 // ME1/1 only
    int cnt_me12 = x[1];                                                 // ME1/2 only
    int cnt_me14 = x[11];                                                // ME0 only
    int cnt_me2a = (x[2] != 0) + (x[3] != 0) + (x[4] != 0);              //

    // clang-format off
    // SingleMu (12)
    {
      bool rule_a_i  = (cnt_me12 != 0) and (cnt_me2a >= 1);
      bool rule_a_ii = (cnt_ye12 != 0) and (cnt_me2a >= 1) and (cnt_ye2a >= 2);
      bool rule_b_i  = (cnt_me11 != 0) and (cnt_me2a >= 1) and (cnt_ye2a >= 2);
      bool rule_b_ii = (cnt_ye11 != 0) and (cnt_me2a >= 2);
      bool rule_c_i  = (cnt_me14 != 0) and (cnt_me11 != 0) and (cnt_ye2b >= 1);
      bool rule_c_ii = (cnt_me14 != 0) and (cnt_me2a >= 1) and (cnt_ye2a >= 2);
      if (rule_a_i or rule_a_ii or rule_b_i or rule_b_ii or rule_c_i or rule_c_ii) {
        mode |= (1 << 3);
        mode |= (1 << 2);
      }
    }
    // DoubleMu (8)
    if (mode < (1 << 3)) {
      bool rule_a_i  = (cnt_me12 != 0) and (cnt_ye2a >= 1);
      bool rule_a_ii = (cnt_me11 != 0) and (cnt_ye2a >= 1);
      bool rule_b_i  = (cnt_ye12 != 0) and (cnt_me2a >= 1);
      bool rule_b_ii = (cnt_ye11 != 0) and (cnt_me2a >= 1);
      bool rule_c_i  = (cnt_me14 != 0) and (cnt_me11 != 0) and (cnt_ye2a >= 1);
      bool rule_c_ii = (cnt_me14 != 0) and (cnt_me2a >= 1);
      if (rule_a_i or rule_a_ii or rule_b_i or rule_b_ii or rule_c_i or rule_c_ii) {
        mode |= (1 << 3);
      }
    }
    // TripleMu (4)
    if (mode < (1 << 2)) {
      bool rule_a_i  = (cnt_me12 != 0) and (cnt_ye2a >= 1);
      bool rule_a_ii = (cnt_me11 != 0) and (cnt_ye2a >= 1);
      bool rule_b_i  = (cnt_ye12 != 0) and (cnt_me2a >= 1);
      bool rule_b_ii = (cnt_ye11 != 0) and (cnt_me2a >= 1);
      bool rule_c_i  = (cnt_me14 != 0) and (cnt_me11 != 0) and (cnt_ye2a >= 1);
      bool rule_c_ii = (cnt_me14 != 0) and (cnt_me2a >= 1);
      bool rule_d    = (cnt_me2a >= 2);
      if (rule_a_i or rule_a_ii or rule_b_i or rule_b_ii or rule_c_i or rule_c_ii or rule_d) {
        mode |= (1 << 2);
      }
    }
    // clang-format on
    return mode;
  }
};

// _____________________________________________________________________________
void TrackFormatter::format(int endcap,
                            int sector,
                            int bx,
                            unsigned model_version,
                            bool unconstrained,
                            const Vector& trk_data,
                            EMTFTrack& trk) const {
  static const int invalid_marker_trk_seg = 115 * 2;          // num_emtf_chambers_v3 * num_emtf_segments_v3
  static const int col_sector = (288 / 2) + 27;               // (num_emtf_img_cols / 2) + offset
  static const int ph_sector = (col_sector << 4) + (1 << 3);  // col -> ph by adding 4 bits (lshift) + offset

  const int endcap_pm = (endcap == 2) ? -1 : endcap;  // using endcap [-1,+1] convention

  // For now, these are all hardcoded
  assert(trk_data.size() == 54);
  // trk_data[0..35] are unused
  int ph_median = trk_data.at(36) + ph_sector;
  int th_median = trk_data.at(37);
  int trk_qual = trk_data.at(38);
  //int trk_bx = trk_data.at(39);  // unused
  seg_ref_array_t seg_ref_array;
  std::copy(std::next(trk_data.begin(), 40), std::next(trk_data.begin(), 52), seg_ref_array.begin());
  seg_valid_array_t seg_valid_array;
  std::transform(std::next(trk_data.begin(), 40),
                 std::next(trk_data.begin(), 52),
                 seg_valid_array.begin(),
                 [](int trk_seg) -> bool { return trk_seg != invalid_marker_trk_seg; });
  int trk_valid = trk_data.at(52);
  int trk_invpt = trk_data.at(53);

  // Invalid track
  if (not trk_valid)
    return;

  // Find EMTF/GMT variables
  const int emtf_pt = find_emtf_pt{}(trk_invpt);  // with calibration
  const int emtf_mode_v1 = find_emtf_mode_v1{}(seg_valid_array);
  const int emtf_mode_v2 = find_emtf_mode_v2{}(seg_valid_array);

  // Apply Phase-1 GMT quality requirement
  // - quality 4: [3, 5, 6, 12]
  // - quality 8: [7, 9, 10]
  // - quality 12: [11, 13, 14, 15]
  static const std::set<int> good_modes = {3, 5, 6, 12, 7, 9, 10, 11, 13, 14, 15};
  trk_valid = (good_modes.find(emtf_mode_v1) != good_modes.end());

  // Invalid track
  if (not trk_valid)
    return;

  // Set all the variables
  trk.setSegRefArray(seg_ref_array);
  trk.setSegValidArray(seg_valid_array);
  trk.setHwPt(0);      // not yet implemented
  trk.setHwEta(0);     // not yet implemented
  trk.setHwPhi(0);     // not yet implemented
  trk.setHwD0(0);      // not yet implemented
  trk.setHwZ0(0);      // not yet implemented
  trk.setHwBeta(0);    // not yet implemented
  trk.setHwCharge(0);  // not yet implemented
  trk.setHwQual(0);    // not yet implemented
  trk.setModelInvpt(trk_invpt);
  trk.setModelPhi(ph_median);
  trk.setModelEta(th_median);
  trk.setModelD0(0);    // not yet implemented
  trk.setModelZ0(0);    // not yet implemented
  trk.setModelBeta(0);  // not yet implemented
  trk.setModelQual(trk_qual);
  trk.setEmtfPt(emtf_pt);
  trk.setEmtfModeV1(emtf_mode_v1);
  trk.setEmtfModeV2(emtf_mode_v2);
  trk.setEndcap(endcap_pm);
  trk.setSector(sector);
  trk.setBx(bx);
  trk.setUnconstrained(unconstrained);
  trk.setValid(trk_valid);
}

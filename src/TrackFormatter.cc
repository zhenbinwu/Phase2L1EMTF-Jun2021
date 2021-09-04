#include "L1Trigger/Phase2L1EMTF/interface/TrackFormatter.h"

#include <algorithm>  // provides std::copy, std::transform
#include <cmath>
#include <iostream>

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
    const int last_binx = 50;  // do not use the entries beyond this bin
                               // last_binx must be less than nbinsx
    const int nbinsx = lut.size();
    const float xmin = 0.;
    const float xmax = 60.;
    const float step = (xmax - xmin) / nbinsx;  // bin size
    const float eps_trk_pt = std::ldexp(1.0f, -1 * (W_OUT - I_OUT));
    float x_f32 = eps_trk_pt * trk_pt_no_calib;
    int binx = static_cast<int>((x_f32 - xmin) / step);  // no rounding
    int binx_safe = std::min(binx, last_binx);
    float x0 = static_cast<float>(binx_safe) * step;
    float x1 = static_cast<float>(binx_safe + 1) * step;
    float y0 = lut[binx_safe];
    float y1 = lut[binx_safe + 1];
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

const TrackFormatter::find_emtf_pt::lut_type TrackFormatter::find_emtf_pt::lut = {{
    0.000,  1.082,  2.256,  3.485,  4.761,  6.081,  7.437,  8.825,  10.240, 11.678, 13.135, 14.611,
    16.107, 17.626, 19.172, 20.750, 22.368, 24.031, 25.744, 27.511, 29.330, 31.199, 33.111, 35.054,
    37.015, 38.977, 40.922, 42.833, 44.694, 46.491, 48.216, 49.862, 51.429, 52.918, 54.336, 55.689,
    56.983, 58.226, 59.425, 60.587, 61.716, 62.818, 63.897, 64.956, 65.998, 67.026, 68.041, 69.045,
    70.040, 71.028, 72.008, 72.983, 73.952, 74.917, 75.877, 76.834, 77.788, 78.739, 79.688, 80.635,
}};

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
  // - at least one station-1 hit (ME1/1, GE1/1, ME1/2, RE1/2, ME0)
  //   with one of the following requirements on station-2,3,4
  //   a. if there is ME1/1 or GE1/1, require 2 more stations
  //   b. if there is ME1/2 or RE1/2, require 1 more station
  //   c. if there is ME0 and
  //      i.  if there is ME1/1 or GE1/1, require 1 more station
  //      ii. else, require 2 more stations
  //
  // DoubleMu (8)
  // - at least one station-1 hit (ME1/1, GE1/1, ME1/2, RE1/2, ME0)
  //   with one of the following requirements on station-2,3,4
  //   a. if there is ME1/1, GE1/1, ME1/2 or RE1/2, require 1 more station
  //   b. if there is ME0 and
  //      i.  if there is ME1/1, GE1/1, require no more station
  //      ii. else, require 1 more station
  //
  // TripleMu (4)
  // - at least two stations
  //   a. same as DoubleMu req a
  //   b. same as DoubleMu req b
  //   c. if there no station-1 hit, require 2 more stations
  //
  // SingleHit (0)
  // - at least one station
  //
  // Note that SingleMu, DoubleMu, TripleMu, SingleHit are mutually-exclusive categories.
  constexpr int operator()(const seg_valid_array_t& x) const {
    int mode = 0;
    int cnt_me11 = x[0] + x[9];          // ME1/1, GE1/1
    int cnt_me12 = x[1] + x[5];          // ME1/2, RE1/2
    int cnt_me14 = x[11];                // ME0
    int cnt_me22 = x[2] + x[10] + x[6];  // ME2, GE2/1, RE2/2
    int cnt_me23 = x[3] + x[7];          // ME3, RE3
    int cnt_me24 = x[4] + x[8];          // ME4, RE4
    int cnt_me20 = (cnt_me22 != 0) + (cnt_me23 != 0) + (cnt_me24 != 0);
    // SingleMu
    {
      bool rule_a = (cnt_me11 >= 1) and (cnt_me20 >= 2);
      bool rule_b = (cnt_me12 >= 1) and (cnt_me20 >= 1);
      bool rule_c_i = (cnt_me14 >= 1) and (cnt_me11 >= 1) and (cnt_me20 >= 1);
      bool rule_c_ii = (cnt_me14 >= 1) and (cnt_me20 >= 2);
      if (rule_a or rule_b or rule_c_i or rule_c_ii) {
        mode |= (1 << 3);
        mode |= (1 << 2);
      }
    }
    // DoubleMu
    if (mode < (1 << 3)) {
      bool rule_a_i = (cnt_me11 >= 1) and (cnt_me20 >= 1);
      bool rule_a_ii = (cnt_me12 >= 1) and (cnt_me20 >= 1);
      bool rule_b_i = (cnt_me14 >= 1) and (cnt_me11 >= 1);
      bool rule_b_ii = (cnt_me14 >= 1) and (cnt_me20 >= 1);
      if (rule_a_i or rule_a_ii or rule_b_i or rule_b_ii) {
        mode |= (1 << 3);
      }
    }
    // TripleMu
    if (mode < (1 << 2)) {
      bool rule_a_i = (cnt_me11 >= 1) and (cnt_me20 >= 1);
      bool rule_a_ii = (cnt_me12 >= 1) and (cnt_me20 >= 1);
      bool rule_b_i = (cnt_me14 >= 1) and (cnt_me11 >= 1);
      bool rule_b_ii = (cnt_me14 >= 1) and (cnt_me20 >= 1);
      bool rule_c = (cnt_me20 >= 2);
      if (rule_a_i or rule_a_ii or rule_b_i or rule_b_ii or rule_c) {
        mode |= (1 << 2);
      }
    }
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

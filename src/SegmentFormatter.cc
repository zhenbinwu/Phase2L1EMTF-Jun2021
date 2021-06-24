#include "L1Trigger/Phase2L1EMTF/interface/SegmentFormatter.h"

#include <algorithm>  // provides std::clamp, std::find_if
#include <cmath>
#include <iostream>
#include <map>
#include <tuple>
#include <utility>

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"

#include "L1Trigger/CSCCommonTrigger/interface/CSCConstants.h"
#include "L1Trigger/CSCCommonTrigger/interface/CSCPatternLUT.h"

#include "L1Trigger/Phase2L1EMTF/interface/Toolbox.h"

using namespace emtf::phase2;

struct SegmentFormatter::is_in_sector {
  constexpr bool operator()(int tp_endcap, int tp_sector) const {
    return (endcap == tp_endcap) and (sector == tp_sector);
  }

  int endcap;
  int sector;
};

struct SegmentFormatter::is_in_neighbor_sector {
  constexpr bool operator()(int tp_endcap, int tp_sector, int tp_subsector, int tp_station, int tp_cscid) const {
    bool cond_me1 =
        (tp_station == 1) and (tp_subsector == 2) and ((tp_cscid == 3) or (tp_cscid == 6) or (tp_cscid == 9));
    bool cond_non_me1 = (tp_station != 1) and ((tp_cscid == 3) or (tp_cscid == 9));
    bool cond = cond_me1 or cond_non_me1;
    return (endcap == tp_endcap) and (toolbox::prev_trigger_sector(sector) == tp_sector) and cond;
  }

  int endcap;
  int sector;
};

struct SegmentFormatter::is_in_bx_0 {
  constexpr bool operator()(int tp_bx) const { return (bx == tp_bx); }

  int bx;
};

struct SegmentFormatter::is_in_bx_0_m1 {
  constexpr bool operator()(int tp_bx) const { return (bx == tp_bx) or (bx == (tp_bx + 1)); }  // add +1 delay

  int bx;
};

struct SegmentFormatter::find_emtf_chamber {
  constexpr int operator()(int subsystem, int tp_subsector, int tp_station, int tp_cscid, bool is_neighbor) const {
    const bool is_station_one = (tp_station == 1);
    const bool is_ring_one = ((tp_cscid - 1) < 3);

    // ME1,2,3,4            :  0..17, 18..26, 27..35, 36..44
    // ME1,2,3,4 (N)        : 45..47, 48..49, 50..51, 52..53
    // RE1,2,3,4 + GE1,2    : 54..71, 72..80, 81..89, 90..98
    // RE1,2,3,4 + GE1,2 (N): 99..101, 102..103, 104..105, 106..107
    // ME0                  : 108..113
    // ME0 (N)              : 114
    constexpr int offsets[6] = {0, 45, 54, 99, 108, 114};

    switch (subsystem) {
      case L1TMuon::kCSC: {
        int chm = kInvalid;
        if (not is_neighbor) {
          chm = offsets[0] + ((is_station_one ? (tp_subsector - 1) : tp_station) * 9) + (tp_cscid - 1);
          emtf_assert((offsets[0] <= chm) and (chm < offsets[1]));
        } else {
          chm = offsets[1] + ((tp_station - 1) * 2) + (is_station_one ? ((tp_cscid - 1) / 3) : (is_ring_one ? 1 : 2));
          emtf_assert((offsets[1] <= chm) and (chm < offsets[2]));
        }
        return chm;
      }
      case L1TMuon::kRPC:
      case L1TMuon::kGEM: {
        int chm = kInvalid;
        if (not is_neighbor) {
          chm = offsets[2] + ((is_station_one ? (tp_subsector - 1) : tp_station) * 9) + (tp_cscid - 1);
          emtf_assert((offsets[2] <= chm) and (chm < offsets[3]));
        } else {
          chm = offsets[3] + ((tp_station - 1) * 2) + (is_station_one ? ((tp_cscid - 1) / 3) : (is_ring_one ? 1 : 2));
          emtf_assert((offsets[3] <= chm) and (chm < offsets[4]));
        }
        return chm;
      }
      case L1TMuon::kME0: {
        int chm = kInvalid;
        if (not is_neighbor) {
          chm = offsets[4] + ((tp_subsector - 1) * 3) + (tp_cscid - 1);
          emtf_assert((offsets[4] <= chm) and (chm < offsets[5]));
        } else {
          chm = offsets[5];
          emtf_assert(chm == offsets[5]);
        }
        return chm;
      }
      default:
        return kInvalid;
    }
  }
};

struct SegmentFormatter::find_emtf_phi {
  constexpr int operator()(int subsystem, int ph) const { return ph; }
};

struct SegmentFormatter::find_emtf_theta {
  constexpr int operator()(int subsystem, int th) const { return th; }
};

struct SegmentFormatter::find_emtf_bend {
  constexpr int operator()(int subsystem, int tp_bend) const {
    switch (subsystem) {
      case L1TMuon::kCSC:
        return std::clamp(tp_bend * 4, -16, 15);  // 5-bit, signed
      case L1TMuon::kME0:
        return std::clamp(tp_bend / 2, -64, 63);  // 7-bit, signed
      default:
        return 0;
    }
  }
};

struct SegmentFormatter::find_emtf_qual {
  constexpr int operator()(int subsystem, int tp_quality) const {
    switch (subsystem) {
      case L1TMuon::kCSC:
        return std::clamp(tp_quality, 0, 15);  // 4-bit, unsigned
      case L1TMuon::kME0:
        return std::clamp(tp_quality, 0, 15);  // 4-bit, unsigned
      default:
        return 0;
    }
  }
};

struct SegmentFormatter::find_emtf_time {
  constexpr int operator()(int subsystem, int tp_bx, int tp_subbx) const {
    switch (subsystem) {
      case L1TMuon::kRPC:
        return std::clamp((tp_bx * 16) + tp_subbx, -32, 31);  // 6-bit, signed
      default:
        return 0;
    }
  }
};

struct SegmentFormatter::find_emtf_site {
  int operator()(int subsystem, int tp_station, int tp_ring) const {
    auto akey = std::make_tuple(subsystem, tp_station, tp_ring);
    auto found = amap.find(akey);
    return (found != amap.end()) ? found->second : kInvalid;
  }

  typedef std::tuple<int, int, int> key_type;
  typedef std::map<key_type, int> map_type;
  static const map_type amap;
};

const SegmentFormatter::find_emtf_site::map_type SegmentFormatter::find_emtf_site::amap = {
    {{1, 1, 4}, 0},   // ME1/1a
    {{1, 1, 1}, 0},   // ME1/1b
    {{1, 1, 2}, 1},   // ME1/2
    {{1, 1, 3}, 1},   // ME1/3
    {{1, 2, 1}, 2},   // ME2/1
    {{1, 2, 2}, 2},   // ME2/2
    {{1, 3, 1}, 3},   // ME3/1
    {{1, 3, 2}, 3},   // ME3/2
    {{1, 4, 1}, 4},   // ME4/1
    {{1, 4, 2}, 4},   // ME4/2
    {{2, 1, 2}, 5},   // RE1/2
    {{2, 1, 3}, 5},   // RE1/3
    {{2, 2, 2}, 6},   // RE2/2
    {{2, 2, 3}, 6},   // RE2/3
    {{2, 3, 1}, 7},   // RE3/1
    {{2, 3, 2}, 7},   // RE3/2
    {{2, 3, 3}, 7},   // RE3/3
    {{2, 4, 1}, 8},   // RE4/1
    {{2, 4, 2}, 8},   // RE4/2
    {{2, 4, 3}, 8},   // RE4/3
    {{3, 1, 1}, 9},   // GE1/1
    {{3, 2, 1}, 10},  // GE2/1
    {{4, 1, 4}, 11}   // ME0
};

struct SegmentFormatter::find_emtf_host {
  int operator()(int subsystem, int tp_station, int tp_ring) const {
    auto akey = std::make_tuple(subsystem, tp_station, tp_ring);
    auto found = amap.find(akey);
    return (found != amap.end()) ? found->second : kInvalid;
  }

  typedef std::tuple<int, int, int> key_type;
  typedef std::map<key_type, int> map_type;
  static const map_type amap;
};

const SegmentFormatter::find_emtf_host::map_type SegmentFormatter::find_emtf_host::amap = {
    {{1, 1, 4}, 0},   // ME1/1a
    {{1, 1, 1}, 0},   // ME1/1b
    {{1, 1, 2}, 1},   // ME1/2
    {{1, 1, 3}, 2},   // ME1/3
    {{1, 2, 1}, 3},   // ME2/1
    {{1, 2, 2}, 4},   // ME2/2
    {{1, 3, 1}, 5},   // ME3/1
    {{1, 3, 2}, 6},   // ME3/2
    {{1, 4, 1}, 7},   // ME4/1
    {{1, 4, 2}, 8},   // ME4/2
    {{3, 1, 1}, 9},   // GE1/1
    {{2, 1, 2}, 10},  // RE1/2
    {{2, 1, 3}, 11},  // RE1/3
    {{3, 2, 1}, 12},  // GE2/1
    {{2, 2, 2}, 13},  // RE2/2
    {{2, 2, 3}, 13},  // RE2/3
    {{2, 3, 1}, 14},  // RE3/1
    {{2, 3, 2}, 15},  // RE3/2
    {{2, 3, 3}, 15},  // RE3/3
    {{2, 4, 1}, 16},  // RE4/1
    {{2, 4, 2}, 17},  // RE4/2
    {{2, 4, 3}, 17},  // RE4/3
    {{4, 1, 4}, 18}   // ME0
};

struct SegmentFormatter::find_seg_zones {
  int operator()(int emtf_host, int emtf_theta) const {
    auto found_0 = amap_0.find(emtf_host);
    auto found_1 = amap_1.find(emtf_host);
    auto found_2 = amap_2.find(emtf_host);
    bool b0 = (found_0 != amap_0.end()) and
              ((found_0->second.first <= emtf_theta) and (emtf_theta <= found_0->second.second));
    bool b1 = (found_1 != amap_1.end()) and
              ((found_1->second.first <= emtf_theta) and (emtf_theta <= found_1->second.second));
    bool b2 = (found_2 != amap_2.end()) and
              ((found_2->second.first <= emtf_theta) and (emtf_theta <= found_2->second.second));
    int word = (b0 << 2) | (b1 << 1) | (b2 << 0);
    return word;
  }

  int operator()(int emtf_host, int emtf_theta1, int emtf_theta2) const {
    auto found_0 = amap_0.find(emtf_host);
    auto found_1 = amap_1.find(emtf_host);
    auto found_2 = amap_2.find(emtf_host);
    bool b0 = (found_0 != amap_0.end()) and
              (((found_0->second.first <= emtf_theta1) and (emtf_theta1 <= found_0->second.second)) or
               ((found_0->second.first <= emtf_theta2) and (emtf_theta2 <= found_0->second.second)));
    bool b1 = (found_1 != amap_1.end()) and
              (((found_1->second.first <= emtf_theta1) and (emtf_theta1 <= found_1->second.second)) or
               ((found_1->second.first <= emtf_theta2) and (emtf_theta2 <= found_1->second.second)));
    bool b2 = (found_2 != amap_2.end()) and
              (((found_2->second.first <= emtf_theta1) and (emtf_theta1 <= found_2->second.second)) or
               ((found_2->second.first <= emtf_theta2) and (emtf_theta2 <= found_2->second.second)));
    int word = (b0 << 2) | (b1 << 1) | (b2 << 0);
    return word;
  }

  typedef std::pair<int, int> value_type;
  typedef std::map<int, value_type> map_type;
  static const map_type amap_0;  // zone 0
  static const map_type amap_1;  // zone 1
  static const map_type amap_2;  // zone 2
};

const SegmentFormatter::find_seg_zones::map_type SegmentFormatter::find_seg_zones::amap_0 = {
    {0, {4, 26}},   // ME1/1
    {3, {4, 25}},   // ME2/1
    {5, {4, 25}},   // ME3/1
    {7, {4, 25}},   // ME4/1
    {9, {17, 26}},  // GE1/1
    {12, {7, 25}},  // GE2/1
    {14, {4, 25}},  // RE3/1
    {16, {4, 25}},  // RE4/1
    {18, {4, 23}}   // ME0
};

const SegmentFormatter::find_seg_zones::map_type SegmentFormatter::find_seg_zones::amap_1 = {
    {0, {24, 53}},   // ME1/1
    {1, {46, 54}},   // ME1/2
    {3, {23, 49}},   // ME2/1
    {5, {23, 41}},   // ME3/1
    {6, {44, 54}},   // ME3/2
    {7, {23, 35}},   // ME4/1
    {8, {38, 54}},   // ME4/2
    {9, {24, 52}},   // GE1/1
    {10, {52, 56}},  // RE1/2
    {12, {23, 46}},  // GE2/1
    {14, {23, 36}},  // RE3/1
    {15, {40, 52}},  // RE3/2
    {16, {23, 31}},  // RE4/1
    {17, {35, 54}}   // RE4/2
};

const SegmentFormatter::find_seg_zones::map_type SegmentFormatter::find_seg_zones::amap_2 = {
    {1, {52, 88}},   // ME1/2
    {4, {52, 88}},   // ME2/2
    {6, {50, 88}},   // ME3/2
    {8, {50, 88}},   // ME4/2
    {10, {52, 84}},  // RE1/2
    {13, {52, 88}},  // RE2/2
    {15, {48, 84}},  // RE3/2
    {17, {52, 84}}   // RE4/2
};

struct SegmentFormatter::find_seg_timezones {
  int operator()(int emtf_host, int tp_bx) const {
    auto found_0 = amap_0.find(emtf_host);
    bool b0 = (found_0 != amap_0.end()) and
              ((found_0->second.first <= (tp_bx + 0)) and ((tp_bx + 0) <= found_0->second.second));
    bool b1 = (found_0 != amap_0.end()) and
              ((found_0->second.first <= (tp_bx + 1)) and ((tp_bx + 1) <= found_0->second.second));  // add +1 delay
    bool b2 = (found_0 != amap_0.end()) and
              ((found_0->second.first <= (tp_bx + 2)) and ((tp_bx + 2) <= found_0->second.second));  // add +2 delay
    int word = (b0 << 2) | (b1 << 1) | (b2 << 0);
    return word;
  }

  typedef std::pair<int, int> value_type;
  typedef std::map<int, value_type> map_type;
  static const map_type amap_0;  // timezone 0
};

const SegmentFormatter::find_seg_timezones::map_type SegmentFormatter::find_seg_timezones::amap_0 = {
    {0, {-1, 0}},   // ME1/1
    {1, {-1, 0}},   // ME1/2
    {2, {-1, 0}},   // ME1/3
    {3, {-1, 0}},   // ME2/1
    {4, {-1, 0}},   // ME2/2
    {5, {-1, 0}},   // ME3/1
    {6, {-1, 0}},   // ME3/2
    {7, {-1, 0}},   // ME4/1
    {8, {-1, 0}},   // ME4/2
    {9, {-1, 0}},   // GE1/1
    {10, {0, 0}},   // RE1/2
    {11, {0, 0}},   // RE1/3
    {12, {-1, 0}},  // GE2/1
    {13, {0, 0}},   // RE2/2
    {14, {0, 0}},   // RE3/1
    {15, {0, 0}},   // RE3/2
    {16, {0, 0}},   // RE4/1
    {17, {0, 0}},   // RE4/2
    {18, {0, 0}}    // ME0
};

// _____________________________________________________________________________
void SegmentFormatter::format_impl(int endcap,
                                   int sector,
                                   int bx,
                                   int strategy,
                                   const CSCGeometry& detgeom,
                                   const csc_subsystem_tag::detid_type& detid,
                                   const csc_subsystem_tag::digi_type& digi,
                                   const ChamberInfo& chminfo,
                                   EMTFHit& hit) const {
  static const int subsystem = L1TMuon::kCSC;
  static const int csc_bx_shift = -CSCConstants::LCT_CENTRAL_BX;

  const int endcap_pm = (endcap == 2) ? -1 : endcap;  // using endcap [-1,+1] convention

  // Extract from detid
  int tp_endcap = detid.endcap();
  int tp_sector = detid.triggerSector();
  int tp_station = detid.station();
  int tp_ring = detid.ring();
  int tp_chamber = detid.chamber();
  int tp_subsector = toolbox::get_trigger_subsector(tp_station, tp_chamber);
  int tp_layer = 0;

  // Extract from digi
  int tp_bx = static_cast<int>(digi.getBX()) + csc_bx_shift;
  int tp_cscid = digi.getCSCID();
  int tp_cscid_check = toolbox::get_trigger_cscid(tp_ring, tp_station, tp_chamber);
  emtf_assert(tp_cscid == tp_cscid_check);
  emtf_maybe_unused(tp_cscid_check);
  int tp_strip = digi.getStrip();  // halfstrip
  // getQuartStripBit() is not yet available in 11_1_7
  //bool tp_quart_strip_bit = digi.getQuartStripBit();
  bool tp_quart_strip_bit = 0;
  // getEighthStripBit() is not yet available in 11_1_7
  //bool tp_eighth_strip_bit = digi.getEighthStripBit();
  bool tp_eighth_strip_bit = 0;
  int tp_strip_es = (tp_strip << 2) + (tp_quart_strip_bit << 1) + (tp_eighth_strip_bit << 0);
  int tp_wire1 = digi.getKeyWG();  // wiregroup
  int tp_wire2 = 0;
  // getSlope() is not yet available in 11_1_7
  //int tp_bend = static_cast<int>(digi.getSlope()) * (static_cast<int>(digi.getBend()) * 2 - 1);
  int tp_bend = 0;
  // getQuality() is not yet available in 11_1_7
  //int tp_quality = digi.getQuality();
  int tp_quality = 6;  // using num of CLCT layers (max)
  // getRun3Pattern() is not yet available in 11_1_7
  //int tp_pattern = digi.getRun3Pattern();
  int tp_pattern = digi.getPattern();
  int tp_cscfr = toolbox::get_trigger_cscfr(tp_ring, tp_station, tp_chamber);
  int tp_subbx = 0;  // no fine resolution timing
  bool tp_valid = digi.isValid();

  // Apply ME1/1a -> ring 4 convention
  // Override tp_ring, tp_strip, tp_strip_es
  const bool is_me11a = (tp_station == 1) and (tp_ring == 1) and (tp_strip >= 128);
  if (is_me11a) {
    tp_ring = 4;
    tp_strip = static_cast<int>(digi.getStrip()) - 128;
    tp_strip_es = (tp_strip << 2) + (tp_quart_strip_bit << 1) + (tp_eighth_strip_bit << 0);
  }
  emtf_maybe_unused(tp_strip_es);  // unused in 11_1_7

#ifdef EMTF_USE_CSC_RUN2_BEND
  // Apply CSC Run 2 pattern -> bend conversion
  // Override tp_bend
  constexpr int tp_bend_lut_size = 11;
  constexpr int tp_bend_lut[tp_bend_lut_size] = {-5, -5, -4, 4, -3, 3, -2, 2, -1, 1, 0};
  emtf_assert(tp_pattern < tp_bend_lut_size);
  tp_bend = tp_bend_lut[tp_pattern];
  tp_bend *= endcap_pm;  // sign flip depending on endcap
#endif

  // Set CSC wire ambiguity
  // Override tp_wire1, tp_wire2
  emtf_assert((1 <= chminfo.wire_ambi.size()) and (chminfo.wire_ambi.size() <= 2));
  const bool has_wire_ambi = (chminfo.wire_ambi.size() > 1);
  if (has_wire_ambi) {
    tp_wire1 = chminfo.wire_ambi.at(0);
    tp_wire2 = chminfo.wire_ambi.at(1);
  }

  // Guard against unexpected data
  // It is sufficient to do this in only one sector
  if ((endcap == 1) and (sector == 1) and (bx == 0)) {
    const auto [max_strip, max_wire] = toolbox::get_csc_max_strip_and_wire(tp_station, tp_ring);
    const auto [max_pattern, max_quality] = toolbox::get_csc_max_pattern_and_quality(tp_station, tp_ring);
    emtf_assert((MIN_ENDCAP <= tp_endcap) and (tp_endcap <= MAX_ENDCAP));
    emtf_assert((MIN_TRIGSECTOR <= tp_sector) and (tp_sector <= MAX_TRIGSECTOR));
    emtf_assert((0 <= tp_subsector) and (tp_subsector <= 2));
    emtf_assert((1 <= tp_station) and (tp_station <= 4));
    emtf_assert((1 <= tp_ring) and (tp_ring <= 4));
    emtf_assert((1 <= tp_chamber) and (tp_chamber <= 36));
    emtf_assert((1 <= tp_cscid) and (tp_cscid <= 9));
    emtf_assert((0 <= tp_strip) and (tp_strip < max_strip));
    emtf_assert((0 <= tp_wire1) and (tp_wire1 < max_wire));
    emtf_assert((0 <= tp_wire2) and (tp_wire2 < max_wire));
    emtf_assert((2 <= tp_pattern) and (tp_pattern < max_pattern));
    emtf_assert((1 <= tp_quality) and (tp_quality < max_quality));
    emtf_assert(tp_valid == true);
    emtf_maybe_unused(max_strip);
    emtf_maybe_unused(max_wire);
    emtf_maybe_unused(max_pattern);
    emtf_maybe_unused(max_quality);
  }

  // Assign chamber and segment numbers
  int emtf_chamber = kInvalid;
  int emtf_segment = kInvalid;

  auto is_in_bx_fn = is_in_bx_0{bx};
  auto is_kindof_in_bx_fn = is_in_bx_0_m1{bx};
  auto is_in_sector_fn = is_in_sector{endcap, sector};
  auto is_in_neighbor_sector_fn = is_in_neighbor_sector{endcap, sector};

  const bool is_timely = (strategy == 0) ? is_in_bx_fn(tp_bx) : is_kindof_in_bx_fn(tp_bx);
  const bool is_native = is_in_sector_fn(tp_endcap, tp_sector);
  const bool is_neighbor = is_in_neighbor_sector_fn(tp_endcap, tp_sector, tp_subsector, tp_station, tp_cscid);

  if (is_timely and (is_native or is_neighbor)) {
    emtf_chamber = find_emtf_chamber{}(subsystem, tp_subsector, tp_station, tp_cscid, is_neighbor);
  }

  // Does not belong to this sector
  if (emtf_chamber == kInvalid)
    return;

  // Get global coordinates and convert them
  auto digi_w1 = digi;  // make a copy
  auto digi_w2 = digi;
  digi_w1.setStrip(tp_strip);  // patch the halfstrip number
  digi_w2.setStrip(tp_strip);
  digi_w1.setWireGroup(tp_wire1);  // patch the wiregroup number
  digi_w2.setWireGroup(tp_wire2);

  const GlobalPoint& gp_w1 = get_global_point(detgeom, detid, digi_w1);
  const GlobalPoint& gp_w2 = has_wire_ambi ? get_global_point(detgeom, detid, digi_w2) : GlobalPoint{};
  const float glob_phi = toolbox::rad_to_deg(gp_w1.phi().value());
  const float glob_theta1 = toolbox::rad_to_deg(gp_w1.theta().value());
  const float glob_theta2 = has_wire_ambi ? toolbox::rad_to_deg(gp_w2.theta().value()) : 0.;
  const float glob_time = 0.;  // no fine resolution timing
  const int ph = toolbox::calc_phi_loc_int(glob_phi, sector);
  const int th1 = toolbox::calc_theta_int(glob_theta1, endcap_pm);
  const int th2 = has_wire_ambi ? toolbox::calc_theta_int(glob_theta2, endcap_pm) : 0;
  emtf_assert((0 <= ph) and (ph < 5040));
  emtf_assert((1 <= th1) and (th1 < 128));
  emtf_assert((0 <= th2) and (th2 < 128));

  // Find EMTF variables
  const int emtf_phi = find_emtf_phi{}(subsystem, ph);
  const int emtf_bend = find_emtf_bend{}(subsystem, tp_bend);
  const int emtf_theta1 = find_emtf_theta{}(subsystem, th1);
  const int emtf_theta2 = find_emtf_theta{}(subsystem, th2);
  const int emtf_qual1 = find_emtf_qual{}(subsystem, tp_quality);
  const int emtf_qual2 = tp_pattern;
  const int emtf_time = find_emtf_time{}(subsystem, tp_bx, tp_subbx);
  const int emtf_site = find_emtf_site{}(subsystem, tp_station, tp_ring);
  const int emtf_host = find_emtf_host{}(subsystem, tp_station, tp_ring);
  const int seg_zones = find_seg_zones{}(emtf_host, emtf_theta1, emtf_theta2);
  const int seg_timezones = find_seg_timezones{}(emtf_host, tp_bx);

  // Set all the variables
  hit.setRawDetId(detid.rawId());
  hit.setSubsystem(subsystem);
  hit.setEndcap(endcap_pm);
  hit.setSector(sector);
  hit.setSubsector(tp_subsector);
  hit.setStation(tp_station);
  hit.setRing(tp_ring);
  hit.setChamber(tp_chamber);
  hit.setCscid(tp_cscid);
  hit.setStrip(tp_strip);
  hit.setStripLo(tp_strip);
  hit.setStripHi(tp_strip);
  hit.setWire1(tp_wire1);
  hit.setWire2(tp_wire2);
  hit.setBend(tp_bend);
  hit.setQuality(tp_quality);
  hit.setPattern(tp_pattern);
  hit.setNeighbor(is_neighbor);
  hit.setZones(seg_zones);
  hit.setTimezones(seg_timezones);
  hit.setCscfr(tp_cscfr);
  hit.setGemdl(tp_layer);
  hit.setSubbx(tp_subbx);
  hit.setBx(tp_bx);
  hit.setEmtfChamber(emtf_chamber);
  hit.setEmtfSegment(emtf_segment);
  hit.setEmtfPhi(emtf_phi);
  hit.setEmtfBend(emtf_bend);
  hit.setEmtfTheta1(emtf_theta1);
  hit.setEmtfTheta2(emtf_theta2);
  hit.setEmtfQual1(emtf_qual1);
  hit.setEmtfQual2(emtf_qual2);
  hit.setEmtfTime(emtf_time);
  hit.setEmtfSite(emtf_site);
  hit.setEmtfHost(emtf_host);
  hit.setGlobPhi(glob_phi);
  hit.setGlobTheta(glob_theta1);
  hit.setGlobPerp(gp_w1.perp());
  hit.setGlobZ(gp_w1.z());
  hit.setGlobTime(glob_time);
  hit.setValid(tp_valid);
}

void SegmentFormatter::format_impl(int endcap,
                                   int sector,
                                   int bx,
                                   int strategy,
                                   const RPCGeometry& detgeom,
                                   const rpc_subsystem_tag::detid_type& detid,
                                   const rpc_subsystem_tag::digi_type& digi,
                                   const ChamberInfo& chminfo,
                                   EMTFHit& hit) const {}

void SegmentFormatter::format_impl(int endcap,
                                   int sector,
                                   int bx,
                                   int strategy,
                                   const GEMGeometry& detgeom,
                                   const gem_subsystem_tag::detid_type& detid,
                                   const gem_subsystem_tag::digi_type& digi,
                                   const ChamberInfo& chminfo,
                                   EMTFHit& hit) const {}

void SegmentFormatter::format_impl(int endcap,
                                   int sector,
                                   int bx,
                                   int strategy,
                                   const ME0Geometry& detgeom,
                                   const me0_subsystem_tag::detid_type& detid,
                                   const me0_subsystem_tag::digi_type& digi,
                                   const ChamberInfo& chminfo,
                                   EMTFHit& hit) const {}

// _____________________________________________________________________________
GlobalPoint SegmentFormatter::get_global_point(const CSCGeometry& detgeom,
                                               const csc_subsystem_tag::detid_type& detid,
                                               const csc_subsystem_tag::digi_type& digi) const {
  const CSCChamber* chamb = detgeom.chamber(detid);
  assert(chamb != nullptr);  // failed to get CSC chamber

  // Local coordinates
  const uint16_t tp_strip = digi.getStrip();  // halfstrip
  const uint16_t tp_wire = digi.getKeyWG();   // wiregroup
  const uint16_t tp_pattern = digi.getPattern();

#ifdef EMTF_USE_CSC_HALFSTRIP
  // assume TMB2007 half-strips only as baseline
  const float offset = CSCPatternLUT::get2007Position(tp_pattern);
  const uint16_t halfstrip_offs = static_cast<uint16_t>(0.5f + tp_strip + offset);
  const uint16_t fullstrip = (halfstrip_offs >> 1) + 1;  // geom starts from 1
#endif

  // the rough location of the hit at the ALCT key layer
  // we will refine this using the half strip information
  const CSCLayer* layer = chamb->layer(CSCConstants::KEY_ALCT_LAYER);
  const CSCLayerGeometry* layer_geom = layer->geometry();
  const LocalPoint& coarse_lp = layer_geom->stripWireGroupIntersection(fullstrip, tp_wire);
  const GlobalPoint& coarse_gp = layer->surface().toGlobal(coarse_lp);

  // determine handedness of the chamber
  auto is_counter_clockwise_fn = [&layer]() -> bool {
    const float phi1 = layer->centerOfStrip(1).phi();
    const float phi2 = layer->centerOfStrip(2).phi();
    const float abs_diff = std::abs(phi1 - phi2);
    return ((abs_diff < M_PI) and (phi1 >= phi2)) or ((abs_diff >= M_PI) and (phi1 < phi2));
  };
  const bool ccw = is_counter_clockwise_fn();

  // the strip width/4.0 gives the offset of the half-strip
  // center with respect to the strip center
  const float hs_offset = layer_geom->stripPhiPitch() / 4.0;
  // we need to subtract the offset of even half strips and add the odd ones
  const float phi_offset = ((halfstrip_offs % 2) ? 1 : -1) * (ccw ? -hs_offset : hs_offset);

  // the global eta calculation uses the middle of the strip
  // so no need to increment it
  const GlobalPoint final_gp(
      GlobalPoint::Polar(coarse_gp.theta(), (coarse_gp.phi().value() + phi_offset), coarse_gp.mag()));
  return final_gp;
}

GlobalPoint SegmentFormatter::get_global_point(const RPCGeometry& detgeom,
                                               const rpc_subsystem_tag::detid_type& detid,
                                               const rpc_subsystem_tag::digi_type& digi) const {
  const RPCRoll* roll = detgeom.roll(detid);
  assert(roll != nullptr);  // failed to get RPC roll

  // Identifier for barrel RPC
  const bool is_barrel = (detid.region() == 0);

  // Identifier for iRPC (RE3/1, RE4/1)
  const bool is_irpc = ((not is_barrel) and (detid.station() >= 3) and (detid.ring() == 1));

  // Local coordinates
  const int tp_clus_width = digi.clusterSize();  // strip_hi - strip_lo + 1
  const int tp_strip_lo = digi.firstClusterStrip();
  const int tp_strip_hi = tp_strip_lo + tp_clus_width - 1;

  // Use half-strip precision, - 0.5f to get the center of the strip (strip starts from 1)
  const float center_of_strip = -0.5f + (0.5f * (tp_strip_lo + tp_strip_hi));
  const LocalPoint& lp_strip = roll->centreOfStrip(center_of_strip);
  const GlobalPoint& gp_strip = roll->surface().toGlobal(lp_strip);

  // Alternatively, use localPosition()
  const LocalPoint& lp = digi.localPosition();
  const GlobalPoint& gp = roll->surface().toGlobal(lp);

  const GlobalPoint final_gp(
      GlobalPoint::Polar((is_irpc ? gp.theta() : gp_strip.theta()), gp_strip.phi(), gp_strip.mag()));
  return final_gp;
}

GlobalPoint SegmentFormatter::get_global_point(const GEMGeometry& detgeom,
                                               const gem_subsystem_tag::detid_type& detid,
                                               const gem_subsystem_tag::digi_type& digi) const {
  const GEMEtaPartition* roll = detgeom.etaPartition(detid);
  assert(roll != nullptr);  // failed to get GEM roll

  // Local coordinates
  const uint16_t tp_pad_lo = digi.pads().front();
  const uint16_t tp_pad_hi = digi.pads().back();

  // Use half-pad precision, + 0.5f to get the center of the pad (pad starts from 0)
  const float center_of_pad = 0.5f + (0.5f * (tp_pad_lo + tp_pad_hi));
  const LocalPoint& lp = roll->centreOfPad(center_of_pad);
  const GlobalPoint& gp = roll->surface().toGlobal(lp);
  return gp;
}

GlobalPoint SegmentFormatter::get_global_point(const ME0Geometry& detgeom,
                                               const me0_subsystem_tag::detid_type& detid,
                                               const me0_subsystem_tag::digi_type& digi) const {
  const ME0Chamber* chamb = detgeom.chamber(detid);
  assert(chamb != nullptr);  // failed to get ME0 chamber

  // Local coordinates
  const int tp_phiposition = digi.getPhiposition();  // in half-strip unit
  const int tp_partition = digi.getPartition();      // in half-roll unit

  //const int istrip = (tp_phiposition >> 1);  // unused
  const int iroll = (tp_partition >> 1) + 1;  // geom starts from 1
  const ME0Layer* layer = chamb->layer(CSCConstants::KEY_ALCT_LAYER);
  const ME0EtaPartition* roll = layer->etaPartition(iroll);
  assert(roll != nullptr);  // failed to get ME0 roll

  // Use 1/4-strip precision, + 0.25f to get the center of half-strip (phiposition starts from 0)
  const float center_of_strip = 0.25f + (0.5f * tp_phiposition);
  const LocalPoint& lp = roll->centreOfStrip(center_of_strip);
  const GlobalPoint& gp = roll->surface().toGlobal(lp);
  return gp;
}

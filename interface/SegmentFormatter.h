#ifndef L1Trigger_Phase2L1EMTF_SegmentFormatter_h
#define L1Trigger_Phase2L1EMTF_SegmentFormatter_h

#include <array>
#include <vector>

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "L1Trigger/Phase2L1EMTF/interface/Common.h"
#include "L1Trigger/Phase2L1EMTF/interface/SubsystemTags.h"

namespace emtf {

  namespace phase2 {

    class SegmentFormatter {
    public:
      struct ChamberInfo {
        typedef std::vector<uint16_t> wire_ambi_t;
        typedef std::vector<std::array<uint16_t, 3> > copad_vec_t;

        wire_ambi_t wire_ambi;  // CSC wire ambiguity
        copad_vec_t copad_vec;  // GEM coincidence pads
      };

      template <typename T1, typename T2, typename T3>
      void format(int endcap,
                  int sector,
                  int bx,
                  int strategy,
                  const T1& detgeom,
                  const T2& detid,
                  const T3& digi,
                  const ChamberInfo& chminfo,
                  EMTFHit& hit) const {
        format_impl(endcap, sector, bx, strategy, detgeom, detid, digi, chminfo, hit);
      }

    private:
      static const int kInvalid = -99;

      // Functors
      struct is_in_sector;
      struct is_in_neighbor_sector;
      struct is_in_bx_0;
      struct is_in_bx_0_m1;

      struct find_emtf_chamber;
      struct find_emtf_phi;
      struct find_emtf_theta;
      struct find_emtf_bend;
      struct find_emtf_qual;
      struct find_emtf_time;
      struct find_emtf_site;
      struct find_emtf_host;
      struct find_seg_zones;
      struct find_seg_timezones;

      // Overloaded for CSC
      void format_impl(int endcap,
                       int sector,
                       int bx,
                       int strategy,
                       const CSCGeometry& detgeom,
                       const csc_subsystem_tag::detid_type& detid,
                       const csc_subsystem_tag::digi_type& digi,
                       const ChamberInfo& chminfo,
                       EMTFHit& hit) const;

      // Overloaded for RPC
      void format_impl(int endcap,
                       int sector,
                       int bx,
                       int strategy,
                       const RPCGeometry& detgeom,
                       const rpc_subsystem_tag::detid_type& detid,
                       const rpc_subsystem_tag::digi_type& digi,
                       const ChamberInfo& chminfo,
                       EMTFHit& hit) const;

      // Overloaded for GEM
      void format_impl(int endcap,
                       int sector,
                       int bx,
                       int strategy,
                       const GEMGeometry& detgeom,
                       const gem_subsystem_tag::detid_type& detid,
                       const gem_subsystem_tag::digi_type& digi,
                       const ChamberInfo& chminfo,
                       EMTFHit& hit) const;

      // Overloaded for ME0
      void format_impl(int endcap,
                       int sector,
                       int bx,
                       int strategy,
                       const ME0Geometry& detgeom,
                       const me0_subsystem_tag::detid_type& detid,
                       const me0_subsystem_tag::digi_type& digi,
                       const ChamberInfo& chminfo,
                       EMTFHit& hit) const;

      // Convert to global coordinates
      GlobalPoint get_global_point(const CSCGeometry& detgeom,
                                   const csc_subsystem_tag::detid_type& detid,
                                   const csc_subsystem_tag::digi_type& digi) const;

      GlobalPoint get_global_point(const RPCGeometry& detgeom,
                                   const rpc_subsystem_tag::detid_type& detid,
                                   const rpc_subsystem_tag::digi_type& digi) const;

      GlobalPoint get_global_point(const GEMGeometry& detgeom,
                                   const gem_subsystem_tag::detid_type& detid,
                                   const gem_subsystem_tag::digi_type& digi) const;

      GlobalPoint get_global_point(const ME0Geometry& detgeom,
                                   const me0_subsystem_tag::detid_type& detid,
                                   const me0_subsystem_tag::digi_type& digi) const;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_SegmentFormatter_h not defined

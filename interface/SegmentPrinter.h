#ifndef L1Trigger_Phase2L1EMTF_SegmentPrinter_h
#define L1Trigger_Phase2L1EMTF_SegmentPrinter_h

#include "L1Trigger/Phase2L1EMTF/interface/Common.h"
#include "L1Trigger/Phase2L1EMTF/interface/SubsystemTags.h"

namespace emtf {

  namespace phase2 {

    class SegmentPrinter {
    public:
      template <typename T1, typename T2>
      void print(const T1& detid, const T2& digi) const {
        print_impl(detid, digi);
      }

      template <typename T>
      void print(const T& thing) const {
        print_impl(thing);
      }

    private:
      struct compress_sector;
      struct compress_cscid;
      struct pretty_print;

      // Overloaded for CSC
      void print_impl(const csc_subsystem_tag::detid_type& detid, const csc_subsystem_tag::digi_type& digi) const;

      // Overloaded for RPC
      void print_impl(const rpc_subsystem_tag::detid_type& detid, const rpc_subsystem_tag::digi_type& digi) const;

      // Overloaded for GEM
      void print_impl(const gem_subsystem_tag::detid_type& detid, const gem_subsystem_tag::digi_type& digi) const;

      // Overloaded for ME0
      void print_impl(const me0_subsystem_tag::detid_type& detid, const me0_subsystem_tag::digi_type& digi) const;

      // Overloaded for converted EMTF hit
      void print_impl(const EMTFHit& hit) const;

      // Overloaded for EMTF track
      void print_impl(const EMTFTrack& trk) const;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_SegmentPrinter_h not defined

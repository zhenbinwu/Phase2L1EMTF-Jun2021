#ifndef L1Trigger_Phase2L1EMTF_TrackFormatter_h
#define L1Trigger_Phase2L1EMTF_TrackFormatter_h

#include <vector>

#include "L1Trigger/Phase2L1EMTF/interface/Common.h"

namespace emtf {

  namespace phase2 {

    class TrackFormatter {
    public:
      typedef std::vector<int> Vector;  // 1-D vector containing tensor data

      typedef EMTFTrack::seg_ref_array_t seg_ref_array_t;
      typedef EMTFTrack::seg_valid_array_t seg_valid_array_t;

      void format(int endcap,
                  int sector,
                  int bx,
                  unsigned model_version,
                  bool unconstrained,
                  const Vector& trk_data,
                  EMTFTrack& trk) const;

    private:
      static const int kInvalid = -99;

      // Functors
      struct find_hw_pt;
      struct find_hw_eta;
      struct find_hw_phi;
      struct find_hw_d0;
      struct find_hw_z0;
      struct find_hw_beta;
      struct find_hw_charge;
      struct find_hw_qual;
      struct find_emtf_pt_no_calib;
      struct find_emtf_pt;
      struct find_emtf_mode_v1;
      struct find_emtf_mode_v2;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_TrackFormatter_h not defined

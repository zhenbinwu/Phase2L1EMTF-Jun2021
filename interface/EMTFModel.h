#ifndef L1Trigger_Phase2L1EMTF_EMTFModel_h
#define L1Trigger_Phase2L1EMTF_EMTFModel_h

#include <vector>

#include "L1Trigger/Phase2L1EMTF/interface/NdArrayDesc.h"

namespace emtf {

  namespace phase2 {

    class EMTFModel {
    public:
      typedef std::vector<int> Vector;  // 1-D vector containing tensor data

      explicit EMTFModel(unsigned version = 3, bool unconstrained = false);
      ~EMTFModel();

      void setVersion(unsigned version) { version_ = version; }

      void setUnconstrained(bool unconstrained) { unconstrained_ = unconstrained; }

      unsigned version() const { return version_; }

      bool unconstrained() const { return unconstrained_; }

      // Get model input shape
      NdArrayDesc get_input_shape() const;

      // Get model output shape
      NdArrayDesc get_output_shape() const;

      // Get max num of segments
      int get_num_segments() const;

      // Get max num of tracks
      int get_num_tracks() const;

      // Fit
      void fit(const Vector& in0, Vector& out) const {
        const NdArrayDesc& input_shape = get_input_shape();
        const NdArrayDesc& output_shape = get_output_shape();
        assert(in0.size() == input_shape.num_elements());
        assert(out.size() == output_shape.num_elements());

        if (version_ == 3) {
          fit_impl_v3(in0, out);
        }
      }

    private:
      void fit_impl_v3(const Vector& in0, Vector& out) const;

      static constexpr int num_emtf_chambers_v3 = 115;      // per sector
      static constexpr int num_emtf_segments_v3 = 2;        // per chamber
      static constexpr int num_emtf_variables_v3 = 13;      // per segment
      static constexpr int num_emtf_tracks_v3 = 4;          // per sector
      static constexpr int num_emtf_trk_variables_v3 = 54;  // per track

      unsigned version_;    // model version
      bool unconstrained_;  // unconstrained fit
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_EMTFModel_h not defined

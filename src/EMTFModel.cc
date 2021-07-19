#include "L1Trigger/Phase2L1EMTF/interface/EMTFModel.h"

using namespace emtf::phase2;

EMTFModel::EMTFModel(unsigned version, bool unconstrained) : version_(version), unconstrained_(unconstrained) {}

EMTFModel::~EMTFModel() {}

NdArrayDesc EMTFModel::get_input_shape() const {
  if (version_ == 3) {
    int n0 = num_emtf_chambers_v3 * num_emtf_segments_v3;
    int n1 = num_emtf_variables_v3;
    return NdArrayDesc({n0, n1});
  }
  return NdArrayDesc{};
}

NdArrayDesc EMTFModel::get_output_shape() const {
  if (version_ == 3) {
    int n0 = num_emtf_tracks_v3;
    int n1 = num_emtf_trk_variables_v3;
    return NdArrayDesc({n0, n1});
  }
  return NdArrayDesc{};
}

int EMTFModel::get_num_segments() const {
  if (version_ == 3) {
    return num_emtf_segments_v3;
  }
  return 0;
}

int EMTFModel::get_num_tracks() const {
  if (version_ == 3) {
    return num_emtf_tracks_v3;
  }
  return 0;
}

void EMTFModel::fit_impl_v3(const Vector& in0, Vector& out) const {}

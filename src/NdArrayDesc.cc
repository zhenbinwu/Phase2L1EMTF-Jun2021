#include "L1Trigger/Phase2L1EMTF/interface/NdArrayDesc.h"

using namespace emtf::phase2;

NdArrayDesc::NdArrayDesc() : extents_{{0, 0, 0, 0}}, strides_{{0, 0, 0, 0}}, num_elements_(0), num_dimensions_(0) {}

NdArrayDesc::~NdArrayDesc() {}

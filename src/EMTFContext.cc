#include "L1Trigger/Phase2L1EMTF/interface/EMTFContext.h"

#include "L1Trigger/Phase2L1EMTF/interface/VersionControl.h"

using namespace emtf::phase2;

EMTFContext::EMTFContext(const edm::ParameterSet& iConfig)
    : pset_(iConfig), version_control_(std::make_unique<VersionControl>()) {}

EMTFContext::~EMTFContext() {}

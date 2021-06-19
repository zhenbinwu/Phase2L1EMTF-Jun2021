#include "L1Trigger/Phase2L1EMTF/interface/EMTFWorker.h"

using namespace emtf::phase2;

EMTFWorker::EMTFWorker(const edm::ParameterSet& iConfig, edm::ConsumesCollector&& iConsumes) : pset_(iConfig) {}

EMTFWorker::~EMTFWorker() {}

void EMTFWorker::before_process(const EMTFContext& iContext, const edm::EventSetup& iSetup) {}

void EMTFWorker::process(const edm::Event& iEvent, EMTFHitCollection& out_hits, EMTFTrackCollection& out_tracks) const {
}

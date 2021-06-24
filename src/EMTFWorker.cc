#include "L1Trigger/Phase2L1EMTF/interface/EMTFWorker.h"

#include "L1Trigger/Phase2L1EMTF/interface/EMTFContext.h"
#include "L1Trigger/Phase2L1EMTF/interface/EMTFModel.h"
#include "L1Trigger/Phase2L1EMTF/interface/GeometryHelper.h"
#include "L1Trigger/Phase2L1EMTF/interface/ConditionHelper.h"
#include "L1Trigger/Phase2L1EMTF/interface/SectorProcessor.h"
#include "L1Trigger/Phase2L1EMTF/interface/SubsystemTags.h"
#include "L1Trigger/Phase2L1EMTF/interface/SubsystemCollection.h"
#include "L1Trigger/Phase2L1EMTF/interface/SubsystemCollector.h"

using namespace emtf::phase2;

EMTFWorker::EMTFWorker(const edm::ParameterSet& iConfig, edm::ConsumesCollector&& iConsumes)
    : pset_(iConfig),
      model_(std::make_unique<EMTFModel>()),
      geom_helper_(std::make_unique<GeometryHelper>(iConsumes)),
      cond_helper_(std::make_unique<ConditionHelper>(iConsumes)),
      cscToken_(
          iConsumes.consumes<csc_subsystem_tag::collection_type>(iConfig.getParameter<edm::InputTag>("cscLabel"))),
      rpcToken_(
          iConsumes.consumes<rpc_subsystem_tag::collection_type>(iConfig.getParameter<edm::InputTag>("rpcLabel"))),
      gemToken_(
          iConsumes.consumes<gem_subsystem_tag::collection_type>(iConfig.getParameter<edm::InputTag>("gemLabel"))),
      me0Token_(
          iConsumes.consumes<me0_subsystem_tag::collection_type>(iConfig.getParameter<edm::InputTag>("me0Label"))),
      cscEnable_(iConfig.getParameter<bool>("cscEnable")),
      rpcEnable_(iConfig.getParameter<bool>("rpcEnable")),
      gemEnable_(iConfig.getParameter<bool>("gemEnable")),
      me0Enable_(iConfig.getParameter<bool>("me0Enable")),
      minBX_(iConfig.getParameter<int>("minBX")),
      maxBX_(iConfig.getParameter<int>("maxBX")),
      bxWindow_(iConfig.getParameter<int>("bxWindow")),
      verbose_(iConfig.getUntrackedParameter<int>("verbosity", 0)) {}

EMTFWorker::~EMTFWorker() {}

void EMTFWorker::before_process(const EMTFContext& iContext, const edm::EventSetup& iSetup) {
  // Check and update based on EventSetup data
  geom_helper_->check(iSetup);
  cond_helper_->check(iSetup);
}

void EMTFWorker::process(const edm::Event& iEvent, EMTFHitCollection& out_hits, EMTFTrackCollection& out_tracks) const {
  // Extract trigger primitives
  SubsystemCollector collector;
  SubsystemCollection muon_primitives;

  if (cscEnable_) {
    collector.collect<csc_subsystem_tag>(iEvent, cscToken_, muon_primitives);
  }
  if (rpcEnable_) {
    collector.collect<rpc_subsystem_tag>(iEvent, rpcToken_, muon_primitives);
  }
  if (gemEnable_) {
    collector.collect<gem_subsystem_tag>(iEvent, gemToken_, muon_primitives);
  }
  if (me0Enable_) {
    collector.collect<me0_subsystem_tag>(iEvent, me0Token_, muon_primitives);
  }

  // Run the sector processors
  for (int endcap = MIN_ENDCAP; endcap <= MAX_ENDCAP; ++endcap) {
    for (int sector = MIN_TRIGSECTOR; sector <= MAX_TRIGSECTOR; ++sector) {
      SectorProcessor processor;
      const edm::EventID& evt_id = iEvent.id();
      processor.process(*this, endcap, sector, evt_id, muon_primitives, out_hits, out_tracks);
    }
  }
}

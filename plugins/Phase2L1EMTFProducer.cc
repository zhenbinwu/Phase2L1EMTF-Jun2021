// system include files
#include <cassert>
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "L1Trigger/Phase2L1EMTF/interface/EMTFContext.h"
#include "L1Trigger/Phase2L1EMTF/interface/EMTFWorker.h"

class Phase2L1EMTFProducer : public edm::stream::EDProducer<edm::GlobalCache<emtf::phase2::EMTFContext> > {
public:
  using global_cache_t = emtf::phase2::EMTFContext;
  using global_cache_pointer = std::unique_ptr<global_cache_t>;

  explicit Phase2L1EMTFProducer(const edm::ParameterSet&, const global_cache_t*);
  ~Phase2L1EMTFProducer() override;

  static global_cache_pointer initializeGlobalCache(const edm::ParameterSet&);
  static void globalEndJob(const global_cache_t*);

  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  void produce(edm::Event&, const edm::EventSetup&) final;

private:
  std::unique_ptr<emtf::phase2::EMTFWorker> worker_;

  // Output tokens
  const edm::EDPutTokenT<emtf::phase2::EMTFHitCollection> hitToken_;
  const edm::EDPutTokenT<emtf::phase2::EMTFTrackCollection> trkToken_;
};

// _____________________________________________________________________________
// Constructor with access to the GlobalCache object.
Phase2L1EMTFProducer::Phase2L1EMTFProducer(const edm::ParameterSet& iConfig, const global_cache_t* iContext)
    : worker_(std::make_unique<emtf::phase2::EMTFWorker>(iConfig, consumesCollector())),
      hitToken_(produces<emtf::phase2::EMTFHitCollection>()),
      trkToken_(produces<emtf::phase2::EMTFTrackCollection>()) {}

Phase2L1EMTFProducer::~Phase2L1EMTFProducer() {}

// This static function is called only once before the constructor is called.
Phase2L1EMTFProducer::global_cache_pointer Phase2L1EMTFProducer::initializeGlobalCache(
    const edm::ParameterSet& iConfig) {
  return std::make_unique<global_cache_t>(iConfig);
}

// This static function is called only once at the end of the job.
void Phase2L1EMTFProducer::globalEndJob(const global_cache_t* iContext) {}

// This is called by multiple streams.
void Phase2L1EMTFProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  emtf::phase2::EMTFHitCollection out_hits;
  emtf::phase2::EMTFTrackCollection out_tracks;

  // Access the GlobalCache object
  const global_cache_t* iContext = globalCache();
  assert(iContext != nullptr);

  // Dispatch
  worker_->before_process(*iContext, iSetup);      // non-const function
  worker_->process(iEvent, out_hits, out_tracks);  // const function

  // Output the products
  iEvent.emplace(hitToken_, std::move(out_hits));
  iEvent.emplace(trkToken_, std::move(out_tracks));
}

// This static function provides the configuration parameters.
void Phase2L1EMTFProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("cscLabel", edm::InputTag("simCscTriggerPrimitiveDigisForEMTF", "MPCSORTED"));
  desc.add<edm::InputTag>("rpcLabel", edm::InputTag("rpcRecHitsForEMTF"));
  desc.add<edm::InputTag>("gemLabel", edm::InputTag("simMuonGEMPadDigiClusters"));
  desc.add<edm::InputTag>("me0Label", edm::InputTag("me0TriggerConvertedPseudoDigis"));
  desc.add<bool>("cscEnable", true);
  desc.add<bool>("rpcEnable", true);
  desc.add<bool>("gemEnable", true);
  desc.add<bool>("me0Enable", true);
  desc.add<int>("minBX", -2);
  desc.add<int>("maxBX", 2);
  desc.add<int>("bxWindow", 1);
  desc.addUntracked<int>("verbosity", 0);
  descriptions.add("phase2L1EMTFProducer", desc);

  //edm::ParameterSetDescription default_desc;
  //default_desc.setUnknown();
  //descriptions.addDefault(default_desc);
}

// define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(Phase2L1EMTFProducer);

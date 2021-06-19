#ifndef L1Trigger_Phase2L1EMTF_EMTFWorker_h
#define L1Trigger_Phase2L1EMTF_EMTFWorker_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "L1Trigger/Phase2L1EMTF/interface/Common.h"
#include "L1Trigger/Phase2L1EMTF/interface/EMTFContext.h"

namespace emtf {

  namespace phase2 {

    class EMTFWorker {
    public:
      explicit EMTFWorker(const edm::ParameterSet& iConfig, edm::ConsumesCollector&& iConsumes);
      ~EMTFWorker();

      void before_process(const EMTFContext& iContext, const edm::EventSetup& iSetup);

      void process(const edm::Event& iEvent, EMTFHitCollection& out_hits, EMTFTrackCollection& out_tracks) const;

    private:
      const edm::ParameterSet& pset_;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_EMTFWorker_h not defined

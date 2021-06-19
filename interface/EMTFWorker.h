#ifndef L1Trigger_Phase2L1EMTF_EMTFWorker_h
#define L1Trigger_Phase2L1EMTF_EMTFWorker_h

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "L1Trigger/Phase2L1EMTF/interface/Common.h"

namespace emtf {

  namespace phase2 {

    class EMTFContext;
    class EMTFModel;
    class GeometryHelper;
    class ConditionHelper;
    class SectorProcessor;

    class EMTFWorker {
    public:
      friend class SectorProcessor;  // allow access to helper objects

      explicit EMTFWorker(const edm::ParameterSet& iConfig, edm::ConsumesCollector&& iConsumes);
      ~EMTFWorker();

      void before_process(const EMTFContext& iContext, const edm::EventSetup& iSetup);

      void process(const edm::Event& iEvent, EMTFHitCollection& out_hits, EMTFTrackCollection& out_tracks) const;

    private:
      const edm::ParameterSet& pset_;

      // Helper objects
      std::unique_ptr<EMTFModel> model_;
      std::unique_ptr<GeometryHelper> geom_helper_;
      std::unique_ptr<ConditionHelper> cond_helper_;

      // Subsystem tokens
      const edm::EDGetToken cscToken_;
      const edm::EDGetToken rpcToken_;
      const edm::EDGetToken gemToken_;
      const edm::EDGetToken me0Token_;

      // Subsystem enables
      const bool cscEnable_;
      const bool rpcEnable_;
      const bool gemEnable_;
      const bool me0Enable_;

      // BX window
      const int minBX_;
      const int maxBX_;
      const int bxWindow_;

      // Verbosity level
      int verbose_;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_EMTFWorker_h not defined

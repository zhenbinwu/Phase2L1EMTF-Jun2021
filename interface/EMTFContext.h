#ifndef L1Trigger_Phase2L1EMTF_EMTFContext_h
#define L1Trigger_Phase2L1EMTF_EMTFContext_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "L1Trigger/Phase2L1EMTF/interface/Common.h"

namespace emtf {

  namespace phase2 {

    class EMTFWorker;
    class VersionControl;

    class EMTFContext {
    public:
      friend class EMTFWorker;  // allow access to helper objects

      explicit EMTFContext(const edm::ParameterSet& iConfig);
      ~EMTFContext();

    private:
      const edm::ParameterSet& pset_;

      // Helper objects
      std::unique_ptr<VersionControl> version_control_;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_EMTFContext_h not defined

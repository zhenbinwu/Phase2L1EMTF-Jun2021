#ifndef L1Trigger_Phase2L1EMTF_EMTFContext_h
#define L1Trigger_Phase2L1EMTF_EMTFContext_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"

namespace emtf {

  namespace phase2 {

    class EMTFContext {
    public:
      explicit EMTFContext(const edm::ParameterSet& iConfig);
      ~EMTFContext();

    private:
      const edm::ParameterSet& pset_;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_EMTFContext_h not defined

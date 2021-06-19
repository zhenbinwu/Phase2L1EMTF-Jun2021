#ifndef L1Trigger_Phase2L1EMTF_ConditionHelper_h
#define L1Trigger_Phase2L1EMTF_ConditionHelper_h

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ESWatcher.h"

#include "CondFormats/L1TObjects/interface/L1TMuonEndCapParams.h"
#include "CondFormats/L1TObjects/interface/L1TMuonEndCapForest.h"
#include "CondFormats/DataRecord/interface/L1TMuonEndCapParamsRcd.h"
#include "CondFormats/DataRecord/interface/L1TMuonEndCapForestRcd.h"

namespace emtf {

  namespace phase2 {

    class ConditionHelper {
    public:
      explicit ConditionHelper(edm::ConsumesCollector&& iConsumes);
      explicit ConditionHelper(edm::ConsumesCollector& iConsumes);
      ~ConditionHelper();

      bool check(const edm::EventSetup& iSetup);

      constexpr const L1TMuonEndCapParams& getParams() const { return *params_; }
      constexpr const L1TMuonEndCapForest& getForest() const { return *forest_; }

    private:
      // ESWatcher functions
      void watch_params(const L1TMuonEndCapParamsRcd& record) { params_ = &(record.get(paramsToken_)); }
      void watch_forest(const L1TMuonEndCapForestRcd& record) { forest_ = &(record.get(forestToken_)); }

      // ESGetToken
      edm::ESGetToken<L1TMuonEndCapParams, L1TMuonEndCapParamsRcd> paramsToken_;
      edm::ESGetToken<L1TMuonEndCapForest, L1TMuonEndCapForestRcd> forestToken_;

      // ESWatcher
      edm::ESWatcher<L1TMuonEndCapParamsRcd> paramsWatcher_;
      edm::ESWatcher<L1TMuonEndCapForestRcd> forestWatcher_;

      // ESHandle products
      // The products are returned as raw pointer. Please use with care.
      const L1TMuonEndCapParams* params_;
      const L1TMuonEndCapForest* forest_;
    };

  }  // namespace phase2

}  // namespace emtf

#endif  // L1Trigger_Phase2L1EMTF_ConditionHelper_h not defined

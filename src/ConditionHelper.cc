#include "L1Trigger/Phase2L1EMTF/interface/ConditionHelper.h"

using namespace emtf::phase2;

ConditionHelper::ConditionHelper(edm::ConsumesCollector&& iConsumes)
    : paramsToken_(iConsumes.esConsumes<L1TMuonEndCapParams, L1TMuonEndCapParamsRcd>()),
      forestToken_(iConsumes.esConsumes<L1TMuonEndCapForest, L1TMuonEndCapForestRcd>()),
      paramsWatcher_(this, &ConditionHelper::watch_params),
      forestWatcher_(this, &ConditionHelper::watch_forest),
      params_(nullptr),
      forest_(nullptr) {}

ConditionHelper::ConditionHelper(edm::ConsumesCollector& iConsumes)
    : paramsToken_(iConsumes.esConsumes<L1TMuonEndCapParams, L1TMuonEndCapParamsRcd>()),
      forestToken_(iConsumes.esConsumes<L1TMuonEndCapForest, L1TMuonEndCapForestRcd>()),
      paramsWatcher_(this, &ConditionHelper::watch_params),
      forestWatcher_(this, &ConditionHelper::watch_forest),
      params_(nullptr),
      forest_(nullptr) {}

ConditionHelper::~ConditionHelper() {}

bool ConditionHelper::check(const edm::EventSetup& iSetup) {
  bool changed = false;
  bool changed_i = false;
  changed_i = paramsWatcher_.check(iSetup);
  changed |= changed_i;
  changed_i = forestWatcher_.check(iSetup);
  changed |= changed_i;
  return changed;
}

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

class Phase2L1EMTFProducer : public edm::stream::EDProducer<> {
public:
  explicit Phase2L1EMTFProducer(const edm::ParameterSet&);
  ~Phase2L1EMTFProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions&);

private:
  void produce(edm::Event&, const edm::EventSetup&) override;
};

Phase2L1EMTFProducer::Phase2L1EMTFProducer(const edm::ParameterSet& iConfig) {}

Phase2L1EMTFProducer::~Phase2L1EMTFProducer() {}

void Phase2L1EMTFProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {}

void Phase2L1EMTFProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription default_desc;
  default_desc.setUnknown();
  descriptions.addDefault(default_desc);
}

// define this as a plug-in
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(Phase2L1EMTFProducer);

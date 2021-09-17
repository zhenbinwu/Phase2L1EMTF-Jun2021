import FWCore.ParameterSet.Config as cms

from L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi import cscTriggerPrimitiveDigis

# Taken from L1Trigger.L1TMuon.simDigis_cff
simCscTriggerPrimitiveDigisForEMTF = cscTriggerPrimitiveDigis.clone(
    CSCComparatorDigiProducer = 'simMuonCSCDigis:MuonCSCComparatorDigi',
    CSCWireDigiProducer = 'simMuonCSCDigis:MuonCSCWireDigi'
)

# Taken from L1Trigger.CSCTriggerPrimitives.cscTriggerPrimitiveDigis_cfi
from Configuration.Eras.Modifier_run3_GEM_cff import run3_GEM
run3_GEM.toModify(simCscTriggerPrimitiveDigisForEMTF,
                  commonParam = dict(isSLHC = cms.bool(True),
                                     runME11Up = cms.bool(True),
                                     runME11ILT = cms.bool(False),  # was: True
                                     useClusters = cms.bool(False),
                                     enableAlctSLHC = cms.bool(False)))  # was: True

from Configuration.Eras.Modifier_phase2_muon_cff import phase2_muon
phase2_muon.toModify(simCscTriggerPrimitiveDigisForEMTF,
                     commonParam = dict(runME21Up = cms.bool(True),
                                        runME21ILT = cms.bool(False),  # was: True
                                        runME31Up = cms.bool(True),
                                        runME41Up = cms.bool(True),
                                        enableAlctSLHC = cms.bool(False)),  # was: True
                     alctSLHCME21 = cscTriggerPrimitiveDigis.alctParam07.clone(alctNplanesHitPattern = 4),  # was: alctSLHC.clone(...)
                     alctSLHCME3141 = cscTriggerPrimitiveDigis.alctParam07.clone(alctNplanesHitPattern = 4))

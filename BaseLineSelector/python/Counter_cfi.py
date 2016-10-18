import FWCore.ParameterSet.Config as cms

#-------------------------------------------------------------------------
#   Weight Counter
#-------------------------------------------------------------------------
beforeAny = cms.EDProducer(
    "WeightCounter",
    lhesrc=cms.InputTag("externalLHEProducer"),
    label=cms.string("Before Any Selection"),
)
afterHLT = cms.EDProducer(
    "WeightCounter",
    lhesrc=cms.InputTag("externalLHEProducer"),
    label=cms.string("After HLT selection"),
)

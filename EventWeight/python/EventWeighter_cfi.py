import FWCore.ParameterSet.Config as cms

EventWeight = cms.EDProducer(
    "EventWeighter",
    lhesrc=cms.InputTag('externalLHEProducer'),
    pusrc =cms.InputTag('slimmedAddPileupInfo')
)

import FWCore.ParameterSet.Config as cms

SupSelector = cms.EDFilter(
    "SupSelector",
    muonsrc=cms.InputTag("skimmedPatMuons"),
    electronsrc=cms.InputTag("skimmedPatElectrons"),
    jetsrc=cms.InputTag("skimmedPatJets")
)

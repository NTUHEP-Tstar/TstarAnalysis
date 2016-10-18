import FWCore.ParameterSet.Config as cms

METFilter = cms.EDFilter(
    "METFilter",
    badchhadsrc = cms.InputTag("BadChargedCandidateFilter"),
    badmusrc    = cms.InputTag("BadPFMuonFilter"),
    recotrgsrc  = cms.InputTag("TriggerResults","","RECO")
)

leptonSeparator = cms.EDFilter(
    "LeptonSeparator",
    muonsrc=cms.InputTag("selectedMuons"),
    electronsrc=cms.InputTag("selectedElectrons"),
    reqmuon = cms.int32(0),
    reqelec = cms.int32(0)
)

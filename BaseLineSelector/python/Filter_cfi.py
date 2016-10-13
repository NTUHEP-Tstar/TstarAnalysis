import FWCore.ParameterSet.Config as cms

METFilter = cms.EDFilter(
    "METFilter",
    badchhadsrc = cms.InputTag("BadChargedCandidateFilter"),
    badmusrc    = cms.InputTag("BadPFMuonFilter"),
    recotrgsrc  = cms.InputTag("TriggerResults","","RECO")
)

tstarFilter = cms.EDFilter(
    "BaseLineSelector",
    muonsrc=cms.InputTag("selectedMuons"),
    electronsrc=cms.InputTag("selectedElectrons"),
    jetsrc=cms.InputTag("selectedJets"),
)

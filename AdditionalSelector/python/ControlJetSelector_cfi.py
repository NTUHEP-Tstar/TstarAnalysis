import FWCore.ParameterSet.Config as cms

controlregion = cms.EDFilter(
    "ControlJetSelector",
    jetsrc=cms.InputTag("skimmedPatJets"),
    muonsrc=cms.InputTag("skimmedPatMuons"),
    elecsrc=cms.InputTag("skimmedPatElectrons"),
    maxjet=cms.int32(-1),
    minjet=cms.int32(6),
    btagchecker=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2Moriond17_2017_1_26_BtoH.csv"),
    checkorder=cms.int32(-1),
    maxpassmedium=cms.int32(0),
    minpassmedium=cms.int32(0),
    maxfailloose=cms.int32(-1),
    minfailloose=cms.int32(2),
)

signalregion = cms.EDFilter(
    "ControlJetSelector",
    jetsrc=cms.InputTag("skimmedPatJets"),
    muonsrc=cms.InputTag("skimmedPatMuons"),
    elecsrc=cms.InputTag("skimmedPatElectrons"),
    maxjet=cms.int32(-1),
    minjet=cms.int32(6),
    btagchecker=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2Moriond17_2017_1_26_BtoH.csv"),
    checkorder=cms.int32(-1),
    maxpassmedium=cms.int32(2),
    minpassmedium=cms.int32(2),
    maxfailloose=cms.int32(-1),
    minfailloose=cms.int32(2),
)

toplikeregion = cms.EDFilter(
    "ControlJetSelector",
    jetsrc=cms.InputTag("skimmedPatJets"),
    muonsrc=cms.InputTag("skimmedPatMuons"),
    elecsrc=cms.InputTag("skimmedPatElectrons"),
    maxjet=cms.int32(-1),
    minjet=cms.int32(4),
    btagchecker=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2Moriond17_2017_1_26_BtoH.csv"),
    checkorder=cms.int32(-1),
    maxpassmedium=cms.int32(2),
    minpassmedium=cms.int32(2),
    maxfailloose=cms.int32(-1),
    minfailloose=cms.int32(0),
)

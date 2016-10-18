import FWCore.ParameterSet.Config as cms

controlregion = cms.EDFilter(
    "ControlJetSelector",
    jetsrc=cms.InputTag("skimmedPatJets"),
    maxjet=cms.int32(-1),
    minjet=cms.int32(6),
    btagchecker=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
    checkorder=cms.int32(6),
    maxpassmedium=cms.int32(0),
    minpassmedium=cms.int32(0),
    maxfailloose=cms.int32(-1),
    minfailloose=cms.int32(2),
)

signalregion = cms.EDFilter(
    "ControlJetSelector",
    jetsrc=cms.InputTag("skimmedPatJets"),
    maxjet=cms.int32(-1),
    minjet=cms.int32(6),
    btagchecker=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
    checkorder=cms.int32(6),
    maxpassmedium=cms.int32(-1),
    minpassmedium=cms.int32(1),
    maxfailloose=cms.int32(-1),
    minfailloose=cms.int32(0),
)

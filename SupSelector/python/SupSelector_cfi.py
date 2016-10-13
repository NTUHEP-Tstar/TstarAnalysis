import FWCore.ParameterSet.Config as cms

controlregion = cms.EDFilter(
    "ControlJetSelector",
    jetsrc=cms.InputTag("skimmedPatJets"),
    maxjet=cms.int(-1),
    minjet=cms.int(6),
    btagchecker=cms.FileInPath("TstarAnalysis/Common/settings/CSVv2_ichep.csv"),
    checkorder=cms.int(6),
    maxpassmedium=cms.int(0),
    minpassmedium=cms.int(0),
    maxfailloose=cms.int(-1),
    minfailloose=cms.int(2),
)

signalregion = cms.EDFilter(
    "ControlJetSelector",
    jetsrc=cms.InputTag("skimmedPatJets"),
    maxjet=cms.int(-1),
    minjet=cms.int(6),
    btagchecker=cms.FileInPath("TstarAnalysis/Common/settings/CSVv2_ichep.csv"),
    checkorder=cms.int(6),
    maxpassmedium=cms.int(-1),
    minpassmedium=cms.int(2),
    maxfailloose=cms.int(-1),
    minfailloose=cms.int(0),
)

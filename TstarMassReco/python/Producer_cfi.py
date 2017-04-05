import FWCore.ParameterSet.Config as cms

tstarMassReco = cms.EDProducer(
    "ChiSqMassReco",
    metsrc      = cms.InputTag( "slimmedMETs" ),
    muonsrc     = cms.InputTag( "skimmedPatMuons" ),
    electronsrc = cms.InputTag( "skimmedPatElectrons" ),
    jetsrc      = cms.InputTag( "skimmedPatJets" ),
    MaxJet      = cms.untracked.int32(6),
    ReqBJet     = cms.untracked.int32(2),
    btagfile    = cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)

tstarMassReco7jet = cms.EDProducer(
    "ChiSqMassReco",
    metsrc      = cms.InputTag( "slimmedMETs" ),
    muonsrc     = cms.InputTag( "skimmedPatMuons" ),
    electronsrc = cms.InputTag( "skimmedPatElectrons" ),
    jetsrc      = cms.InputTag( "skimmedPatJets" ),
    MaxJet      = cms.untracked.int32(7),
    ReqBJet     = cms.untracked.int32(2),
    btagfile    = cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)

tstarMassReco8jet = cms.EDProducer(
    "ChiSqMassReco",
    metsrc      = cms.InputTag( "slimmedMETs" ),
    muonsrc     = cms.InputTag( "skimmedPatMuons" ),
    electronsrc = cms.InputTag( "skimmedPatElectrons" ),
    jetsrc      = cms.InputTag( "skimmedPatJets" ),
    MaxJet      = cms.untracked.int32(8),
    ReqBJet     = cms.untracked.int32(2),
    btagfile    = cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)

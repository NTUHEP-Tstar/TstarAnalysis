import FWCore.ParameterSet.Config as cms

#-------------------------------------------------------------------------------
#   Muon Settings
#-------------------------------------------------------------------------------
selectedMuons = cms.EDFilter(
    "MuonProducer",
    vertexsrc = cms.InputTag("offlineSlimmedPrimaryVertices"),
    muonsrc   = cms.InputTag("slimmedMuons"),
    packedsrc = cms.InputTag("packedPFCandidates"),
)

skimmedPatMuons = cms.EDFilter(
    "PATMuonSelector",
    src = cms.InputTag("selectedMuons"),
    cut = cms.string("")
    )

#-------------------------------------------------------------------------------
#   Electron Settings
#-------------------------------------------------------------------------------
selectedElectrons = cms.EDFilter(
    "ElectronProducer",
    electronsrc = cms.InputTag("slimmedElectrons"),
    packedsrc = cms.InputTag("packedPFCandidates"),
)

skimmedPatElectrons = cms.EDFilter(
    "PATElectronSelector",
    src = cms.InputTag("selectedElectrons"),
    cut = cms.string("")
    )

#-------------------------------------------------------------------------------
#   Jet Settings
#-------------------------------------------------------------------------------
selectedJets =  cms.EDFilter(
    "JetProducer",
    jetsrc      = cms.InputTag("slimmedJets"),
    muonsrc     = cms.InputTag("selectedMuons"),
    electronsrc = cms.InputTag("selectedElectrons"),
)

skimmedPatJets = cms.EDFilter(
    "PATJetSelector",
    src = cms.InputTag("selectedJets"),
    cut = cms.string("")
    )

#-------------------------------------------------------------------------------
#   Weight Counter
#-------------------------------------------------------------------------------
beforeAny = cms.EDProducer(
    "WeightCounter",
    lhesrc = cms.InputTag("externalLHEProducer"),
    label  = cms.string("Before Any Selection"),
)

afterBaseLine = cms.EDProducer(
    "WeightCounter",
    lhesrc = cms.InputTag("externalLHEProducer"),
    label  = cms.string("After all selections"),
)

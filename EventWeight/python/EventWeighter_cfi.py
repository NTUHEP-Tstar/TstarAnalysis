import FWCore.ParameterSet.Config as cms

PileupWeight = cms.EDProducer(
    "PileupWeight",
    pusrc      = cms.InputTag('slimmedAddPileupInfo'),
    pileupfile = cms.FileInPath("TstarAnalysis/EventWeight/data/pileupweights.csv")
)

ElectronWeight = cms.EDProducer(
    "ElectronWeight",
    elecsrc   = cms.InputTag('skimmedPatElectrons'),
    trackfile = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_trackeff.root"),
    cutfile   = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_tightcuteff.root")
)

EventWeight = cms.EDProducer(
    "EventWeight",
    lhesrc   = cms.InputTag('externalLHEProducer'),
    elecwsrc = cms.InputTag( "ElectronWeight", "ElectronWeight" , "TstarMassReco" ),
    puwsrc   = cms.InputTag( 'PileupWeight'  , 'PileupWeight'   , "TstarMassReco" )
)

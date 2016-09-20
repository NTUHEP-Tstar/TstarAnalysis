import FWCore.ParameterSet.Config as cms

PileupWeight = cms.EDProducer(
    "PileupWeight",
    pusrc      = cms.InputTag('slimmedAddPileupInfo'),
    pileupfile = cms.FileInPath("TstarAnalysis/EventWeight/data/pileupweights_69200.csv"),
    pileupfile71260 = cms.FileInPath("TstarAnalysis/EventWeight/data/pileupweights_71260.csv"),
    pileupfile62000 = cms.FileInPath("TstarAnalysis/EventWeight/data/pileupweights_62000.csv")
)

ElectronWeight = cms.EDProducer(
    "ElectronWeight",
    elecsrc   = cms.InputTag('skimmedPatElectrons'),
    gsffile   = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_trackeff.root"),
    cutfile   = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_loosecuteff.root")
)

ElectronMeidumWeight = cms.EDProducer(
    "ElectronWeight",
    elecsrc   = cms.InputTag('skimmedPatElectrons'),
    gsffile   = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_trackeff.root"),
    cutfile   = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_mediumcuteff.root")
)

ElectronTightWeight = cms.EDProducer(
    "ElectronWeight",
    elecsrc   = cms.InputTag('skimmedPatElectrons'),
    gsffile   = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_trackeff.root"),
    cutfile   = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_tightcuteff.root")
)

BtagWeight = cms.EDProducer(
    "BtagWeight",
    jetsrc   = cms.InputTag('skimmedPatJets'),
    btagfile = cms.FileInPath("TstarAnalysis/Common/data/CSVv2_ichep.csv"),
)

TopPtWeight = cms.EDProducer(
    "TopPtWeight",
    gensrc = cms.InputTag( 'prunedGenParticles' ),
    a = cms.double(0.159),
    b = cms.double(-0.00141),
    ptmin = cms.double(0),
    ptmax = cms.double(400)
)

EventWeight = cms.EDProducer(
    "EventWeight",
    lhesrc   = cms.InputTag('externalLHEProducer'),
    elecwsrc = cms.InputTag( "ElectronWeight", "ElectronWeight" ),
    puwsrc   = cms.InputTag( 'PileupWeight'  , 'PileupWeight'   ),
    btagsrc  = cms.InputTag( 'BtagWeight'    , 'BtagWeight' )
)

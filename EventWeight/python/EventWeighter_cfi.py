import FWCore.ParameterSet.Config as cms

#-------------------------------------------------------------------------------
#   Pile Up weights
#-------------------------------------------------------------------------------
PileupWeight = cms.EDProducer(
    "PileupWeight",
    pusrc      = cms.InputTag('slimmedAddPileupInfo'),
    pileupfile = cms.FileInPath("TstarAnalysis/EventWeight/data/pileupweights_69200.csv"),
)

PileupWeightBestFit = cms.EDProducer(
    "PileupWeight",
    pusrc      = cms.InputTag('slimmedAddPileupInfo'),
    pileupfile = cms.FileInPath("TstarAnalysis/EventWeight/data/pileupweights_62000.csv"),
)

PileupWeightXsecup = cms.EDProducer(
    "PileupWeight",
    pusrc      = cms.InputTag('slimmedAddPileupInfo'),
    pileupfile = cms.FileInPath("TstarAnalysis/EventWeight/data/pileupweights_72383.csv"),
)
PileupWeightXsecdown = cms.EDProducer(
    "PileupWeight",
    pusrc      = cms.InputTag('slimmedAddPileupInfo'),
    pileupfile = cms.FileInPath("TstarAnalysis/EventWeight/data/pileupweights_66016.csv"),
)

#-------------------------------------------------------------------------------
#   Electron Weights
#-------------------------------------------------------------------------------
ElectronWeightNoTrigger = cms.EDProducer(
    "ElectronWeight",
    elecsrc = cms.InputTag('skimmedPatElectrons'),
    rhosrc  = cms.InputTag('fixedGridRhoFastjetAll'),
    weightlist = cms.VPSet(
        cms.PSet(
            weightname = cms.string('GsfWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_trackeff.root"),
            fileobj    = cms.string("EGamma_SF2D"),
        ),
        cms.PSet(
            weightname = cms.string('CutWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_tightcuteff.root"),
            fileobj    = cms.string("EGamma_SF2D"),
        ),
    )
)

ElectronWeightAll = cms.EDProducer(
    "ElectronWeight",
    elecsrc = cms.InputTag('skimmedPatElectrons'),
    rhosrc  = cms.InputTag('fixedGridRhoFastjetAll'),
    weightlist = cms.VPSet(
        cms.PSet(
            weightname = cms.string('GsfWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_trackeff.root"),
            fileobj    = cms.string("EGamma_SF2D"),
        ),
        cms.PSet(
            weightname = cms.string('CutWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_triggersafecut_eff.root"),
            fileobj    = cms.string("EGamma_SF2D"),
        ),
        cms.PSet(
            weightname = cms.string('TriggerWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath("TstarAnalysis/EventWeight/data/electron_trigger.root"),
            fileobj    = cms.string("scale_ele32"),
        ),
    )
)

#-------------------------------------------------------------------------------
#   Muon Weights
#-------------------------------------------------------------------------------
MuonWeightNoTrigger = cms.EDProducer(
    "MuonWeight",
    muonsrc = cms.InputTag( 'skimmedPatMuons' ),
    weightlist = cms.VPSet(
        cms.PSet(
            weightname = cms.string('IDWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonScaleFactor_Merge.root' ),
            fileobj    = cms.string('muid'),
        ),
        cms.PSet(
            weightname = cms.string('IsoWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonScaleFactor_Merge.root' ),
            fileobj    = cms.string( 'muiso' ),
        ),

    )
)

MuonWeightAll = cms.EDProducer(
    "MuonWeight",
    muonsrc = cms.InputTag( 'skimmedPatMuons' ),
    weightlist = cms.VPSet(
        cms.PSet(
            weightname = cms.string('IDWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonScaleFactor_Merge.root' ),
            fileobj    = cms.string('muid'),
        ),
        cms.PSet(
            weightname = cms.string('IsoWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonScaleFactor_Merge.root' ),
            fileobj    = cms.string( 'muiso' ),
        ),
        cms.PSet(
            weightname = cms.string('TriggerWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonScaleFactor_Merge.root' ),
            fileobj    = cms.string('mutrig'),
        ),
    )
)


#-------------------------------------------------------------------------------
#   B tag weights
#-------------------------------------------------------------------------------
BtagWeight = cms.EDProducer(
    "BtagWeight",
    checkjet = cms.int32(-1),
    jetsrc   = cms.InputTag('skimmedPatJets'),
    btagfile = cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)

#-------------------------------------------------------------------------------
#   Negative weight treatment
#-------------------------------------------------------------------------------
GenWeight = cms.EDProducer(
    "GenWeight",
    lhesrc = cms.InputTag('externalLHEProducer')
)

#-------------------------------------------------------------------------------
#   Top Pt reweighting
#-------------------------------------------------------------------------------
TopPtWeight = cms.EDProducer(
    "TopPtWeight",
    gensrc = cms.InputTag( 'prunedGenParticles' ),
    a = cms.double(0.0615),
    b = cms.double(-0.0005),
    minpt = cms.double(0),
    maxpt = cms.double(10000)
)

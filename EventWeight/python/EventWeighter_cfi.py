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
ElectronWeight = cms.EDProducer(
    "ElectronWeight",
    elecsrc = cms.InputTag('skimmedPatElectrons'),
    gsffile = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_trackeff.root"),
    gsfhist = cms.string("EGamma_SF2D"),
    cutfile = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_tightcuteff.root"),
    cuthist = cms.string("EGamma_SF2D"),
    trgfile = cms.FileInPath("TstarAnalysis/EventWeight/data/electron_trigger.root"),
    trghist = cms.string("total_clone"),
)

#-------------------------------------------------------------------------------
#   Muon Weights
#-------------------------------------------------------------------------------
MuonWeight = cms.EDProducer(
    "MuonWeight",
    muonsrc = cms.InputTag( 'skimmedPatMuons' ),
    idfile  = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonID_Z_RunBCD_prompt80X_7p65.root' ),
    idhist  = cms.string('MC_NUM_TightIDandIPCut_DEN_genTracks_PAR_pt_spliteta_bin1/pt_abseta_ratio'),
    isofile = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonIso_Z_RunBCD_prompt80X_7p65.root' ),
    isohist = cms.string( 'MC_NUM_TightRelIso_DEN_TightID_PAR_pt_spliteta_bin1/pt_abseta_ratio' ),
    trgfile = cms.FileInPath( 'TstarAnalysis/EventWeight/data/muon_trigger.root' ),
    trghist = cms.string('total_clone'),
)

#-------------------------------------------------------------------------------
#   B tag weights
#-------------------------------------------------------------------------------
BtagWeight = cms.EDProducer(
    "BtagWeight",
    jetsrc   = cms.InputTag('skimmedPatJets'),
    btagfile = cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)

#-------------------------------------------------------------------------------
#   Top Pt reweighting
#-------------------------------------------------------------------------------
TopPtWeight = cms.EDProducer(
    "TopPtWeight",
    gensrc = cms.InputTag( 'prunedGenParticles' ),
    a = cms.double(0.159),
    b = cms.double(-0.00141),
    minpt = cms.double(0),
    maxpt = cms.double(400)
)

TopPtWeightSum = cms.EDProducer(
    "WeightSum",
    weightsrc = cms.InputTag('TopPtWeight', 'TopPtWeight' )
)


#-------------------------------------------------------------------------------
#   Total weight ( excluding TopPt weights )
#-------------------------------------------------------------------------------
EventWeight = cms.EDProducer(
    "EventWeight",
    lhesrc   = cms.InputTag( 'externalLHEProducer' ),
    elecwsrc = cms.InputTag( 'ElectronWeight', 'ElectronWeight' ),
    muonwsrc = cms.InputTag( 'MuonWeight' ,    'MuonWeight' ),
    puwsrc   = cms.InputTag( 'PileupWeight',   'PileupWeight' ),
    btagsrc  = cms.InputTag( 'BtagWeight',     'BtagWeight' ),
)

EventWeightSum = cms.EDProducer(
    "WeightSum",
    weightsrc = cms.InputTag( 'EventWeight', 'EventWeight' )
)

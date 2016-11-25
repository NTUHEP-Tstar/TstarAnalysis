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
        # cms.PSet(
        #     weightname = cms.string('CutWeight'),
        #     objecttype = cms.string('TH2D'),
        #     file       = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_tightcuteff.root"),
        #     fileobj    = cms.string("EGamma_SF2D"),
        # ),
        cms.PSet(
            weightname = cms.string('CutWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath("TstarAnalysis/EventWeight/data/egamma_triggersafecut_eff.root"),
            fileobj    = cms.string("EGamma_SF2D"),
        ),
        cms.PSet(
            weightname = cms.string('TriggerWeight'),
            objecttype = cms.string('TEfficiency'),
            file       = cms.FileInPath("TstarAnalysis/EventWeight/data/electron_trigger.root"),
            fileobj    = cms.string("total_clone"),
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
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonID_Z_RunBCD_prompt80X_7p65.root' ),
            fileobj    = cms.string('MC_NUM_TightIDandIPCut_DEN_genTracks_PAR_pt_spliteta_bin1/pt_abseta_ratio'),
        ),
        cms.PSet(
            weightname = cms.string('IsoWeight'),
            objecttype = cms.string('TH2D'),
            file = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonIso_Z_RunBCD_prompt80X_7p65.root' ),
            fileobj = cms.string( 'MC_NUM_TightRelIso_DEN_TightID_PAR_pt_spliteta_bin1/pt_abseta_ratio' ),
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
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonID_Z_RunBCD_prompt80X_7p65.root' ),
            fileobj    = cms.string('MC_NUM_TightIDandIPCut_DEN_genTracks_PAR_pt_spliteta_bin1/pt_abseta_ratio'),
        ),
        cms.PSet(
            weightname = cms.string('IsoWeight'),
            objecttype = cms.string('TH2D'),
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/MuonIso_Z_RunBCD_prompt80X_7p65.root' ),
            fileobj    = cms.string( 'MC_NUM_TightRelIso_DEN_TightID_PAR_pt_spliteta_bin1/pt_abseta_ratio' ),
        ),
        cms.PSet(
            weightname = cms.string('TriggerWeight'),
            objecttype = cms.string('TEfficiency'),
            file       = cms.FileInPath( 'TstarAnalysis/EventWeight/data/muon_trigger.root' ),
            fileobj    = cms.string('total_era_bcd_clone'),
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
    a = cms.double(0.159),
    b = cms.double(-0.00141),
    minpt = cms.double(0),
    maxpt = cms.double(400)
)

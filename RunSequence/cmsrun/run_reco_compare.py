import glob
import sys

import FWCore.ParameterSet.Config as cms
#-------------------------------------------------------------------------
#   Option parsing
#-------------------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as opts
#-------------------------------------------------------------------------
#   Loading addtional lepton selection
#-------------------------------------------------------------------------
import TstarAnalysis.AdditionalSelector.AddLeptonSelector_cfi as mylepsel
#-------------------------------------------------------------------------
#   Jet selection and options b-tag weighting
#-------------------------------------------------------------------------
import TstarAnalysis.AdditionalSelector.ControlJetSelector_cfi as myjetsel
import TstarAnalysis.EventWeight.EventWeighter_cfi as myweight

options = opts.VarParsing('analysis')

options.register('sample',
                 '',
                 opts.VarParsing.multiplicity.list,
                 opts.VarParsing.varType.string,
                 'EDM Files to process'
                 )

options.register('output',
                 'edm_MassReco.root',
                 opts.VarParsing.multiplicity.singleton,
                 opts.VarParsing.varType.string,
                 'Output EDM filename'
                 )

options.register(
    'Mode',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Mode of operation'
)

options.setDefault('maxEvents', 100)

options.parseArguments()

#-------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------
process = cms.Process("HitFitCompare")
process.load("Configuration.EventContent.EventContent_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents))
process.source = cms.Source("PoolSource",
                            fileNames=cms.untracked.vstring(options.sample)
                            )

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))


if "Control" in options.Mode:
    print "Loading modules for control region"
    process.jetselector = myjetsel.controlregion
elif "Signal" in options.Mode:
    print "Loading modules for signal region"
    process.jetselector = myjetsel.signalregion
    process.BtagWeight = myweight.BtagWeight
    process.BtagWeight.checkjet = process.jetselector.checkorder
else:
    print "ERROR! Input Mode: [", options.Mode, "] not recognized!"
    sys.exit(1)

process.electronsel = mylepsel.ElectronSelector
process.muonsel = mylepsel.MuonSelector
process.electronsel.reqtrigger = cms.VPSet()
process.muonsel.reqtrigger = cms.VPSet()


#-------------------------------------------------------------------------
#   Reloading weight summing and lepton weight factors
#-------------------------------------------------------------------------
process.ElectronWeight = myweight.ElectronWeightAll
process.MuonWeight = myweight.MuonWeightAll

if "Signal" in options.Mode:
    process.EventWeight = cms.EDProducer(
        "WeightProdSum",
        weightlist=cms.VInputTag(
            cms.InputTag("ElectronWeight", "ElectronWeight", "HitFitCompare"),
            cms.InputTag("MuonWeight", "MuonWeight", "HitFitCompare"),
            cms.InputTag("PileupWeight", "PileupWeight"),
            cms.InputTag("GenWeight", "GenWeight"),
            cms.InputTag('BtagWeight', 'BtagWeight')
        )
    )
    process.EventWeightAll = cms.EDProducer(
        "WeightProdSum",
        weightlist=cms.VInputTag(
            cms.InputTag("ElectronWeight", "ElectronWeight", "HitFitCompare"),
            cms.InputTag("MuonWeight", "MuonWeight", "HitFitCompare"),
            cms.InputTag("PileupWeight", "PileupWeight"),
            cms.InputTag("GenWeight", "GenWeight"),
            cms.InputTag('BtagWeight', 'BtagWeight'),
            cms.InputTag("TopPtWeight", "TopPtWeight")
        )
    )

elif "Control" in options.Mode:
    process.EventWeight = cms.EDProducer(
        "WeightProdSum",
        weightlist=cms.VInputTag(
            cms.InputTag("ElectronWeight", "ElectronWeight", "HitFitCompare"),
            cms.InputTag("MuonWeight", "MuonWeight", "HitFitCompare"),
            cms.InputTag("PileupWeight", "PileupWeight"),
            cms.InputTag("GenWeight", "GenWeight"),
        )
    )
    process.EventWeightAll = cms.EDProducer(
        "WeightProdSum",
        weightlist=cms.VInputTag(
            cms.InputTag("ElectronWeight", "ElectronWeight"),
            cms.InputTag("MuonWeight", "MuonWeight"),
            cms.InputTag("PileupWeight", "PileupWeight"),
            cms.InputTag("GenWeight", "GenWeight"),
            cms.InputTag("TopPtWeight", "TopPtWeight")
        )
    )


#-------------------------------------------------------------------------
#   Load Settings
#-------------------------------------------------------------------------
# process.ChiSq6jet0b = cms.EDProducer(
#     "ChiSqMassReco",
#     metsrc      = cms.InputTag( "slimmedMETs" ),
#     muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#     electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#     jetsrc      = cms.InputTag( "skimmedPatJets" ),
#     ReqBJet     = cms.untracked.int32(0),
# )
process.ChiSq6jet1b = cms.EDProducer(
    "ChiSqMassReco",
    metsrc=cms.InputTag("slimmedMETs"),
    muonsrc=cms.InputTag("skimmedPatMuons"),
    electronsrc=cms.InputTag("skimmedPatElectrons"),
    jetsrc=cms.InputTag("skimmedPatJets"),
    MaxJet=cms.untracked.int32(6),
    ReqBJet=cms.untracked.int32(1),
    btagfile=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)
process.ChiSq6jet2b = cms.EDProducer(
    "ChiSqMassReco",
    metsrc=cms.InputTag("slimmedMETs"),
    muonsrc=cms.InputTag("skimmedPatMuons"),
    electronsrc=cms.InputTag("skimmedPatElectrons"),
    jetsrc=cms.InputTag("skimmedPatJets"),
    MaxJet=cms.untracked.int32(6),
    ReqBJet=cms.untracked.int32(2),
    btagfile=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)

# process.ChiSq8jet0b = cms.EDProducer(
#     "ChiSqMassReco",
#     metsrc      = cms.InputTag( "slimmedMETs" ),
#     muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#     electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#     jetsrc      = cms.InputTag( "skimmedPatJets" ),
#     MaxJet      = cms.untracked.int32(8),
#     ReqBJet     = cms.untracked.int32(0)
# )

process.ChiSq8jet1b = cms.EDProducer(
    "ChiSqMassReco",
    metsrc=cms.InputTag("slimmedMETs"),
    muonsrc=cms.InputTag("skimmedPatMuons"),
    electronsrc=cms.InputTag("skimmedPatElectrons"),
    jetsrc=cms.InputTag("skimmedPatJets"),
    MaxJet=cms.untracked.int32(8),
    ReqBJet=cms.untracked.int32(1),
    btagfile=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)
process.ChiSq8jet2b = cms.EDProducer(
    "ChiSqMassReco",
    metsrc=cms.InputTag("slimmedMETs"),
    muonsrc=cms.InputTag("skimmedPatMuons"),
    electronsrc=cms.InputTag("skimmedPatElectrons"),
    jetsrc=cms.InputTag("skimmedPatJets"),
    MaxJet=cms.untracked.int32(8),
    ReqBJet=cms.untracked.int32(2),
    btagfile=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)

# process.NoTopConstrain6j0b = cms.EDProducer(
#     "HitFitMassReco",
#     metsrc      = cms.InputTag( "slimmedMETs" ),
#     muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#     electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#     jetsrc      = cms.InputTag( "skimmedPatJets" ),
#     ReqBJet     = cms.untracked.int32(0),
# )

process.NoTopConstrain6j2b = cms.EDProducer(
    "HitFitMassReco",
    metsrc=cms.InputTag("slimmedMETs"),
    muonsrc=cms.InputTag("skimmedPatMuons"),
    electronsrc=cms.InputTag("skimmedPatElectrons"),
    jetsrc=cms.InputTag("skimmedPatJets"),
    MaxJet=cms.untracked.int32(6),
    ReqBJet=cms.untracked.int32(2),
    btagfile=cms.FileInPath("TstarAnalysis/EventWeight/data/CSVv2_ichep.csv"),
)

# process.NoTopConstrain6j2b = cms.EDProducer(
#     "HitFitMassReco",
#     metsrc      = cms.InputTag( "slimmedMETs" ),
#     muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#     electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#     jetsrc      = cms.InputTag( "skimmedPatJets" ),
#     ReqBJet     = cms.untracked.int32(2),
# )

# process.NoTopConstrain8j0b = cms.EDProducer(
#     "HitFitMassReco",
#     metsrc      = cms.InputTag( "slimmedMETs" ),
#     muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#     electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#     jetsrc      = cms.InputTag( "skimmedPatJets" ),
#     MaxJet      = cms.untracked.int32(8),
#     ReqBJet     = cms.untracked.int32(0),
# )

# process.NoTopConstrain8j1b = cms.EDProducer(
#     "HitFitMassReco",
#     metsrc      = cms.InputTag( "slimmedMETs" ),
#     muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#     electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#     jetsrc      = cms.InputTag( "skimmedPatJets" ),
#     MaxJet      = cms.untracked.int32(8),
#     ReqBJet     = cms.untracked.int32(1),
# )

# process.NoTopConstrain8j2b = cms.EDProducer(
#     "HitFitMassReco",
#     metsrc      = cms.InputTag( "slimmedMETs" ),
#     muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#     electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#     jetsrc      = cms.InputTag( "skimmedPatJets" ),
#     MaxJet      = cms.untracked.int32(8),
#     ReqBJet     = cms.untracked.int32(2),
# )

# process.AddTopConstrain6j0b = cms.EDProducer(
#  "HitFitMassReco",
#  metsrc      = cms.InputTag( "slimmedMETs" ),
#  muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#  electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#  jetsrc      = cms.InputTag( "skimmedPatJets" ),
#  MaxJet      = cms.untracked.int32(6),
#  ReqBJet     = cms.untracked.int32(0),
#  fittingTopMass = cms.untracked.double(173.5)
# )
#
# process.AddTopConstrain6j1b = cms.EDProducer(
#  "HitFitMassReco",
#  metsrc      = cms.InputTag( "slimmedMETs" ),
#  muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#  electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#  jetsrc      = cms.InputTag( "skimmedPatJets" ),
#  MaxJet      = cms.untracked.int32(6),
#  ReqBJet     = cms.untracked.int32(1),
#  fittingTopMass = cms.untracked.double(173.5)
# )
# process.AddTopConstrain6j2b = cms.EDProducer(
#     "HitFitMassReco",
#     metsrc      = cms.InputTag( "slimmedMETs" ),
#     muonsrc     = cms.InputTag( "skimmedPatMuons" ),
#     electronsrc = cms.InputTag( "skimmedPatElectrons" ),
#     jetsrc      = cms.InputTag( "skimmedPatJets" ),
#     MaxJet      = cms.untracked.int32(6),
#     ReqBJet     = cms.untracked.int32(2),
#     fittingTopMass = cms.untracked.double(173.5)
# )
#
#-------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------
process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string(options.output),
    outputCommands=cms.untracked.vstring(
        "keep *",
        "drop *_*ElectronWeight*_*_*",
        "drop *_*MuonWeight*_*_*",
        "drop *_*_WeightProd_*",
        "drop *_*_WeightSum_*",
        "keep *_*ElectronWeight*_*_HitFitCompare",
        "keep *_*MuonWeight*_*_HitFitCompare",
        "keep *_*_WeightProd_HitFitCompare",
        "keep *_*_WeightSum_HitFitCompare",
        "keep *_BeforeAll_*_*"
    ),
    SelectEvents=cms.untracked.PSet(
        SelectEvents=cms.vstring('path')
    )
)

if "Signal" in options.Mode:
    process.path = cms.Path(
        process.jetselector
        * process.BtagWeight
        * process.electronsel
        * process.muonsel
        * process.ElectronWeight
        * process.MuonWeight
        * process.EventWeight
        * process.EventWeightAll
        * (
            process.ChiSq6jet1b
            + process.ChiSq6jet2b
            + process.ChiSq8jet1b
            + process.ChiSq8jet2b
            + process.NoTopConstrain6j2b
        )
    )
elif "Control" in options.Mode:
    process.path = cms.Path(
        process.jetselector
        * process.electronsel
        * process.muonsel
        * process.ElectronWeight
        * process.MuonWeight
        * process.EventWeight
        * process.EventWeightAll
        * (
            process.ChiSq6jet1b
            + process.ChiSq6jet2b
            + process.ChiSq8jet1b
            + process.ChiSq8jet2b
            + process.NoTopConstrain6j2b
        )
    )


# process.myfilterpath = cms.Path(process.tstarMassReco)
process.endPath = cms.EndPath(process.edmOut)

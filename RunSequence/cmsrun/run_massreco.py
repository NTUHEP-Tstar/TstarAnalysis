#*************************************************************************
#
#  Filename    : run_massreco.py
#  Description : Python file for CMSRUN for production mass reconstruction
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#*************************************************************************

import sys

import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as opts
import FWCore.PythonUtilities.LumiList as LumiList
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag

options = opts.VarParsing('analysis')

options.register(
    'sample',
    'file:tstarBaseline.root',
    opts.VarParsing.multiplicity.list,
    opts.VarParsing.varType.string,
    'EDM Files to process'
)

options.register(
    'output',
    'tstarMassReco.root',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Output EDM filename'
)

options.register(
    'Mode',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Mode of for tstar reconstruction (Control and Signal)'
)

options.register(
    'Lumimask',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Lumimask to impose on data'
)

options.setDefault('maxEvents', 1000)

options.parseArguments()

#-------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------
process = cms.Process("TstarMassReco")

process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Services_cff')

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents)
)
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.sample)
)

if options.Lumimask:
    process.source.lumisToProcess = LumiList.LumiList(
        filename=options.Lumimask).getVLuminosityBlockRange()

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

#-------------------------------------------------------------------------------
#   Objects for jet selection and weigting
#-------------------------------------------------------------------------------
import TstarAnalysis.AdditionalSelector.ControlJetSelector_cfi as myjetsel
import TstarAnalysis.EventWeight.EventWeighter_cfi as myweight

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

#-------------------------------------------------------------------------------
#   Lepton selector and weighter
#-------------------------------------------------------------------------------
import TstarAnalysis.AdditionalSelector.AddLeptonSelector_cfi as mylepsel
process.electronsel = mylepsel.ElectronSelector
process.muonsel = mylepsel.MuonSelector

#-------------------------------------------------------------------------
#   Reloading weight summing and lepton weight factors
#-------------------------------------------------------------------------
process.ElectronWeight = myweight.ElectronWeightAll
process.MuonWeight = myweight.MuonWeightAll

if "Signal" in options.Mode:
    process.EventWeight = cms.EDProducer(
        "WeightProdSum",
        weightlist=cms.VInputTag(
            cms.InputTag("ElectronWeight", "ElectronWeight", "TstarMassReco"),
            cms.InputTag("MuonWeight", "MuonWeight", "TstarMassReco"),
            cms.InputTag("PileupWeight", "PileupWeight"),
            cms.InputTag("GenWeight", "GenWeight"),
            cms.InputTag('BtagWeight', 'BtagWeight')
        )
    )
    process.EventWeightAll = cms.EDProducer(
        "WeightProdSum",
        weightlist=cms.VInputTag(
            cms.InputTag("ElectronWeight", "ElectronWeight", "TstarMassReco"),
            cms.InputTag("MuonWeight", "MuonWeight", "TstarMassReco"),
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
            cms.InputTag("ElectronWeight", "ElectronWeight", "TstarMassReco"),
            cms.InputTag("MuonWeight", "MuonWeight", "TstarMassReco"),
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
#   Load mass calculator
#-------------------------------------------------------------------------
process.load("TstarAnalysis.TstarMassReco.Producer_cfi")

#-------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------
process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string(options.output),
    outputCommands=cms.untracked.vstring(
        "keep *",
        "drop *_skimmedPatJets_*_tstarbaseline",
        "drop *_*ElectronWeight*_*_*",
        "drop *_*MuonWeight*_*_*",
        "drop *_*_WeightProd_*",
        "drop *_*_WeightSum_*",
        "keep *_*ElectronWeight*_*_TstarMassReco",
        "keep *_*MuonWeight*_*_TstarMassReco",
        "keep *_*_WeightProd_TstarMassReco",
        "keep *_*_WeightSum_TstarMassReco",
        "keep *_BeforeAll_*_*",
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
        * process.tstarMassReco
    )
elif "Control" in options.Mode:
    process.path = cms.Path(
        process.skimmedPatJets
        * process.jetselector
        * process.electronsel
        * process.muonsel
        * process.ElectronWeight
        * process.MuonWeight
        * process.EventWeight
        * process.EventWeightAll
        * process.tstarMassReco
    )

# process.myfilterpath = cms.Path(process.tstarMassReco)
process.endPath = cms.EndPath(process.edmOut)

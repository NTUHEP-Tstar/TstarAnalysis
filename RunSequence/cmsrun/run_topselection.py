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
import TstarAnalysis.EventWeight.EventWeighter_cfi as myweight
#-------------------------------------------------------------------------
#   Loading addtional selection
#-------------------------------------------------------------------------

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
    'toplike.root',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Output EDM filename'
)

options.register(
    'lumimask',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Lumi Mask to apply'
)

options.register(
    'reportEvery',
    10000,
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.int,
    'How often to print messages'
)

options.setDefault('maxEvents', 1000)

options.parseArguments()

#-------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------
process = cms.Process("TopLike")
process.load("Configuration.EventContent.EventContent_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = options.reportEvery
process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents)
)
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.sample)
)

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

#-------------------------------------------------------------------------------
#   Loading lumi masking for data events
#-------------------------------------------------------------------------------
if options.lumimask:
    import FWCore.PythonUtilities.LumiList as LumiList
    process.source.lumisToProcess = LumiList.LumiList(
        filename=options.lumimask).getVLuminosityBlockRange()
    # since we are using a single object trigger, it should be OK

#-------------------------------------------------------------------------------
#   Setting up jet selection criteria
#-------------------------------------------------------------------------------
import TstarAnalysis.AdditionalSelector.ControlJetSelector_cfi as mysel
process.jetselector = mysel.toplikeregion

#-------------------------------------------------------------------------------
#   Setting adding addtional lepton selection
#-------------------------------------------------------------------------------
import TstarAnalysis.AdditionalSelector.AddLeptonSelector_cfi as mylepsel
process.electronsel = mylepsel.ElectronSelector
process.muonsel     = mylepsel.MuonSelector

if options.lumimask == "" :
    # disable trigger selection for non-data samples
    process.electronsel.reqtrigger = cms.VPSet()
    process.muonsel.reqtrigger = cms.VPSet()


#-------------------------------------------------------------------------------
#   Setting up for B tag weighting and re summing of weights
#-------------------------------------------------------------------------------
import TstarAnalysis.EventWeight.EventWeighter_cfi as myweight

process.ElectronWeight = myweight.ElectronWeightAll
process.MuonWeight     = myweight.MuonWeightAll
process.BtagWeight     = myweight.BtagWeight

process.EventWeight = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("ElectronWeight", "ElectronWeight","TopLike"),
        cms.InputTag("MuonWeight",     "MuonWeight","TopLike"),
        cms.InputTag("PileupWeight",   "PileupWeight"),
        cms.InputTag("SignWeight", "SignWeight"),
        cms.InputTag('BtagWeight','BtagWeight')
    )
)
process.EventWeightAll = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("ElectronWeight", "ElectronWeight","TopLike"),
        cms.InputTag("MuonWeight",     "MuonWeight","TopLike"),
        cms.InputTag("PileupWeight",   "PileupWeight"),
        cms.InputTag("SignWeight",     "SignWeight"),
        cms.InputTag('BtagWeight',     'BtagWeight'),
        cms.InputTag("TopPtWeight",    "TopPtWeight")
    )
)

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
        "keep *_*ElectronWeight*_*_TopLike",
        "keep *_*MuonWeight*_*_TopLike",
        "keep *_*_WeightProd_TopLike",
        "keep *_*_WeightSum_TopLike",
    ),
    SelectEvents=cms.untracked.PSet(
        SelectEvents=cms.vstring('path')
    )
)

process.path = cms.Path(
    process.jetselector
    * process.BtagWeight
    * process.electronsel
    * process.muonsel
    * process.ElectronWeight
    * process.MuonWeight
    * process.EventWeight
    * process.EventWeightAll
)

process.endPath = cms.EndPath(process.edmOut)

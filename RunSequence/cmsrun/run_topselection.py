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


options.setDefault('maxEvents', 1000)

options.parseArguments()

#-------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------
process = cms.Process("TopLike")
process.load("Configuration.EventContent.EventContent_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100
process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents)
)
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.sample)
)

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

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
        cms.InputTag("GenWeight", "GenWeight"),
        cms.InputTag('BtagWeight','BtagWeight')
    )
)
process.EventWeightAll = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("ElectronWeight", "ElectronWeight","TopLike"),
        cms.InputTag("MuonWeight",     "MuonWeight","TopLike"),
        cms.InputTag("PileupWeight",   "PileupWeight"),
        cms.InputTag("GenWeight",     "GenWeight"),
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
        "keep *_BeforeAll_*_*"
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

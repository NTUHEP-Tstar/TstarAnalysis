#*************************************************************************
#
#  Filename    : ConfFile_cfg.py
#  Description : cmsRun configuration file settings for tstar base line selection
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#*************************************************************************
import sys

import FWCore.ParameterSet.Config as cms
#-------------------------------------------------------------------------
#   Setting up options parser
#-------------------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as opts
import TstarAnalysis.EventWeight.EventWeighter_cfi as myweight

options = opts.VarParsing('analysis')

options.register(
    'sample',
    'file://tstarMassReco.root',
    opts.VarParsing.multiplicity.list,
    opts.VarParsing.varType.string,
    'EDM File to process'
)

options.register(
    'output',
    'tstarSupSelec.root',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Output EDM filename'
)

options.setDefault('maxEvents', 1000)

options.parseArguments()

#-------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------
process = cms.Process("TstarSupSelec")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.MessageLogger.cerr.FwkReport.reportEvery = 10000

process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents))

process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.sample)
)

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

#-------------------------------------------------------------------------------
#   Loading my filters
#-------------------------------------------------------------------------------
print "Loading main filter\n\n"
process.load("TstarAnalysis.SupSelector.SupSelector_cfi")

print "Reloading event weighter to recount events"
process.EventWeight = myweight.EventWeight
process.load("TstarAnalysis.BaseLineSelector.Counter_cfi")

#-------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------
process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string(options.output),
    outputCommands=cms.untracked.vstring(
        "keep *",
        "drop *_EventWeight_*_TstarMassReco"  # Dropping previous weighting results
    ),
    SelectEvents=cms.untracked.PSet(SelectEvents=cms.vstring('myfilterpath'))
)

process.myfilterpath = cms.Path(
    process.SupSelector
    * process.EventWeight
)


process.endPath = cms.EndPath(process.edmOut)

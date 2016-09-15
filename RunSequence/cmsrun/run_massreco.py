import FWCore.ParameterSet.Config as cms

#-------------------------------------------------------------------------
#   Option parsing
#-------------------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as opts
import glob
import sys

options = opts.VarParsing('analysis')

options.register(
    'sample',
    'file://tstarBaseline.root',
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

options.setDefault('maxEvents', 1000)

options.parseArguments()

#-------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------
process = cms.Process("TstarMassReco")
process.load("Configuration.EventContent.EventContent_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents)
)
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.sample)
)

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

#-------------------------------------------------------------------------
#   Load default Settings
#-------------------------------------------------------------------------
process.load("TstarAnalysis.TstarMassReco.Producer_cfi")
process.load("TstarAnalysis.EventWeight.EventWeighter_cfi")

#-------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------
process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string(options.output),
    outputCommands=cms.untracked.vstring(
        "keep *",
    ),
    SelectEvents=cms.untracked.PSet(
        SelectEvents=cms.vstring('path')
    )
)

process.path = cms.Path(
    process.tstarMassReco
    * (process.ElectronWeight
        + process.PileupWeight
        + process.BtagWeight)
    * process.EventWeight
)

# process.myfilterpath = cms.Path(process.tstarMassReco)
process.endPath = cms.EndPath(process.edmOut)

#*******************************************************************************
 #
 #  Filename    : run_massreco.py
 #  Description : Python file for CMSRUN for production mass reconstruction
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as opts
import sys

options = opts.VarParsing('analysis')

options.register(
    'sample',
    '/store/user/yichen/tstar_store/tstarbaseline/Muon/TstarTstarToTgluonTgluon_M-800_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root',
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
    'lumimask',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Lumi Mask to apply'
)

options.register(
    'Mode',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Mode of for tstar reconstruction (Control and Signal)'
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

if options.lumimask :
    import FWCore.PythonUtilities.LumiList as LumiList
    process.source.lumisToProcess = LumiList.LumiList( filename=options.lumimask ).getVLuminosityBlockRange()

#-------------------------------------------------------------------------------
#   Loading addtional selection
#-------------------------------------------------------------------------------
import TstarAnalysis.TstarMassReco.ControlJetSelector_cfi as mysel

if "Control" in options.Mode :
    process.jetselector = mysel.controlregion
elif "Signal" in options.Mode :
    process.jetselector = mysel.signalregion
else:
    print "ERROR! Input Mode: [", options.Mode, "] not recognized!"
    sys.exit(1)

#-------------------------------------------------------------------------------
#   Reloading the event weight summation
#-------------------------------------------------------------------------------
import TstarAnalysis.EventWeight.EventWeighter_cfi as myweights
process.TopPtWeightSum = myweights.TopPtWeightSum
process.EventWeightSum = myweights.EventWeightSum

#-------------------------------------------------------------------------
#   Load default Settings
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
        "drop *_*WeightSum*_*_*",
        "keep *_*WeightSum*_*_TstarMassReco"
    ),
    SelectEvents=cms.untracked.PSet(
        SelectEvents=cms.vstring('path')
    )
)

process.path = cms.Path(
    process.jetselector
    * process.TopPtWeightSum
    * process.EventWeightSum
    * process.tstarMassReco
)

# process.myfilterpath = cms.Path(process.tstarMassReco)
process.endPath = cms.EndPath(process.edmOut)

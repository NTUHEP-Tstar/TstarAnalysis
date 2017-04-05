#*******************************************************************************
 #
 #  Filename    : MCResolution.py
 #  Description : Config file for generating tree for resolution calculation
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import sys

import FWCore.ParameterSet.Config as cms
#-------------------------------------------------------------------------
#   Setting up options parser
#-------------------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as opts

options = opts.VarParsing('analysis')

options.register(
    'sample',
    'file:/wk_cms2/yichen/MiniAOD/TstarM700.root',
    opts.VarParsing.multiplicity.list,
    opts.VarParsing.varType.string,
    'EDM Filter to process'
)

options.register(
    'output',
    'mcres_700.root',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Output EDM filename'
)

options.setDefault('maxEvents', 1000)

options.parseArguments()

#-------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------
print "Defining new process, and loading common settings"
process = cms.Process("tstarbaseline")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.EventContent.EventContent_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents)
    )
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.sample)
)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True) ,
    allowUnscheduled = cms.untracked.bool(True),
)

#-------------------------------------------------------------------------------
#   Loading service and plugin
#-------------------------------------------------------------------------------
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string( options.output )
    )

process.mcres = cms.EDAnalyzer(
    "MCResolution",
    jetsrc = cms.InputTag("slimmedJets"),
    muonsrc = cms.InputTag("slimmedMuons"),
    elecsrc = cms.InputTag("slimmedElectrons"),
    gensrc = cms.InputTag("prunedGenParticles")
)

process.p = cms.Path( process.mcres )

print "Finish reading cfg file... Loading main framework"

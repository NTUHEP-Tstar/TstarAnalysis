#*************************************************************************
#
#  Filename    : JetLepTest.py
#  Description : Simple Config for running small file
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#*************************************************************************
import sys

import FWCore.ParameterSet.Config as cms
#-------------------------------------------------------------------------
#   Setting up options parser
#-------------------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as opts

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *

options = opts.VarParsing('analysis')

options.register(
    'sample',
    'file:TT_Moriond17.root',
    opts.VarParsing.multiplicity.list,
    opts.VarParsing.varType.string,
    'EDM Filter to process'
)

options.register(
    'output',
    'jetlepsep.root',
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
process.load("RecoEgamma.ElectronIdentification.ElectronIDValueMapProducer_cfi")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v6'

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
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
#-------------------------------------------------------------------------
#   Importing Electron VID maps
#-------------------------------------------------------------------------
print "Loading electron ID processes..."
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
elecIDModules = [
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Summer16_80X_V1_cff',
    'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV60_cff',
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff'
]

for mod in elecIDModules:
    setupAllVIDIdsInModule(process, mod, setupVIDElectronSelection)

#-------------------------------------------------------------------------
#   Load self written filters
#-------------------------------------------------------------------------
print "Loading main filter...."
import TstarAnalysis.BaseLineSelector.Producer_cfi as myfilter

process.selectedMuons = myfilter.selectedMuons
process.selectedElectrons = myfilter.selectedElectrons

process.jetlepsep = cms.EDAnalyzer(
    "JetLepTest",
    jetsrc = cms.InputTag("slimmedJets"),
    muonsrc = cms.InputTag("selectedMuons"),
    elecsrc = cms.InputTag("selectedElectrons")
)

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string( options.output )
    )


process.p = cms.Path(
    process.selectedMuons
    * process.egmGsfElectronIDSequence
    * process.selectedElectrons
    * process.jetlepsep
    )

print "Finish reading cfg file... Loading main framework"

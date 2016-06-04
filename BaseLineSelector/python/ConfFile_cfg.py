#*******************************************************************************
 #
 #  Filename    : ConfFile_cfg.py
 #  Description : cmsRun configuration file settings for tstar base line selection
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import FWCore.ParameterSet.Config as cms

#-------------------------------------------------------------------------------
#   Setting up options parser
#-------------------------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as opts
options = opts.VarParsing('analysis')

options.register( 'sample',
    'file:////wk_cms/yichen/miniAODs/MC_MiniAOD_76X/TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8.root',
    opts.VarParsing.multiplicity.list,
    opts.VarParsing.varType.string,
    'EDM Filter to process'
    )

options.register( 'output',
    'tstarBaseline.root',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Output EDM filename'
    )

options.register( 'Mode',
    0,
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.int,
    'Filter operation'
    )

options.register( 'LumiMask',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Add lumi mask to process'
    )

options.setDefault('maxEvents',1000)

options.parseArguments()

#-------------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------------
process = cms.Process("TstarBaseLine")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load("RecoEgamma.ElectronIdentification.ElectronIDValueMapProducer_cfi")
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag.globaltag = '76X_mcRun2_asymptotic_v12'

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.sample))

import FWCore.PythonUtilities.LumiList as LumiList
if options.LumiMask:
    process.source.lumisToProcess = LumiList.LumiList(filename = options.LumiMask ).getVLuminosityBlockRange()


process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
# process.options.allowUnscheduled = cms.untracked.bool(True)

#-------------------------------------------------------------------------------
#   Importing HLT Filter
#-------------------------------------------------------------------------------
requiredHLTs = []

if options.Mode == 0 : # Muon Channel
    requiredHLTs = ['HLT_IsoMu27*']
elif options.Mode == 1: # Electron Channel
    requiredHLTs = ['HLT_Ele27_eta2p1_WPLoose_Gsf_*']

from HLTrigger.HLTfilters.hltHighLevel_cfi import *
process.hltfilter = hltHighLevel.clone(
    TriggerResultsTag = "TriggerResults::HLT",
    HLTPaths = requiredHLTs
)

#-------------------------------------------------------------------------------
#   Electron ID Processes
#-------------------------------------------------------------------------------
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *

switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD )
elecIDModules  = [
    "RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff"
    ]

for mod in elecIDModules:
   setupAllVIDIdsInModule( process, mod, setupVIDElectronSelection )

#-------------------------------------------------------------------------------
#   Load Self written producers
#-------------------------------------------------------------------------------
process.load("TstarAnalysis.BaseLineSelector.Producer_cfi")
process.selectedElectrons.vetoMap   = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-veto"  );
process.selectedElectrons.looseMap  = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-loose" );
process.selectedElectrons.mediumMap = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-medium");
process.selectedElectrons.tightMap  = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-tight" );

#-------------------------------------------------------------------------------
#   Load Self Written Filters
#-------------------------------------------------------------------------------
process.load("TstarAnalysis.BaseLineSelector.Filter_cfi")
process.tstarFilter.runMode = cms.int32( options.Mode )

#-------------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------------
process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string( options.output ),
    outputCommands = cms.untracked.vstring(
        "keep *",
        "keep *_externalLHEProducer_*_*",
        "keep *_generator_*_*",
        "keep *_prunedGenParticles_*_*",
        "keep *_TriggerResults_*_HLT",
        "keep *_fixedGridRhoFastjetAll_*_*",
        "keep *_offlineSlimmedPrimaryVertices_*_*",
        "keep *_slimmedMETs_*_*",
        "keep *_skimmedPatMuons_*_*",
        "keep *_skimmedPatElectrons_*_*",
        "keep *_skimmedPatJets_*_*",
        "keep edmMergeableCounter_*_*_*",
        ),
    SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('myfilterpath') )
    )

process.myfilterpath = cms.Path(
    process.beforeAny
    * process.hltfilter
    * process.selectedMuons
    * process.skimmedPatMuons
    * process.egmGsfElectronIDSequence
    * process.selectedElectrons
    * process.skimmedPatElectrons
    * process.selectedJets
    * process.skimmedPatJets
    * process.tstarFilter
    * process.afterBaseLine
)


process.endPath = cms.EndPath( process.edmOut )

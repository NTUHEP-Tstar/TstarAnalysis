#*******************************************************************************
 #
 #  Filename    : ConfFile_cfg.py
 #  Description : cmsRun configuration file settings for tstar base line selection
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import FWCore.ParameterSet.Config as cms
import sys

#-------------------------------------------------------------------------------
#   Setting up options parser
#-------------------------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as opts
options = opts.VarParsing('analysis')

options.register( 'sample',
    'file:////afs/cern.ch/work/y/yichen/MiniAOD/MC_80X_reHLT/TTJets.root',
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
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Filter operation'
    )

options.register('GlobalTag',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Global Tag to use'
)

options.setDefault('maxEvents',1000)

options.parseArguments()

#-------------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------------
process = cms.Process("tstarbaseline")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load("RecoEgamma.ElectronIdentification.ElectronIDValueMapProducer_cfi")
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag.globaltag = options.GlobalTag

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.sample))

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
# process.options.allowUnscheduled = cms.untracked.bool(True)

#-------------------------------------------------------------------------------
#   Importing HLT Filter
#-------------------------------------------------------------------------------
requiredHLTs = []

if options.Mode == 'MuonSignal' : # Muon Channel
    requiredHLTs = ['HLT_IsoMu27*']
elif options.Mode == 'ElectronSignal': # Electron Channel
    requiredHLTs = ['HLT_Ele27_eta2p1_WPLoose_Gsf_*']
else:
    print "Mode not recognized!"
    sys.exit(1)


from HLTrigger.HLTfilters.hltHighLevel_cfi import *
process.hltfilter = hltHighLevel.clone(
    TriggerResultsTag = "TriggerResults::HLT",
    HLTPaths = requiredHLTs
)
if '80X_mcRun2_asymptotic_2016_miniAODv2_v1' == options.GlobalTag:
    process.hltfilter.TriggerResultsTag = "TriggerResults::HLT2"

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
#   Load Self Written Filters
#-------------------------------------------------------------------------------
print "Loading main filter\n\n"
import TstarAnalysis.BaseLineSelector.Filter_cfi as myfilters

process.tstarFilter = myfilters.tstarFilter
process.tstarFilter.runMode = cms.string( options.Mode )

#-------------------------------------------------------------------------------
#   Load Self written producers
#-------------------------------------------------------------------------------
print "Loading producers\n\n"
process.load("TstarAnalysis.BaseLineSelector.Producer_cfi")
process.selectedElectrons.vetoMap   = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-veto"  );
process.selectedElectrons.looseMap  = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-loose" );
process.selectedElectrons.mediumMap = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-medium");
process.selectedElectrons.tightMap  = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-tight" );


#-------------------------------------------------------------------------------
#   Load counters
#-------------------------------------------------------------------------------
print "Loading counters\n\n"
process.load("TstarAnalysis.BaseLineSelector.Counter_cfi")

#-------------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------------
process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string( options.output ),
    outputCommands = cms.untracked.vstring(
        "drop *",
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
    * process.afterHLT
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

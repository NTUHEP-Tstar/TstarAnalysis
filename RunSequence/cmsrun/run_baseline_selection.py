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
import TstarAnalysis.EventWeight.EventWeighter_cfi as weight
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
from PhysicsTools.PatAlgos.tools.coreTools import *
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *

options = opts.VarParsing('analysis')

options.register(
    'sample',
    'file:/afs/cern.ch/work/y/yichen/MiniAOD/MC_80X_reHLT/TT_powheg_2.root',
    opts.VarParsing.multiplicity.list,
    opts.VarParsing.varType.string,
    'EDM Filter to process'
)

options.register(
    'output',
    'tstarBaseline.root',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Output EDM filename'
)

options.register(
    'GlobalTag',
    '80X_mcRun2_asymptotic_2016_miniAODv2_v1',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Global Tag to use'
)

options.register(
    'Mode',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Lepton mode to use'
)

options.setDefault('maxEvents', 1000)

options.parseArguments()

#-------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------
process = cms.Process("tstarbaseline")
process.load("Configuration.EventContent.EventContent_cff")
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load("JetMETCorrections.Modules.JetResolutionESProducer_cfi")
process.load(
    'Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
process.load("RecoEgamma.ElectronIdentification.ElectronIDValueMapProducer_cfi")
process.GlobalTag.globaltag = options.GlobalTag

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10000
process.maxEvents = cms.untracked.PSet(
    input=cms.untracked.int32(options.maxEvents))
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(options.sample)
)

process.options = cms.untracked.PSet(wantSummary=cms.untracked.bool(True))

#-------------------------------------------------------------------------
#   Importing Electron VID maps
#-------------------------------------------------------------------------
print "Loading electron ID processes..."
switchOnVIDElectronIdProducer(process, DataFormat.MiniAOD)
elecIDModules = [
    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff'
]

for mod in elecIDModules:
    setupAllVIDIdsInModule(process, mod, setupVIDElectronSelection)

#-------------------------------------------------------------------------
#   Load self written filters
#-------------------------------------------------------------------------
print "Loading main filter...."
process.load("TstarAnalysis.BaseLineSelector.Filter_cfi")

if options.Mode == "Muon":
    process.leptonSeparator.reqmuon = cms.int32(1)
elif options.Mode == 'Electron':
    process.leptonSeparator.reqelec = cms.int32(1)
else:
    print "[ERROR!!] Unrecognized option [", options.Mode, "]!"
    sys.exit(1)

#-------------------------------------------------------------------------
#   Loading met filtering processes
#-------------------------------------------------------------------------
print "Loading met filtering processes....."
process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")

process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")

process.seqMET = cms.Sequence(
    process.BadPFMuonFilter
    * process.BadChargedCandidateFilter
    * process.METFilter
)

#-------------------------------------------------------------------------
#   Load selected object producers
#-------------------------------------------------------------------------
print "Loading producers...."
process.load("TstarAnalysis.BaseLineSelector.Producer_cfi")

process.seqObjProduce = cms.Sequence(
    process.selectedMuons
    * process.skimmedPatMuons
    * process.egmGsfElectronIDSequence
    * process.selectedElectrons
    * process.skimmedPatElectrons
    * process.selectedJets
    * process.skimmedPatJets
)

#-------------------------------------------------------------------------
#   Load counters
#-------------------------------------------------------------------------
print "Loading counters...."
process.load("TstarAnalysis.BaseLineSelector.Counter_cfi")

#-------------------------------------------------------------------------
#   Load event weighting
#   explicitly removing
#-------------------------------------------------------------------------
print "Loading event weighers...."

# Defining weight calculators
process.ElectronWeight = weight.ElectronWeightNoTrigger
process.MuonWeight = weight.MuonWeightNoTrigger
process.PileupWeight = weight.PileupWeight
process.PileupWeightBestFit = weight.PileupWeightBestFit
process.PileupWeightXsecup = weight.PileupWeightXsecup
process.PileupWeightXsecdown = weight.PileupWeightXsecdown
process.SignWeight = weight.SignWeight
process.TopPtWeight = weight.TopPtWeight

# Defining weight summing calculators
process.EventWeight = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("ElectronWeight", "ElectronWeight"),
        cms.InputTag("MuonWeight",     "MuonWeight"),
        cms.InputTag("PileupWeight",   "PileupWeight"),
        cms.InputTag("SignWeight",     "SignWeight")
    )
)

process.EventWeightAll = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("ElectronWeight", "ElectronWeight"),
        cms.InputTag("MuonWeight", "MuonWeight"),
        cms.InputTag("PileupWeight", "PileupWeight"),
        cms.InputTag("TopPtWeight", "TopPtWeight"),
        cms.InputTag("SignWeight", "SignWeight")
    )
)

# Defninig sequence
process.seqWeight = cms.Sequence(
    (
        process.ElectronWeight
        + process.MuonWeight
        + process.PileupWeight
        + process.PileupWeightBestFit
        + process.PileupWeightXsecup
        + process.PileupWeightXsecdown
        # Removing Btag since it is not used in selection
        + process.SignWeight
        + process.TopPtWeight
    )
    * process.EventWeight
    * process.EventWeightAll
)

#-------------------------------------------------------------------------
#   Defining process paths
#-------------------------------------------------------------------------
print "Defining process path...."
process.myfilterpath = cms.Path(
    process.beforeAny
    * process.seqMET
    * process.seqObjProduce
    * process.leptonSeparator
    * process.seqWeight
)

#-------------------------------------------------------------------------
#   Defining objects to keep
#-------------------------------------------------------------------------
keepobjlist = [
    "keep *_externalLHEProducer_*_*",
    "keep *_generator_*_*",
    "keep *_prunedGenParticles_*_*",
    "keep *_TriggerResults_*_HLT*",
    "keep *_fixedGridRhoFastjetAll_*_*",
    "keep *_offlineSlimmedPrimaryVertices_*_*",
    "keep *_slimmedMETs_*_*",
    "keep *_skimmedPatMuons_*_*",
    "keep *_skimmedPatElectrons_*_*",
    "keep *_skimmedPatJets_*_*",
    "keep *_slimmedAddPileupInfo_*_*",
    "keep *_selectedPatTrigger_*_*",
    "keep edmMergeableCounter_*_*_*",
    "keep *_*Weight*_*_*",
]
if options.Mode == "Electron":
    keepobjlist.append(
        "keep *_reducedEgamma_*_*"  # Required for addtional electron selection
    )

#-------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------

process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string(options.output),
    outputCommands=cms.untracked.vstring(
        "drop *",
        *keepobjlist
    ),
    SelectEvents=cms.untracked.PSet(SelectEvents=cms.vstring('myfilterpath'))
)

process.endPath = cms.EndPath(process.edmOut)

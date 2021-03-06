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
import TstarAnalysis.RunSequence.Naming            as myname
import TstarAnalysis.RunSequence.Settings          as myset

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
    'tstarBaseline.root',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Output EDM filename'
)

options.register(
    'GlobalTag',
    '80X_mcRun2_asymptotic_2016_TrancheIV_v6',
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
print "Defining new process, and loading common settings"
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

process.seqMET = cms.Sequence(
    process.METFilter
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
#   Load event weighting
#   explicitly removing
#-------------------------------------------------------------------------
print "Loading event weighers...."

# Defining weight calculators
process.ElectronWeight       = weight.ElectronWeightNoTrigger
process.MuonWeight           = weight.MuonWeightNoTrigger
process.PileupWeight         = weight.PileupWeight
process.PileupWeightXsecup   = weight.PileupWeightXsecup
process.PileupWeightXsecdown = weight.PileupWeightXsecdown
process.GenWeight            = weight.GenWeight
process.TopPtWeight          = weight.TopPtWeight

# Defining weight summing calculators
process.EventWeight = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("ElectronWeight", "ElectronWeight"),
        cms.InputTag("MuonWeight",     "MuonWeight"),
        cms.InputTag("PileupWeight",   "PileupWeight"),
        cms.InputTag("GenWeight",     "GenWeight")
    )
)

process.EventWeightAll = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("ElectronWeight", "ElectronWeight"),
        cms.InputTag("MuonWeight", "MuonWeight"),
        cms.InputTag("PileupWeight", "PileupWeight"),
        cms.InputTag("TopPtWeight", "TopPtWeight"),
        cms.InputTag("GenWeight", "GenWeight")
    )
)

# Counting effective number of events before any selection is done
process.BeforeAll = cms.EDProducer(
    "WeightProdSum",
    weightlist = cms.VInputTag(
        cms.InputTag("GenWeight","GenWeight")
    )
)

## Weights that should be calculated in total weight
process.noSelWeights = cms.Sequence(
    (
        process.PileupWeight
        + process.PileupWeightXsecup
        + process.PileupWeightXsecdown
        + process.GenWeight
        + process.TopPtWeight
    )
    * process.BeforeAll
)

# Defninig sequence
process.selWeights = cms.Sequence(
    (
        process.ElectronWeight
        + process.MuonWeight
    )
    * process.EventWeight
    * process.EventWeightAll
)

#-------------------------------------------------------------------------
#   Defining process paths
#-------------------------------------------------------------------------
print "Defining process path...."
process.myfilterpath = cms.Path(
    process.noSelWeights
    * process.seqObjProduce
    * process.leptonSeparator
    * process.seqMET
    * process.selWeights
)


#-------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------
print "Defining final output module"
process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName=cms.untracked.string(options.output),
    outputCommands=cms.untracked.vstring(
        "keep *",
        "drop *_slimmedElectrons_*_*",
        "drop *_slimmedJets*_*_*",
        "drop *_slimmedMETsNoHF_*_*",
        "drop *_slimmedJETsPuppi_*_*",
        "drop *_slimmedPhotons_*_*",
        "drop *_slimmedTaus*_*_*",
        "drop *_selectedMuons_*_*",
        "drop *_selectedElectrons_*_*",
        "drop *_selectedJets_*_*",
        "drop *_egmGsfElectronIDs_*_*",
        "drop *_electronMVAValueMapProducer_*_*",
        "drop *_TriggerResults_*_tstarbaseline",
        "drop *_BeforeAll_WeightProd_*",
        "keep mgrCounter_*_*_*"
    ),
    SelectEvents=cms.untracked.PSet(SelectEvents=cms.vstring('myfilterpath') )
)

process.endPath = cms.EndPath(process.edmOut)


print "Finish reading cfg file... Loading main framework"

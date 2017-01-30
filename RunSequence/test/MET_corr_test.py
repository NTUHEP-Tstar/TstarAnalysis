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
    'METTest.root',
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
    'JECVersion',
    '',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'JEC version to use, auto deducing from global tag is not specified'
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

options.JECVersion = myname.GetJECFromGT( options.GlobalTag )

#-------------------------------------------------------------------------------
#   Reloading JEC/JER values from sqlite files
#   https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JecSqliteFile
#-------------------------------------------------------------------------------
print "Loading new JEC files"
from CondCore.DBCommon.CondDBSetup_cfi import CondDBSetup
process.jec = cms.ESSource('PoolDBESSource',
    CondDBSetup,
    connect = cms.string( myname.JECDBName(options.JECVersion) ),
    toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string(myname.JECTagName(options.JECVersion,'AK4PFchs')),
            label  = cms.untracked.string('AK4PFchs')
        ),
    )
)

# Add an ESPrefer to override JEC that might be available from the global tag
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource', 'jec')

#-------------------------------------------------------------------------------
#   MET re-correction - MiniAOD objects required
#   https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETUncertaintyPrescription#Instructions_for_8_0_X_X_20_for
#-------------------------------------------------------------------------------
print "Adding new MET recorrections"
from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

runMetCorAndUncFromMiniAOD(
    process,
    isData=(options.GlobalTag == myset.data_global_tag),
    pfCandColl=cms.InputTag("packedPFCandidates"),
    recoMetFromPFCs=True,
)

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
    process.fullPatMetSequence
    * process.BadPFMuonFilter
    * process.BadChargedCandidateFilter
)

#-------------------------------------------------------------------------
#   Defining process paths
#-------------------------------------------------------------------------
print "Defining process path...."
process.myfilterpath = cms.Path(
    process.seqMET
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
        "keep mgrCounter_*_*_*"
    ),
    SelectEvents=cms.untracked.PSet(SelectEvents=cms.vstring('myfilterpath'))
)

process.endPath = cms.EndPath(process.edmOut)


print "Finish reading cfg file... Loading main framework"

import FWCore.ParameterSet.Config as cms

#-------------------------------------------------------------------------------
#   Option parsing
#-------------------------------------------------------------------------------
import FWCore.ParameterSet.VarParsing as opts
import glob
import sys

options = opts.VarParsing('analysis')

options.register( 'sample',
    '',
    opts.VarParsing.multiplicity.list,
    opts.VarParsing.varType.string,
    'EDM Files to process'
    )

options.register( 'output',
    'edm_MassReco.root',
    opts.VarParsing.multiplicity.singleton,
    opts.VarParsing.varType.string,
    'Output EDM filename'
    )

options.register( 'sampledir',
    '',
    opts.VarParsing.multiplicity.list,
    opts.VarParsing.varType.string,
    'Directory(s) to edm files'
)

options.setDefault('maxEvents',100)

options.parseArguments()

if not options.sampledir and not options.sample :
    print "Error! No sample specified!"
    sys.exit(1);
elif options.sampledir and options.sample:
    print "Warning! Input specified, ignoring sampledir"
elif options.sampledir and not options.sample:
    print "Processing all files in (", options.sampledir,")..."
    for directory in options.sampledir:
        directory.strip();
        for edm_file in glob.glob( directory + "/*.root" ):
            options.sample.append( 'file:'+edm_file );

#-------------------------------------------------------------------------------
#   Defining process and cmsRun outputs
#-------------------------------------------------------------------------------
process = cms.Process("HitFitCompare")
process.load("Configuration.EventContent.EventContent_cff")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(options.sample)
    )

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

process.ChiSq6jet2b = cms.EDProducer(
    "ChiSqMassReco",
    metsrc      = cms.InputTag( "slimmedMETs" ),
    muonsrc     = cms.InputTag( "skimmedPatMuons" ),
    electronsrc = cms.InputTag( "skimmedPatElectrons" ),
    jetsrc      = cms.InputTag( "skimmedPatJets" ),
    ReqBJet     = cms.untracked.int32(2),
)

process.ConstrainChiSq = cms.EDProducer(
    "ConstrainChiSq",
    chisqsrc = cms.InputTag("ChiSq6jet2b","ChiSquareResult"),
)

#-------------------------------------------------------------------------------
#   Defining output Module
#-------------------------------------------------------------------------------
process.edmOut = cms.OutputModule(
    "PoolOutputModule",
    fileName = cms.untracked.string( options.output ),
    outputCommands = cms.untracked.vstring(
        "drop *",
        "keep RecoResult_*_*_*",
        ),
    )

# process.myfilterpath = cms.Path(process.tstarMassReco)
process.endPath = cms.EndPath( process.edmOut )

import FWCore.ParameterSet.Config as cms

process = cms.Process("MCPileUpMaker")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        'file:///afs/cern.ch/work/y/yichen/MiniAOD/MC_80X_reHLT/TTJets.root'
    )
)

process.TFileService = cms.Service("TFileService",
    fileName= cms.string("MCPileUp.root")
)

process.MCPileUpHist = cms.EDAnalyzer(
    "MCPileUpHistMaker",
    lhesrc = cms.InputTag( "externalLHEProducer" ),
    pusrc  = cms.InputTag( "slimmedAddPileupInfo" )
)

process.p = cms.Path(process.MCPileUpHist)

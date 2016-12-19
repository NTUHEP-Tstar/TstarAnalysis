import FWCore.ParameterSet.Config as cms

process = cms.Process("MCPileUpMaker")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
    # replace 'myfile.root' with the source file you want to use
    fileNames = cms.untracked.vstring(
        '/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/50000/0693E0E7-97BE-E611-B32F-0CC47A78A3D8.root'
    )
)

process.MessageLogger.cerr.FwkReport.reportEvery = 10000

process.TFileService = cms.Service("TFileService",
    fileName= cms.string("MCPileUp.root")
)

process.MCPileUpHist = cms.EDAnalyzer(
    "MCPileUpHistMaker",
    lhesrc = cms.InputTag( "externalLHEProducer" ),
    pusrc  = cms.InputTag( "slimmedAddPileupInfo" )
)

process.p = cms.Path(process.MCPileUpHist)

from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'MCPileUpMaker'
config.General.workArea = 'crabworkspace/'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'mcpileup_cfg.py'

## Input parameters
config.JobType.pyCfgParams = [
]

config.Data.inputDataset = '/TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring16MiniAODv2-PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_miniAODv2_v0-v1/MINIAODSIM'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 64
config.Data.outLFNDirBase = '/store/user/yichen/MCpileup_80X/'
config.Data.publication = False

config.Site.storageSite = 'T2_CH_CERN'

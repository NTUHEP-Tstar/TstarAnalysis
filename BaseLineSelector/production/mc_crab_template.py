from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'TSTARSELECT_NAME'
config.General.workArea = 'work_area'
config.General.transferOutputs = True
config.General.transferLogs = True

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = './run_production.py'

## Input parameters
config.JobType.pyCfgParams = [
   'Mode=RUNMODE',
   ]

config.Data.inputDataset = 'DATASET'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 16
config.Data.outLFNDirBase = '/store/user/yichen/tstar_76X/'
config.Data.publication = False


config.Site.storageSite = 'T3_TW_NTU_HEP'

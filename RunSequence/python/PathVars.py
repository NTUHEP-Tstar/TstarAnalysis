#*******************************************************************************
 #
 #  Filename    : PathVars.py
 #  Description : Global variables for storing path information
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import os

tstar_dir   =  os.environ['CMSSW_BASE'] + "/src/TstarAnalysis/"
setting_dir =  tstar_dir + 'RunSequence/settings/'
cmsrun_dir  =  tstar_dir + 'RunSequence/cmsrun/'
storage_dir =  tstar_dir + 'RunSequence/store/'

setting_file_list = [
    setting_dir + 'mc_datasets.json',
    setting_dir + 'data_datasets.json',
]

filter_modes = [
    "MuonSignal",
    "ElectronSignal"
]

lumi_file      = setting_dir + 'lumimask_latest.json'
run_cfg_file   = cmsrun_dir  + 'run_baseline_selection.py'
crab_cfg_dir   = storage_dir + 'CrabConfig/'
crab_work_dir  = storage_dir + 'CrabWorkSpace/'

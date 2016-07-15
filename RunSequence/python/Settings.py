#*******************************************************************************
 #
 #  Filename    : PathVars.py
 #  Description : Global variables for storing path information
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import os

#-------------------------------------------------------------------------------
#   Directory structue definition DO NOT CHANGE!!
#-------------------------------------------------------------------------------

tstar_dir   =  os.environ['CMSSW_BASE'] + "/src/TstarAnalysis/"
setting_dir =  tstar_dir + 'RunSequence/settings/'
cmsrun_dir  =  tstar_dir + 'RunSequence/cmsrun/'
storage_dir =  tstar_dir + 'RunSequence/store/'


run_cfg_file   = cmsrun_dir  + 'run_baseline_selection.py'
crab_cfg_dir   = storage_dir + 'CrabConfig/'
script_dir     = storage_dir + 'Scripts/'
crab_work_dir  = storage_dir + 'CrabWorkSpace/'

#-------------------------------------------------------------------------------
#   Default settings for crab bjobs, change if needed
#-------------------------------------------------------------------------------
crab_default_site = 'T2_CH_CERN'
crab_default_path = '/store/user/yichen/tstar_76X/'
crab_default_lumi = setting_dir + 'lumi_mask_latest.json'


data_global_tag = "76X_dataRun2_v15"
mc_global_tag   = "76X_mcRun2_asymptotic_v12"

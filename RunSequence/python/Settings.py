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
crab_site         = 'T2_TW_NCHC'
crab_siteurl      = 'se01.grid.nchc.org.tw'
crab_user_path    = '/store/user/yichen/'
crab_default_path = crab_user_path + 'tstar_crab/'
xrd_default_path  = '/cms/' + crab_default_path

#-------------------------------------------------------------------------------
#   EOS loading path
#-------------------------------------------------------------------------------
eos_mount_path = os.environ['HOME']+'/eos/'
eos_path       = eos_mount_path + '/cms/'
eos_remoteprefix = 'root://se01.grid.nchc.org.tw/'
edmstorage_dir = '/wk_cms2/yichen/TstarAnalysis/EDMStore_New/'

#-------------------------------------------------------------------------------
#   CMSRUN settings
#-------------------------------------------------------------------------------
data_global_tag = "80X_dataRun2_2016SeptRepro_v4"
mc_global_tag   = "80X_mcRun2_asymptotic_2016_TrancheIV_v6"

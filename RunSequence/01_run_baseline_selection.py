#!/bin/env python
#*******************************************************************************
 #
 #  Filename    : 01_run_baseline_selection.sh
 #  Description : Submitting all designated datasets to crab
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import ManagerUtils.EDMUtils.EDMFileList     as list_util
import TstarAnalysis.RunSequence.Naming      as my_name
import TstarAnalysis.RunSequence.Settings    as my_settings
import sys, os
import optparse
#-------------------------------------------------------------------------------
#   Defining global variables
#-------------------------------------------------------------------------------
config_file_default = """
from CRABClient.UserUtilities import config
config = config()

config.General.requestName = '{0}'
config.General.workArea = '{1}'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '{2}'
config.JobType.maxMemoryMB = 2500 ## Requesting 2.5G of memory!

## Input parameters
config.JobType.pyCfgParams = [
    'Mode={3}',
    'GlobalTag={4}',
    'HLT={5}'
]

config.Data.inputDataset = '{6}'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 16 ## Very fine job splitting
config.Data.outLFNDirBase = '{7}'
config.Data.publication = False

config.Site.storageSite = '{8}'
config.Site.blacklist = ['T2_DE_DESY']
"""


#-------------------------------------------------------------------------------
#   Main control flows
#-------------------------------------------------------------------------------
def MakeCrabFile( dataset, opt ):

    # Preparing variables
    task_name = my_name.GetTaskName( 'tstar' , dataset , opt.mode )
    work_area = my_settings.crab_work_dir
    run_file  = my_settings.cmsrun_dir + 'run_baseline_selection.py'
    mode      = opt.mode
    global_tag= ""
    hlt       = my_name.GetHLT(dataset)
    lfn_dir   = my_settings.crab_default_path
    site      = my_settings.crab_default_site
    lumi_file = my_settings.crab_default_lumi

    if my_name.IsData( dataset ):
        global_tag = my_settings.data_global_tag
    else:
        global_tag = my_settings.mc_global_tag

    file_content     = config_file_default.format(
        task_name  , #{0}
        work_area  , #{1}
        run_file   , #{2}
        mode       , #{3}
        global_tag , #{4}
        hlt        , #{5}
        dataset    , #{6}
        lfn_dir    , #{7}
        site       , #{8}
    )

    ## Writing to Crab file
    crab_config_file = my_name.GetCrabFile('tstarbaseline',dataset,opt.mode)
    my_file = open(  crab_config_file , 'w' )
    my_file.write( file_content )
    my_file.close()
    return crab_config_file



def main():
    parser = optparse.OptionParser()
    parser.add_option('-i', '--inputlist', dest='input', help='list of data sets to generate', default=None, type='string')
    parser.add_option('-m', '--mode',      dest='mode',  help='which mode to run with',        default=None, type='string')
    parser.add_option('-r', '--runcrab'  , dest='run' , action="store_true", help="whether to run submit command", default=False )

    (opt,args) = parser.parse_args()

    if not opt.input or not opt.mode:
        print "Error! [input] nad [mode] inputs are obligatory!"
        parser.print_help()
        return

    with open(opt.input) as f:
        dataset_list = f.readlines()
        for dataset in dataset_list :
            dataset =dataset.strip()
            crab_file = MakeCrabFile( dataset, opt )
            if opt.run :
                print 'Submitting to crab'
                os.system('crab submit '+crab_file)
            else:
                print "Written config to", crab_file


if __name__ == "__main__":
    sys.exit(main())

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

## Input parameters
config.JobType.pyCfgParams = [
    'Mode={3}',
    'GlobalTag={4}',
    'HLT={5}'
]

config.Data.inputDataset = '{6}'
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 8
config.Data.outLFNDirBase = '{7}'
config.Data.publication = False

config.Site.storageSite = '{8}'
"""


#-------------------------------------------------------------------------------
#   Main control flows
#-------------------------------------------------------------------------------
def MakeCrabFile( data_set, opt ):

    # Preparing variables
    task_name = my_name.GetTaskName( 'tstarbaseline' , data_set , opt.mode )
    work_area = my_settings.crab_work_dir
    run_file  = my_settings.cmsrun_dir + 'run_baseline_selection.py'
    mode      = opt.mode
    global_tag= ""
    hlt       = my_name.GetHLT(data_set)
    lfn_dir   = opt.path
    site      = opt.site
    lumi_file = opt.lumi

    if my_name.IsData( data_set ):
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
        data_set   , #{6}
        lfn_dir    , #{7}
        site       , #{8}
    )

    if my_name.IsData( data_set ):
        file_content = file_content + "\nconfig.Data.lumiMask = '{}'\n".format(lumi_file)

    ## Writing to Crab file
    crab_config_file = my_name.GetCrabFile('tstarbaseline',data_set,opt.mode)
    my_file = open(  crab_config_file , 'w' )
    my_file.write( file_content )
    my_file.close()
    return crab_config_file



def main():
    parser = optparse.OptionParser()
    parser.add_option('-i', '--inputlist', dest='input', help='list of data sets to generate', default=None, type='string')
    parser.add_option('-m', '--mode',      dest='mode',  help='which mode to run with',        default=None, type='string')
    parser.add_option('-s', '--site',      dest='site',  help='which site to pass the output', default=my_settings.crab_default_site, type='string')
    parser.add_option('-p', '--path',      dest='path',  help='which path to pass the output', default=my_settings.crab_default_path, type='string')
    parser.add_option('-l', '--lumi',      dest='lumi',  help='lumi mask file to use'        , default=my_settings.crab_default_lumi, type='string')
    parser.add_option('-r', '--runcrab'  , dest='run' , action="store_true", help="whether to run submit command", default=False )

    (opt,args) = parser.parse_args()

    if not opt.input or not opt.mode:
        print "Error! [input] nad [mode] inputs are obligatory!"
        parser.print_help()
        return

    with open(opt.input) as f:
        data_set_list = f.readlines()
        for data_set in data_set_list :
            data_set =data_set.strip()
            crab_file = MakeCrabFile( data_set, opt )
            if opt.run :
                print 'Submitting to crab'
                os.system('crab submit '+crab_file)
            else:
                print "Written config to", crab_file


if __name__ == "__main__":
    sys.exit(main())

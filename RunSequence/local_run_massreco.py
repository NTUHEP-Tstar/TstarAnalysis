#!/bin/env python
#*************************************************************************
#
#  Filename    : 03_run_massreco.py
#  Description : Running mass reconstruction with standard methods
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#*************************************************************************
import optparse
import os
import re
import subprocess
import glob

import TstarAnalysis.RunSequence.Naming   as myname
import TstarAnalysis.RunSequence.Settings as mysetting
import TstarAnalysis.RunSequence.PathVars as mypath


script_template = """
#!/bin/bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd  {0}/RunSequence/
eval `scramv1 runtime -sh`
cmsRun {0}/RunSequence/cmsrun/run_massreco.py Mode={1} maxEvents=-1 sample={2} output={3} Lumimask="{4}"
cp  {3} {5}
rm {3}
"""


def main():
    parser = optparse.OptionParser()
    parser.add_option('-i', '--inputlist', dest='input',
                      help='list of data sets to generate', default=None, type='string')
    parser.add_option('-m', '--mode',      dest='mode',
                      help='which mode to run with',        default=None, type='string')

    (opt, args) = parser.parse_args()
    if not opt.input or not opt.mode:
        print "Error! [input] and [mode] inputs are obligatory!"
        parser.print_help()
        return

    with open(opt.input) as f:
        dataset_list = f.readlines()
        for dataset in dataset_list:
            dataset = dataset.strip()

            filequery = ""

            if "Muon" in  opt.mode:
                filequery = myname.GetEDMStoreGlob( 'tstarbaseline', dataset, "Muon")
            elif "Electron" in opt.mode:
                filequery = myname.GetEDMStoreGlob( 'tstarbaseline', dataset, "Electron")
            else:
                print "ERROR! Unrecongnized mode ", opt.mode
                sys.exit(1)

            # Getting lumimask for data
            Lumimask  = ""
            if myname.IsData(dataset) and "Muon" in opt.mode:
                Lumimask="/wk_cms/yichen/TstarAnalysis/CMSSW_8_0_26_patch1/src/TstarAnalysis/RunSequence/settings/lumimask_2016_master.json"
            elif myname.IsData(dataset) and "Electron" in opt.mode:
                Lumimask="/wk_cms/yichen/TstarAnalysis/CMSSW_8_0_26_patch1/src/TstarAnalysis/RunSequence/settings/lumimask_2016_electron.json"


            # p = subprocess.Popen(
            #    ['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',filequery],
            #    stdout=subprocess.PIPE,
            #    stderr=subprocess.PIPE)
            # out, err = p.communicate()
            file_master_list = glob.glob( filequery )


            file_chunks = [file_master_list[i:i + 16]
                           for i in range(0, len(file_master_list), 16)]

            for index, file_list in enumerate(file_chunks):
                file_list = ["file://"+x for x in file_list]
                sample_input = ','.join(file_list)

                tempoutput       = myname.GetTempOutput( 'massreco',dataset, opt.mode,index  )
                storeoutput      = myname.GetEDMStoreFile( 'massreco', dataset, opt.mode, index )
                script_file_name = myname.GetScriptFile( 'massreco', dataset, opt.mode, index )

                script_content = script_template.format(
                    mysetting.tstar_dir,
                    opt.mode,
                    sample_input,
                    tempoutput,
                    Lumimask,
                    storeoutput,
                )

                script_file = open(script_file_name, 'w')
                script_file.write(script_content)
                script_file.close()
                os.system("chmod +x " + script_file_name)

                print "Writting into file ", script_file_name

if __name__ == "__main__":
    main()

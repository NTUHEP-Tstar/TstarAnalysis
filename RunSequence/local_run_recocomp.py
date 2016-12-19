#!/bin/env python
#*******************************************************************************
 #
 #  Filename    : runHistCompare.py
 #  Description : Generating scripts for running hist compare
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import optparse
import os
import re
import subprocess
channellist= ["ElectronSignal", "MuonSignal"]

import TstarAnalysis.RunSequence.Naming   as myname
import TstarAnalysis.RunSequence.Settings as mysetting
import TstarAnalysis.RunSequence.PathVars as mypath

script_template = """
#!/bin/bash
cd {0}
eval `scramv1 runtime -sh`
cmsRun {0}/RunSequence/cmsrun/run_reco_compare.py Mode={1} sample={2} output={3} maxEvents=-1
xrdcp -f {3} root://{4}/{5}
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
            if "Tstar" not in dataset : ## Must be a tstar signal MC
                print "Skipping dataset ", dataset
                continue

            filequery = ""

            if "Muon" in  opt.mode:
                filequery = myname.GetEDMStoreGlob( 'tstarbaseline', dataset, "Muon")
            elif "Electron" in opt.mode:
                filequery = myname.GetEDMStoreGlob( 'tstarbaseline', dataset, "Electron")
            else:
                print "ERROR! Unrecongnized mode ", opt.mode
                sys.exit(1)

            p = subprocess.Popen(
                ['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',filequery],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE)
            out, err = p.communicate()
            file_master_list = [ os.path.dirname(filequery)+'/'+x for x in  out.split()]

            file_chunks = [file_master_list[i:i + 3]
                           for i in range(0, len(file_master_list), 3)]

            for index, file_list in enumerate(file_chunks):
                sample_input = ','.join(file_list)

                tempoutput       = myname.GetTempOutput( 'recocomp',dataset, opt.mode,index  )
                storeoutput      = myname.GetEDMStoreFile( 'recocomp', dataset, opt.mode, index )
                script_file_name = myname.GetScriptFile( 'recocomp', dataset, opt.mode, index )

                script_content = script_template.format(
                    mysetting.tstar_dir,
                    opt.mode,
                    sample_input,
                    tempoutput,
                    mysetting.crab_siteurl,
                    storeoutput,
                )

                script_file = open(script_file_name, 'w')
                script_file.write(script_content)
                script_file.close()
                os.system("chmod +x " + script_file_name)

                print "Writting into file ", script_file_name

if __name__ == "__main__":
    main()

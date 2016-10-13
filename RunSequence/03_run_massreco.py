#!/bin/env python
#*************************************************************************
#
#  Filename    : 03_run_massreco.py
#  Description : Running mass reconstruction with standard methods
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#*************************************************************************
import glob
import optparse
import os
import pprint

import TstarAnalysis.RunSequence.Naming as myname
import TstarAnalysis.RunSequence.Settings as mysetting
import TstarAnalysis.RunSequence.PathVars as mypath


script_template = """
#!/bin/bash
/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select -b fuse mount $HOME/eos/
cd  {0}/RunSequence/
eval `scramv1 runtime -sh`
cmsRun {0}/RunSequence/cmsrun/run_massreco.py maxEvents=-1 sample={1} output={2} lumimask="{3}"
"""


def main():
    parser = optparse.OptionParser()
    parser.add_option('-i', '--inputlist', dest='input',
                      help='list of data sets to generate', default=None, type='string')
    parser.add_option('-m', '--mode',      dest='mode',
                      help='which mode to run with',        default=None, type='string')
    parser.add_option('-r', '--runlsf', dest='run',
                      help="whether to run command, also specify queue", default=False, type='string')

    (opt, args) = parser.parse_args()
    if not opt.input or not opt.mode:
        print "Error! [input] and [mode] inputs are obligatory!"
        parser.print_help()
        return

    with open(opt.input) as f:
        dataset_list = f.readlines()
        for dataset in dataset_list:
            dataset = dataset.strip()
            glob_query = myname.GetStoreGlob(
                'tstarbaseline', dataset,  opt.mode)
            file_master_list = glob.glob(glob_query)

            file_chunks = [file_master_list[i:i + 8]
                           for i in range(0, len(file_master_list), 8)]

            for index, file_list in enumerate(file_chunks):
                file_list = ['file://' + x for x in file_list]
                sample_input = ','.join(file_list)
                output = myname.GetStoreFile(
                    'massreco', dataset, opt.mode, index)
                script_file_name = myname.GetScriptFile(
                    'massreco', dataset, opt.mode, index)

                lumimask=""
                if myname.IsData( dataset ) :
                    lumimask=mypath.setting_dir + "/lumimask_ICHEP.json"

                script_content = script_template.format(
                    mysetting.tstar_dir,  # {0}
                    sample_input,  # {1}
                    output,  # {2}
                    lumimask
                )

                script_file = open(script_file_name, 'w')
                script_file.write(script_content)
                script_file.close()
                os.system("chmod +x " + script_file_name)

                if not opt.run:
                    print "Writting into file ", script_file_name
                else:
                    print "Do not run the scripts here, navigate to RunSequence/store/LFN_run/"

if __name__ == "__main__":
    main()

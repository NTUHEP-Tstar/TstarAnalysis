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
import subprocess

import TstarAnalysis.RunSequence.Naming as myname
import TstarAnalysis.RunSequence.Settings as mysetting
import TstarAnalysis.RunSequence.PathVars as mypath


script_template = """
#!/bin/bash
cd  {0}/RunSequence/
eval `scramv1 runtime -sh`
cmsRun {0}/RunSequence/cmsrun/run_topselection.py maxEvents=-1 sample={1} output={2}
xrdcp -f {2} root://{3}//{4}
rm {2}
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

            filequery = myname.GetEDMStoreGlob( 'tstarbaseline', dataset, opt.mode )
            p = subprocess.Popen(
                ['/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select','ls',filequery],
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE)
            out, err = p.communicate()
            masterfilelist = [ os.path.dirname(filequery)+'/'+x for x in  out.split()]

            filechunks = [masterfilelist[i:i + 3]
                           for i in range(0, len(masterfilelist), 3)]

            for index, file_list in enumerate(filechunks):
                sample_input = ','.join(file_list)

                tempoutput     = myname.GetTempOutput( 'toplike',dataset, opt.mode,index  )
                storeoutput    = myname.GetEDMStoreFile( 'toplike', dataset, opt.mode, index )
                scriptfilename = myname.GetScriptFile( 'toplike', dataset, opt.mode, index )

                script_content = script_template.format(
                    mysetting.tstar_dir,
                    sample_input,
                    tempoutput,
                    mysetting.crab_siteurl,
                    storeoutput,
                )

                scriptfile = open(scriptfilename, 'w')
                scriptfile.write(script_content)
                scriptfile.close()
                os.system("chmod +x " + scriptfilename)

                print "Writting into file ", scriptfilename

if __name__ == "__main__":
    main()

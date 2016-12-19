#!/bin/env python
#*******************************************************************************
 #
 #  Filename    : 02_retrieve_files.py
 #  Description : Retrieving files of crab output
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import os
import glob
import optparse

import TstarAnalysis.RunSequence.PathVars  as mypath
import TstarAnalysis.RunSequence.Naming    as myname
import TstarAnalysis.RunSequence.Settings  as myset

script_template = """
#!/bin/bash
cd  {0}/RunSequence/
eval `scramv1 runtime -sh`
edmCopyPickMerge inputFiles={1} outputFile={2} &> /dev/null
xrdcp -f {2} root://{3}//{4} &> /dev/null
"""

def main():
    parser = optparse.OptionParser()
    parser.add_option('-i', '--inputlist', dest='input', help='list of data sets to generate', default=None, type='string')
    parser.add_option('-m', '--mode',      dest='mode',  help='which mode to run with',        default=None, type='string')

    (opt,args) = parser.parse_args()

    if not opt.input or not opt.mode:
        print "Error! Both inputs are required"
        parser.print_help()
        return

    with open(opt.input) as f:
        dataset_list = f.readlines()
        for dataset in dataset_list :
            dataset  = dataset.strip() ## Removing redundant characeter like '\n'
            splitting= 0
            if myname.IsData( dataset ):
                splitting = 32
            else:
                splitting = 8
            print dataset
            crabfilelist = myname.GetCrabOutputFileList('tstar',dataset,opt.mode)
            crabfilechunk =  [crabfilelist[i:i + splitting]
                           for i in range(0, len(crabfilelist), splitting)]


            for index,crabfileset in enumerate(crabfilechunk):
                script    = myname.GetScriptFile( 'retrieve', dataset, opt.mode, index)
                storefile = myname.GetEDMStoreFile( 'tstarbaseline', dataset, opt.mode, index )
                tmpfile   = "/tmp/"+os.path.basename(storefile)
                ## Will automatically create directory is doesn't exists
                # os.system( 'xrdcp -f root://{0}//{1} root://{0}//{2}'.format( myset.crab_siteurl, outfile, storefile ) )
                script_content = script_template.format(
                    myset.tstar_dir,
                    ",".join(crabfileset),
                    tmpfile,
                    myset.crab_siteurl,
                    storefile
                )

                script_file = open( script , 'w' )
                script_file.write( script_content )
                script_file.close()

                os.system( 'chmod +x ' + script )
                print "Written scripts to ", script

if __name__ == "__main__":
    main()

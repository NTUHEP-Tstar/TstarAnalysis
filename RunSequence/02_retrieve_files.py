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

            print dataset
            crabfilelist = myname.GetCrabOutputFileList('tstar',dataset,opt.mode)

            for index,outfile in enumerate(crabfilelist):
                storefile = myname.GetEDMStoreFile( 'tstarbaseline', dataset, opt.mode, index )
                ## Will automatically create directory is doesn't exists
                os.system( 'xrdcp -f root://{0}//{1} root://{0}//{2}'.format( myset.crab_siteurl, outfile, storefile ) )

if __name__ == "__main__":
    main()

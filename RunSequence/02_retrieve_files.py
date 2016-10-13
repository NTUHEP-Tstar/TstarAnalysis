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
    parser.add_option('-c', '--copy',      dest='copy',  help='Whether to copy (for outide lxplus)', action='store_true' )

    (opt,args) = parser.parse_args()

    if not opt.input or not opt.mode:
        print "Error! Both inputs are required"
        parser.print_help()
        return

    with open(opt.input) as f:
        dataset_list = f.readlines()
        for dataset in dataset_list :
            dataset  = dataset.strip() ## Removing redundant characeter like '\n'
            store_dir = myname.GetStoreDir     ('tstarbaseline',dataset,opt.mode)
            print "Linking output for data set ", dataset, "in mode" , opt.mode
            os.system( 'mkdir -p '+store_dir )

            if opt.copy:
                print "using copy method!"
                crab_dir         = myname.GetCrabDir      ('tstar',dataset,opt.mode)
                crab_results_dir = myname.GetCrabOutputDir('tstar',dataset,opt.mode)
                os.system( "crab getoutput --checksum=no "+crab_dir )
                output_file_list = glob.glob( crab_results_dir + '*.root')

                for index,output_file in enumerate(output_file_list):
                    store_file = myname.GetStoreFile( 'tstarbaseline', dataset, opt.mode, index )
                    os.system( 'mv {} {}'.format( output_file, store_file ) )
            else:
                print "Linking files to eosmounted directory"
                base_path = myset.eos_path + myset.crab_default_path
                if not os.path.isdir( base_path ) :
                    print "Please mount the eos file system to the location " + myset.eos_path
                query  = base_path + myname.GetPrimary( dataset ) + '/crab_' + myname.GetTaskName( 'tstar', dataset, opt.mode )
                query  = query + '/*/*/*.root'
                output_file_list = glob.glob( query )
                output_file_list.sort() ## Guarantee output alphabetical order to avoid crab-job resubmissions
                for index,output_file in enumerate(output_file_list):
                    store_file = myname.GetStoreFile( 'tstarbaseline', dataset, opt.mode, index )
                    os.system( 'ln -sf {} {}'.format( output_file, store_file ) )

if __name__ == "__main__":
    main()

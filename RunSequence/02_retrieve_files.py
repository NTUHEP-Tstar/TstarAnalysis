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

import TstarAnalysis.RunSequence.PathVars  as my_path
import TstarAnalysis.RunSequence.Naming    as my_name

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
        data_set_list = f.readlines()
        for data_set in data_set_list :
            data_set         = data_set.strip() ## Removing redundant characeter like '\n'
            crab_dir         = my_name.GetCrabDir      ('tstarbaseline',data_set,opt.mode)
            store_dir        = my_name.GetStoreDir     ('tstarbaseline',data_set,opt.mode)
            crab_results_dir = my_name.GetCrabOutputDir('tstarbaseline',data_set,opt.mode)
            print "Retrieving output for data set ", data_set, "in mode" , opt.mode
            os.system( "crab getoutput "+crab_dir )
            os.system( 'mkdir -p '+store_dir )
            output_file_list = glob.glob( crab_results_dir + '*.root')

            for index,output_file in enumerate(output_file_list):
                store_file = my_name.GetStoreFile( 'tstarbaseline', data_set, opt.mode, index )
                os.system( 'mv {} {}'.format( output_file, store_file ) )

if __name__ == "__main__":
    main()
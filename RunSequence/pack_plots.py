#!/bin/env python
#*******************************************************************************
 #
 #  Filename    : pack_plots.py
 #  Description : Packing all plots into single tar file
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import os, glob, sys

dirlist=["CompareDataMC","TstarMassReco", "LimitCalc" ]

tstardir=os.environ["CMSSW_BASE"] + '/src/TstarAnalysis/'

if os.path.exists( "./results" ):
    os.system("rm ./results -rf")

os.system('mkdir -p ./results')

for direc in dirlist:
    os.system( 'mkdir -p ./results/'+direc)
    channellist = glob.glob( tstardir + direc + '/results/*' )
    channellist = [ os.path.basename(x) for x in channellist ]
    print channellist
    for channel in channellist:
        plotfilelist = []
        plotfilelist.extend( glob.glob( tstardir + direc + '/results/' + channel + '/*.png' ) )
        plotfilelist.extend( glob.glob( tstardir + direc + '/results/' + channel + '/*.tex' ) )

        for plotfile in plotfilelist:
            newname = './results/' +direc +'/'+ channel + '_' + os.path.basename( plotfile )
            os.system( 'cp {} {}'.format(plotfile,newname) )

os.system('tar -zcvf results.tar.gz results')
os.system("rm ./results -rf")

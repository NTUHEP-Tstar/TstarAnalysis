#!/bin/env python
#*******************************************************************************
 #
 #  Filename    : pack_plots.py
 #  Description : Packing all plots into single tar file
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import os, glob, sys

dirlist=["CompareDataMC","MassRecoCompare", "LimitCalc" ]

tstardir=os.environ["CMSSW_BASE"] + '/src/TstarAnalysis/'

if os.path.exists( "./results" ):
    os.system("rm ./results -rf")

os.system('mkdir -p ./results')

os.system( 'cp -r --verbose /wk_cms/sam7k9621/ele_TriggerStudy ./results')
os.system( 'cp -r --verbose ' + tstardir + 'SignalMCStudy/results ./results/SignalMCStudy' )
os.system( 'cp -r --verbose ' + tstardir + 'Common/results ./results/WeightStudy' )

for direc in dirlist:
    os.system( 'mkdir -p ./results/'+direc)
    channellist = glob.glob( tstardir + direc + '/results/*' )
    channellist = [ os.path.basename(x) for x in channellist ]
    print channellist
    for channel in channellist:
        filelist = []
        filelist.extend( glob.glob( tstardir + direc + '/results/' + channel + '/*.pdf' ) )
        filelist.extend( glob.glob( tstardir + direc + '/results/' + channel + '/*.tex' ) )

        for myfile in filelist:
            newname = './results/' +direc +'/'+ channel + '_' + os.path.basename( myfile )
            os.system( 'cp --verbose {} {}'.format(myfile,newname) )

os.system('tar -zcvf results.tar.gz results')
os.system("rm ./results -rf")

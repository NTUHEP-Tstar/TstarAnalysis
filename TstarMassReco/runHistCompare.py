#!/bin/env python
#*******************************************************************************
 #
 #  Filename    : runHistCompare.py
 #  Description : Generating scripts for running hist compare
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import os, sys, glob, re
channellist= ["ElectronSignal", "MuonSignal"]

scriptinput = """
#!/bin/bash
cd {}/src/TstarAnalysis/TstarMassReco/
eval `scramv1 runtime -sh`
cmsRun {}/src/TstarAnalysis/TstarMassReco/python/HitFitCompare.py sample={} output={} Mode={} maxEvents=-1
"""

storebase = os.environ["HOME"] + '/eos/cms/store/user/yichen/tstar_store/tstarbaseline/'

for channel in channellist:
    prechannel=channel[:-6] ; # Removing "signal postfix"
    filelist = glob.glob( storebase + prechannel + "/Tstar*.root")
    print filelist
    for inputfile in filelist:

        inputfile=inputfile[len(os.environ['HOME']):] # removing "${HOME}" prefix
        inputfile=inputfile[len('/eos/cms'):] # removing eosmount  prefix

        mass = re.findall(r'\d+', inputfile )[0]
        outputfile = 'results/' + channel + '/TstarM' + str(mass) + ".root"

        scriptfilename = ("run/script_" + channel +'_' + str(mass) + '.sh' )
        scriptfile = open( scriptfilename , 'w' )
        scriptfile.write( scriptinput.format(
            os.environ["CMSSW_BASE"],
            os.environ["CMSSW_BASE"],
            inputfile,
            outputfile,
            channel
        ))
        scriptfile.close()
        os.system("chmod +x "+ scriptfilename)

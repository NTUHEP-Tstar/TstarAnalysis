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
cmsRun python/HitFitCompare.py sample={} output={} maxEvents=-1 &> /dev/null
"""

for channel in channellist:
    filelist = glob.glob( "sample_input/" + channel + "/Tstar*.root");
    for inputfile in filelist:
        mass = re.findall(r'\d+', inputfile )[0]
        outputfile = 'results/' + channel + '/tstar_M' + str(mass) + ".root"

        scriptfilename = ("run/script_" + channel +'_' + str(mass) + '.sh' )
        scriptfile = open( scriptfilename , 'w' )
        scriptfile.write( scriptinput.format(
            os.environ["CMSSW_BASE"],
            'file://' + inputfile,
            outputfile
        ))
        scriptfile.close()
        os.system("chmod +x "+ scriptfilename)

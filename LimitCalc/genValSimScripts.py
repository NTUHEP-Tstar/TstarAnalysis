#!/usr/bin/env python
#*******************************************************************************
 #
 #  Filename    : make_run_val_simfit.py
 #  Description : Generating scripts for command RunValGenFit
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
import os,sys
channellist=["MuonSignal","ElectronSignal"]
masspointlist=[
    "TstarM700",
    "TstarM800",
    "TstarM900",
    "TstarM1000",
    "TstarM1100",
    "TstarM1200",
    "TstarM1300",
    "TstarM1400",
    "TstarM1500",
    "TstarM1600",
]

functionlist=["Exo","Lognorm"]

runnum   = 3000
injlist  = [0,30,300]

header="""
#!/bin/bash
cd {}/src/TstarAnalysis/LimitCalc/
eval `scramv1 runtime -sh`
""".format( os.environ['CMSSW_BASE'] )

cmd = """
./RunValGenFit -c {0} -f {1} -n {2} -m {3} {4} {5} &> /dev/null
"""

plotcmd = "./PlotValGenFit -c {0} -f {1} -a {2}"

for channel in channellist:
    for func in functionlist:
        for inj in injlist :
            for masspoint in masspointlist:
                filename = "run/runsimfitval_{}_{}_{}_a{}.sh".format(
                    channel, func, masspoint, inj )
                script = open( filename, 'w')
                script.write( header )
                script.write(cmd.format( channel, func, runnum , masspoint , "-a" , inj ))
                script.close()
                os.system("chmod +x "+filename)
            print plotcmd.format( channel, func, inj )

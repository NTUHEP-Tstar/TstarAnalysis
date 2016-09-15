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
    "tstar_M700",
    "tstar_M800",
    "tstar_M900",
    "tstar_M1000",
    "tstar_M1100",
    "tstar_M1200",
    "tstar_M1300",
    "tstar_M1400",
    "tstar_M1500",
    "tstar_M1600",
]

functionlist=["Exo","Lognorm"]

runnum   = 3000
injlist  = ["0","30"]

header="""
#!/bin/bash
cd {}/src/TstarAnalysis/LimitCalc/
eval `scramv1 runtime -sh`
""".format( os.environ['CMSSW_BASE'] )

cmd = """
./RunValGenFit -c {0} -f {1} -n {2} -m {3} {4} {5} &> /dev/null
"""

for masspoint in masspointlist:
    for channel in channellist:
        for func in functionlist:
            for inj in injlist :
                filename = "run/runsimfitval_{}_{}_{}_a{}.sh".format(
                    channel, func, masspoint, inj )
                script = open( filename, 'w')
                script.write( header )
                script.write(cmd.format( channel, func, runnum , masspoint , "-a" , inj ))
                script.close()
                os.system("chmod +x "+filename)

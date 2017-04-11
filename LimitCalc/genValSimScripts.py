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

functionlist=["Lognorm","Exo","LogExt3", "LogExt4"]

runnum   = 100
injlist  = [0,1]

header="""
#!/bin/bash
source /cvmfs/cms.cern.ch/cmsset_default.sh
cd {}/src/TstarAnalysis/LimitCalc/
eval `scramv1 runtime -sh`
""".format( os.environ['CMSSW_BASE'] )

cmd = """
RunValGenFit --channel {0} --fitmethod SimFit --fitfunc {1} --num {2} --masspoint {3} -e Rereco {4} {5}
"""

plotcmd = "./PlotValGenFit --channel {0} --fitmethod SimFit --fitfunc {1} --relmag {2} -e Rereco"

for channel in channellist:
    for func in functionlist:
        for inj in injlist :
            for masspoint in masspointlist:
                filename = "run/runsimfitval_{}_{}_{}_r{}.sh".format(
                    channel, func, masspoint, inj )
                script = open( filename, 'w')
                script.write( header )
                script.write(cmd.format( channel, func, runnum , masspoint , "--relmag" , inj ))
                script.close()
                os.system("chmod +x "+filename)
            print plotcmd.format( channel, func, inj )

### other alternative
masspointlist=[
    "TstarM800",
    "TstarM1200",
    "TstarM1600",
]

bkgtypelist = [
    "ISRup",
    "ISRdown",
    "FSRup",
    "FSRdown",
]

functionlist = ["Lognorm"]

cmd = """
RunValGenFit --channel {0} --fitmethod SimFit --fitfunc {1} --num {2} --masspoint {3} -e Rereco {4} {5} --bkgtype {6}
"""

plotcmd = "./PlotValGenFit --channel {0} --fitmethod SimFit --fitfunc {1} --relmag {2} -e Rereco --bkgtype {3}"

for channel in channellist:
    for func in functionlist:
        for inj in injlist :
            for bkgtype in bkgtypelist:
                for masspoint in masspointlist:
                    filename = "run/runsimfitval_{}_{}_{}_r{}_{}.sh".format( channel, func, masspoint, inj, bkgtype )
                    script = open( filename, 'w')
                    script.write( header )
                    script.write( cmd.format( channel, func, runnum , masspoint , "--relmag" , inj, bkgtype ) )
                    script.close()
                    os.system("chmod +x "+filename)
                    print plotcmd.format( channel, func, inj, bkgtype )

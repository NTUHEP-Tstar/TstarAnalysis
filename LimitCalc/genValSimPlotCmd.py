#!/bin/env  python2
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
functionlist=["Lognorm","Exo","LogExt3"]
injlist  = [0,1]

plotcmd = "./PlotValGenFit --channel {0} --fitmethod SimFit --fitfunc {1} --relmag {2} -e Rereco"

for channel in channellist:
    for func in functionlist:
        for inj in injlist :
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

plotcmd = "./PlotValGenFit --channel {0} --fitmethod SimFit --fitfunc {1} --relmag {2} -e Rereco {3} {4} --masstag={5}"

for channel in channellist:
    for func in functionlist:
        for inj in injlist :
            for bkgtype in bkgtypelist:
                for masspoint in masspointlist:
                    print plotcmd.format( channel, func, inj, "--bkgtype", bkgtype, masspoint  )

bkgtypelist=[1.20,1.55]

for channel in channellist:
    for func in functionlist:
        for inj in injlist :
            for bkgtype in bkgtypelist:
                for masspoint in masspointlist:
                    print plotcmd.format( channel, func, inj, "--forcerho", bkgtype, masspoint )

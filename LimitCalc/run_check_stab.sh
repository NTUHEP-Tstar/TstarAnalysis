#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : run_check_stab.sh
 #  Description : Script for submitting onto queue.
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
cd /afs/cern.ch/work/y/yichen/TstarAnalysis/CMSSW_8_0_12/src/TstarAnalysis/LimitCalc
eval `scramv1 runtime -sh`
./CheckSimFitStab -c MuonSignal     -f Fermi -r 3000
./CheckSimFitStab -c ElectronSignal -f Fermi -r 3000
./CheckSimFitStab -c MuonSignal     -f Exo   -r 3000
./CheckSimFitStab -c ElectronSignal -f Exo   -r 3000

#!/bin/bash
cd /afs/cern.ch/work/y/yichen/TstarAnalysis/CMSSW_8_0_12/src/TstarAnalysis/LimitCalc
eval `scramv1 runtime -sh`
./CheckSimFitStab -c MuonSignal -f Exo -r 1000 -a 3
./CheckSimFitStab -c MuonSignal -f Exo -r 1000 -a 10
./CheckSimFitStab -c MuonSignal -f Exo -r 1000 -a 30
./CheckSimFitStab -c MuonSignal -f Exo -r 1000 -x 3.
./CheckSimFitStab -c MuonSignal -f Exo -r 1000 -x 1.
./CheckSimFitStab -c MuonSignal -f Exo -r 1000 -x 0.3
./CheckSimFitStab -c MuonSignal -f Exo -r 1000 -x 0.1

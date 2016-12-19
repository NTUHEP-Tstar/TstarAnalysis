#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : MakeDataPileup.sh
 #  Description : Running the standard commands for getting data pile up histrogram
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
 #  - Main Reference:
 #     https://twiki.cern.ch/twiki/bin/view/CMS/PileupJSONFileforData
 #  - Minimum bias cross section reference:
 #     69200 : https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData#Pileup_JSON_Files_For_Run_II
 #
 #     72383 66016 - 4.6% error in same page
 #
 #     62000 : "Best Fit" by trial and error.
 #
#*******************************************************************************

xseclist="69200 72383 66016 62000"

for xsec in $xseclist ; do
   pileupCalc.py \
     -i data/lumimask_latest.json            \
     --inputLumiJSON data/pileup_latest.txt  \
     --calcMode true                         \
     --minBiasXsec ${xsec}                   \
     --maxPileupBin  100                     \
     --numPileupBins 100                     \
     results/pileuphist_${xsec}.root

   # ./MakePileupWeights $xsec
done

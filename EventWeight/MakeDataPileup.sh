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
 #     71620 : https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/583/3.html
 #     69200 : https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData#Pileup_JSON_Files_For_Run_II
 #
#*******************************************************************************

xseclist="62000 69200 71260"

for xsec in $xseclist ; do
   pileupCalc.py \
     -i data/lumi_mask_latest.json           \
     --inputLumiJSON data/pileup_latest.txt  \
     --calcMode true                         \
     --minBiasXsec ${xsec}                   \
     --maxPileupBin  50                      \
     --numPileupBins 50                      \
     results/pileuphist_${xsec}.root
done

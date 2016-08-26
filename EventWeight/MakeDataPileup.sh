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
 #     https://hypernews.cern.ch/HyperNews/CMS/get/luminosity/583/3.html
 #
#*******************************************************************************

pileupCalc.py \
     -i data/lumi_mask_latest.json           \
     --inputLumiJSON data/pileup_latest.txt  \
     --calcMode true                         \
     --minBiasXsec 71260                     \
     --maxPileupBin  50                      \
     --numPileupBins 50                      \
     results/pileuphist.root

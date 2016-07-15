#*******************************************************************************
 #
 #  Filename    : make_compare_file.sh
 #  Description : execute cmsRun python/HitFitCompare.py to get comparison file
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
#!/bin/bash

cmsRun python/HitFitCompare.py \
   sample=file://samples/MuonSignal/TstarTstarToTgluonTgluon_M-1000_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root \
   output=results/MuonSignal_M1000.root \
   maxEvents=-1


cmsRun python/HitFitCompare.py \
   sample=file://samples/MuonSignal/TstarTstarToTgluonTgluon_M-700_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root \
   output=results/MuonSignal_M700.root \
   maxEvents=-1

cmsRun python/HitFitCompare.py \
   sample=file://samples/MuonSignal/TstarTstarToTgluonTgluon_M-1600_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root \
   output=results/MuonSignal_M1600.root \
   maxEvents=-1

########################################

cmsRun python/HitFitCompare.py \
   sample=file://samples/ElectronSignal/TstarTstarToTgluonTgluon_M-700_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root \
   output=results/ElectronSignal_M700.root \
   maxEvents=-1

cmsRun python/HitFitCompare.py \
   sample=file://samples/ElectronSignal/TstarTstarToTgluonTgluon_M-1000_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root \
   output=results/ElectronSignal_M1000.root \
   maxEvents=-1

cmsRun python/HitFitCompare.py \
   sample=file://samples/ElectronSignal/TstarTstarToTgluonTgluon_M-1000_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root \
   output=results/ElectronSignal_M1000.root \
   maxEvents=-1

#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : run_all.sh
 #  Description : Running all the production on the test files
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
cd /afs/cern.ch/work/y/yichen/TstarAnalysis/CMSSW_7_6_4/src/TstarAnalysis/TstarMassReco/test/
eval `scramv1 runtime -sh`
cmsRun HitFitCompare.py maxEvents=-1 sample=file://ElectronSignal_Input.root          output=ElectronSignal_Output.root
cmsRun HitFitCompare.py maxEvents=-1 sample=file://ElectronSignal_HighMass_Input.root output=ElectronSignal_HighMass_Output.root
cmsRun HitFitCompare.py maxEvents=-1 sample=file://ElectronSignal_LowMass_Input.root  output=ElectronSignal_LowMass_Output.root
cmsRun HitFitCompare.py maxEvents=-1 sample=file://MuonSignal_Input.root              output=MuonSignal_Output.root
cmsRun HitFitCompare.py maxEvents=-1 sample=file://MuonSignal_HighMass_Input.root     output=MuonSignal_HighMass_Output.root
cmsRun HitFitCompare.py maxEvents=-1 sample=file://MuonSignal_LowMass_Input.root      output=MuonSignal_LowMass_Output.root

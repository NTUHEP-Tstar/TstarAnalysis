#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : cms_run.all
 #  Description : Scripts for generation required files
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

for mass in {700..1600..100}; do
  echo cmsRun python/JetLepTest.py sample=file:/wk_cms2/yichen/MiniAOD/TstarM${mass}.root output=results/jetlep_M${mass} maxEvents=-1;
done

#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : RunEverything.sh
 #  Description : Quick file for generating commands to run everything
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
channellist="MuonBaseline ElectronBaseline MuonSignal ElectronSignal MuonControl ElectronControl MuonTopLike ElectronTopLike"
grouplist="TTBoson TTJets SingleTop SingleBoson DiBoson Data TstarM700 TstarM800 TstarM900 TstarM1000 TstarM1100 TstarM1200 TstarM1300 TstarM1400 TstarM1500 TstarM1600"
typelist="Err Qk Tb"
cmdlist="MakeSummaryTable KinematicCompare FullCompare"

for channel in $channellist; do
   for group in $grouplist ; do
      for type in $typelist ; do
         echo "FillHistogram -c $channel -g $group -t $type"
      done
   done
done

for cmd in $cmdlist ; do
   for channel in $channellist; do
      echo $cmd -c $channel
   done
done

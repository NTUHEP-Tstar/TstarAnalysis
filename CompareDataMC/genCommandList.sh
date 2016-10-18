#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : RunEverything.sh
 #  Description : Quick file for generating commands to run everything
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
channellist="MuonBaseline ElectronBaseline MuonSignal ElectronSignal"
cmdlist="MakeSummaryTable KinematicCompare FullCompare"
for cmd in $cmdlist ; do
   for channel in $channellist; do
      echo $cmd -c $channel
   done
done

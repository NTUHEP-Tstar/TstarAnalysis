#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : RunEverything.sh
 #  Description : Quick file for generating commands to run everything
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************
channellist="MuonSignal ElectronSignal MuonControl ElectronControl MuonTopLike ElectronTopLike"
grouplist="TTBoson TTJets SingleTop SingleBoson TstarM700 TstarM800 TstarM900 TstarM1000 TstarM1100 TstarM1200 TstarM1300 TstarM1400 TstarM1500 TstarM1600 Data"
typelist="Err Qk Tb"
eralist="Rereco"
cmdlist="MakeSummaryTable KinematicCompare FullCompare"

for channel in $channellist; do
   for group in $grouplist ; do
      for type in $typelist ; do
         if [[ $group == "Data" ]]; then
            for era in $eralist ; do
            echo "FillHistogram -c $channel -g $group -t $type -e $era"
            done
         else
            echo "FillHistogram -c $channel -g $group -t $type"
         fi
      done
   done
done

for cmd in $cmdlist ; do
   for channel in $channellist; do
      for era in $eralist ; do
         echo "$cmd -c $channel -e $era"
      done
   done
done

for group in "TstarM1200" "TstarM1600" "TstarM800" ; do
  for channel in $channellist ; do
    echo "ErrorCompare -c $channel -g $group"
  done
done

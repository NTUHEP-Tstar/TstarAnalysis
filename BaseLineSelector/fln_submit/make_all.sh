#!/bin/bash
#-------------------------------------------------------------------------------
#
#   file        : make_all.sh
#   descriptions: submit all data samples designated to user
#   usage       : ./make_all.sh
#
#-------------------------------------------------------------------------------

for sample in $(cat MC_List.asc ) ; do
   echo "working on ${sample}..."
   ./make_scripts.sh ElectronSignal $sample
   ./make_scripts.sh MuonSignal $sample
done

for sample in $(cat Data_List.asc ); do
   echo "working on ${sample}..."
   if [[ $sample == *"Muon"*  ]]; then
      ./make_scripts.sh MuonSignal $sample
   elif [[ $sample == *"Electron"* ]]; then
      ./make_scripts.sh ElectronSignal $sample
   fi
done

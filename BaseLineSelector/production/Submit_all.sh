#!/bin/bash
#-------------------------------------------------------------------------------
#
#   file        : submit_all.sh
#   descriptions: submit all data samples designated to user
#   usage       : ./submit_all.sh
#
#-------------------------------------------------------------------------------

for sample in $(cat MC_List.asc ) ; do
   echo "working on ${sample}..."
   ./Submit_Sample.sh muchannel $sample
   ./Submit_Sample.sh elchannel $sample
done

for sample in $(cat Data_List.asc ); do
   echo "working on ${sample}..."
   if [[ $sample == *"Muon"*  ]]; then
      ./Submit_Sample.sh muchannel $sample
   elif [[ $sample == *"Electron"* ]]; then
      ./Submit_Sample.sh elchannel $sample
   fi
done

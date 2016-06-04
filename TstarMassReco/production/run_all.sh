#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : run_all.sh
 #  Description : Running the production of all the sample
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

SOURCE_DIR=/wk_cms/yichen/TstarAnalysis/filtered_MiniAODs/
OUTPUT_DIR=$PWD/MASSREO
LOG_DIR=$PWD/LOG
MAX_INSTANCE=$(cat /proc/cpuinfo | grep processor | wc --lines)
MAX_INSTANCE=$((MAX_INSTANCE/2))

for channel in MuonSignal ElectronSignal ; do
   mkdir -p $OUTPUT_DIR/$channel
   mkdir -p $LOG_DIR/$channel
   for sample_dir in $(ls -d $SOURCE_DIR/$channel/* ) ; do
      sample=${sample_dir##*/};

      if [[ $(ls ${sample_dir}/*.root | wc -l )  -lt 1 ]] ; then
         continue;
      fi

      ls ${sample_dir}/*.root  > $PWD/inputlist

      split -l 10 $PWD/inputlist $PWD/inputlist_

      for inputlist_file in $(ls $PWD/inputlist_* ); do
         label=${inputlist_file##*inputlist_};
         sample_option=""
         for file in $( cat $inputlist_file ); do
            sample_option="file://${file},"${sample_option}
         done
         sample_option=${sample_option%,}

         output_option=${OUTPUT_DIR}/${channel}/${sample}_${label}.root
         output_log=${LOG_DIR}/${channel}/${sample}_${label}.txt

         echo "Sending to background ${channel} ${sample} ${label}"
         instance_count=$( ps aux | grep ${USER} | grep "cmsRun" | grep -v grep | wc -l )
         while [[ $instance_count -gt  $MAX_INSTANCE ]]; do
            echo "Already too many instances! ${instance_count}"
            sleep 10
            instance_count=$( ps aux | grep ${USER} | grep "cmsRun" | grep -v grep | wc -l )
         done
         cmsRun $PWD/cmsRun_cfg.py \
         sample=$sample_option \
         output=$output_option \
         maxEvents=-1 &> $output_log &
      done

      rm $PWD/inputlist*
   done
done

instance_count=$( ps aux | grep ${USER} | grep "cmsRun" | grep -v grep | wc -l )
while [[ $instance_count != "0" ]]; do
   printf "\rStill ${instance_count} running..."
   instance_count=$( ps aux | grep ${USER} | grep "cmsRun" | grep -v grep | wc -l )
   sleep 10
done

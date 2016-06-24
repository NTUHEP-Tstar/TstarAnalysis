#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : run_all.sh
 #  Description : Running the production of all the sample
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

SOURCE_DIR=$PWD/../../BaseLineSelector/fln_submit/EDM/
OUTPUT_DIR=$PWD/MASSRECO
LOG_DIR=$PWD/LOG
SH_DIR=$PWD/SH
MAX_INSTANCE=$(cat /proc/cpuinfo | grep processor | wc --lines)
MAX_INSTANCE=$((MAX_INSTANCE/2))

for channel in MuonSignal ElectronSignal ; do
   mkdir -p $OUTPUT_DIR/$channel
   mkdir -p $LOG_DIR/$channel
   mkdir -p $SH_DIR/$channel
   for sample_dir in $(ls -d $SOURCE_DIR/$channel/* ) ; do
      sample=${sample_dir##*/};

      if [[ $(ls ${sample_dir}/*.root | wc -l )  -lt 1 ]] ; then
         continue;
      fi

      ls ${sample_dir}/*.root  > $PWD/inputlist

      split -l 5 $PWD/inputlist $PWD/inputlist_

      for inputlist_file in $(ls $PWD/inputlist_* ); do
         label=${inputlist_file##*inputlist_};
         sample_option=""
         for file in $( cat $inputlist_file ); do
            sample_option="file://${file},"${sample_option}
         done
         sample_option=${sample_option%,}

         output_option=${OUTPUT_DIR}/${channel}/${sample}_${label}.root
         output_log=${LOG_DIR}/${channel}/${sample}_${label}.txt
         output_sh=${SH_DIR}/${channel}/${sample}_${label}.sh

         cmd="cmsRun $PWD/cmsRun_cfg.py"
         cmd=${cmd}" sample=$sample_option"
         cmd=${cmd}" output=$output_option"
         cmd=${cmd}" maxEvents=-1 &> $output_log"
         echo "#!/bin/bash" > $output_sh
         echo "cd ${PWD}" >>  $output_sh
         echo "eval \`scramv1 runtime -sh\`" >> $output_sh
         echo "$cmd" >> $output_sh

         chmod +x $output_sh
      done

      rm $PWD/inputlist*
   done
done

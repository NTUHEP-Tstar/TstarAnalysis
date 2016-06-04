#*******************************************************************************
 #
 #  Filename    : make_scripts
 #  Description : Make run scripts for a single submission dataset
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
 #  Usage:
 #     ./make_scripts [channel] [dataset]
 #
#*******************************************************************************
#!/bin/bash

LOG_DIR=${PWD}/LOG
EDM_DIR=${PWD}/EDM
SH_DIR=${PWD}/SH
SPLIT_PREFIX=${PWD}/.temp
LUMIMASK=${PWD}/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_v2.txt

#-------------------------------------------------------------------------------
#   Main control flow
#-------------------------------------------------------------------------------
function parse_argument()
{
   if [[ $# != 2 ]]; then
      echo "Error! Illegal number fo arguments"
      exit 1
   fi

   if [[ $1 == "ElectronSignal" ]]; then
      echo "Running on electron signal channel"
   elif [[ $1 == "MuonSignal" ]]; then
      echo "Running on muon signal channel"
   else
      echo "Error! Unknown channel! (${1})"
      exit 1
   fi
}

function main()
{
   parse_argument $@
   channel=$1
   dataset=$2
   name=$( make_name $dataset ) || exit $?
   make_directories $channel $name

   file_sections=$( make_file_list $dataset )
   for file in $file_sections; do
      label=${file/${SPLIT_PREFIX}/}
      echo "Working on Channel ($channel), dataset($dataset) [label $label]"

      sample_input=$( make_input_list $file )
      output=${EDM_DIR}/${channel}/${name}/tstarBaseline_${label}.root
      sh_file=${SH_DIR}/${channel}/${name}/run_${label}.sh
      log_file=${LOG_DIR}/${channel}/${name}/log_${label}.txt
      cmd="cmsRun $PWD/run_production.py"
      cmd="$cmd maxEvents=-1"
      cmd="$cmd sample=$sample_input"
      cmd="$cmd output=${output}"
      if [[ $channel == "ElectronSignal" ]]; then
         cmd="$cmd Mode=1"
      elif [[ $channel == "MuonSignal" ]]; then
         cmd="$cmd Mode=0"
      fi
      if [[ $sample_input == *data* ]]; then
         cmd="$cmd LumiMask=$LUMIMASK"
      fi
      echo "#!/bin/bash" > $sh_file
      echo "cd ${PWD}" >>  $sh_file
      echo "eval \`scramv1 runtime -sh\`" >> $sh_file
      echo "$cmd &> $log_file" >> $sh_file

      chmod +x $sh_file
   done
   rm -rf $file_sections
}

#-------------------------------------------------------------------------------
#   Helper functions
#-------------------------------------------------------------------------------
function make_name()
{
   dataset=$1

   short_label=""
   if [[ $dataset ==  *"RunIIFall15MiniAODv2-PU25nsData2015v1"* ]]; then
      short_label=$(echo $dataset | awk -F "/" '{print $2}')
   elif [[ $dataset == *"16Dec2015"* ]]; then
      short_label=$(echo $dataset | awk -F "/" '{print $2"_"$3}')
   else
      >&2 echo "Unrecognized dataset! ($dataset)"
      exit 1
   fi
   echo $short_label

}

function make_directories()
{
   channel=$1
   name=$2

   mkdir -p  ${SH_DIR}/${channel}/${name}
   mkdir -p  ${EDM_DIR}/${channel}/${name}
   mkdir -p  ${LOG_DIR}/${channel}/${name}
}

function make_file_list()
{
   sample=$1
   das_client --limit=100000000 --query="file dataset=$sample" | grep "store" > .temp.txt
   split -l 8 .temp.txt $SPLIT_PREFIX
   rm .temp.txt
   ls "$SPLIT_PREFIX"*
}

function make_input_list()
{
   file=$1
   ans=""
   for input in $(cat $file) ; do
      ans=${input},${ans}
   done
   ans=${ans%,}
   echo $ans
}

main $@

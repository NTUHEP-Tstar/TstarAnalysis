#!/bin/bash
#-------------------------------------------------------------------------------
#
#   File        : Submit_Sample.sh
#   Descriptions: Submit a single data sample
#   Usage       : ./Submit_Sample.sh [sample name]
#
#-------------------------------------------------------------------------------
Config_dir=$PWD/crab_config/

#--------------------------------------------------------------------------------
#  Pre-requisites
#--------------------------------------------------------------------------------
if [[ $# != 2 ]]; then
   echo "Error! Expect exactly two arguments command!"
   exit 1
fi

if [[ ! -d $Config_dir ]] ; then
   mkdir $Config_dir
fi

#--------------------------------------------------------------------------------
#  Main control flow
#--------------------------------------------------------------------------------
function main(){
   mode=$1
   dataset=$2

   modeopt=$( GetModeOpt $mode ) || exit $?
   name=$( MakeName $dataset $mode ) || exit $?
   template=""
   if [[ $dataset == *"16Dec2015"* ]]; then
      template=${PWD}/data_crab_template.py
   else
      template=${PWD}/mc_crab_template.py
   fi
   crab_file=${Config_dir}/${name}.py

   ## Making crab configuration file
   cat $template                    |
   sed "s@DATASET@${dataset}@"      |
   sed "s@RUNMODE@${modeopt}@"      |
   sed "s@NAME@${name}@" > $crab_file

   ## Submitting crab job
   # export SCRAM_ARCH=slc6_amd64_gcc491
   source /cvmfs/cms.cern.ch/crab3/crab.sh
   crab submit -c $crab_file
}

function GetModeOpt(){
   if [[ $1 == "muchannel" ]]; then
      echo "0"
   elif [[ $1 == "elchannel" ]]; then
      echo "1"
   else
      >&2 echo "Unrecognized option! ($1)"
      exit 1
   fi
}

function MakeName(){
   dataset=$1
   mode=$2

   short_label=""
   if [[ $dataset ==  *"RunIIFall15MiniAODv2-PU25nsData2015v1"* ]]; then
      short_label=$(echo $dataset | awk -F "/" '{print $2}')
   elif [[ $dataset == *"16Dec2015"* ]]; then
      short_label=$(echo $dataset | awk -F "/" '{print $2"_"$3}')
   else
      >&2 echo "Unrecognized dataset! ($dataset)"
      exit 1
   fi

   short_label=${short_label}_${mode}
   echo $short_label
}

## Executing main function
main $@

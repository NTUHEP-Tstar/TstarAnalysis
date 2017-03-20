#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : run_most.sh
 #  Description : Running most of the availiable options
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

channel_list="MuonSignal ElectronSignal"
method_list="SimFit Template"
fitfunc_list="Fermi Exo Lognorm"
combine_method="Asymptotic"

for method in $method_list ; do
   for fitfunc in $fitfunc_list ; do
         for channel in $channel_list ; do
            echo "MakeRooFit      --channel $channel --fitmethod $method --fitfunc $fitfunc"
            if [ $method == "SimFit" ] && [ $fitfunc == "Lognorm" ] ; then
              echo "PlotLimit       --channel $channel --fitmethod $method --fitfunc $fitfunc -x $combine_method"
              echo "DisableNuisance --channel $channel --fitmethod $method --fitfunc $fitfunc -x $combine_method"
              echo "PlotCombinePull --channel $channel --fitmethod $method --fitfunc $fitfunc --mass TstarM800"
              echo "PlotCombinePull --channel $channel --fitmethod $method --fitfunc $fitfunc --mass TstarM1200"
              echo "PlotCombinePull --channel $channel --fitmethod $method --fitfunc $fitfunc --mass TstarM1600"
            fi
         done
         if [ $method == "SimFit" ] && [ $fitfunc == "Lognorm" ] ; then
           echo "MergeCards      --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --channellist $channel_list "
           echo "PlotLimit       --channel SignalMerge --fitmethod $method --fitfunc $fitfunc -x $combine_method"
           echo "DisableNuisance --channel SignalMerge --fitmethod $method --fitfunc $fitfunc -x $combine_method"
           echo "PlotCombinePull --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --mass TstarM800"
           echo "PlotCombinePull --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --mass TstarM1200"
           echo "PlotCombinePull --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --mass TstarM1600"
         fi
   done
done

## Commands for background function comparison
channel_list="MuonSignal MuonControl ElectronSignal ElectronControl"
funclist="Lognorm LogExt3 LogExt4 LogExt5"
samplelist="Background"

for channel in $channel_list ; do
  for sample in $samplelist ; do
    echo "CompareFitFunc --channel $channel -f $funclist -s $sample"
  done
done

channel_list="MuonSignal ElectronSignal"

exttaglist="--mucut=35 --mucut=40 --mucut=50 --mucut=60 --masscut=500 --masscut=600 --muiso=0.12 --muiso=0.09 --muiso=0.06 --leadjetpt=50 --leadjetpt=80 --recoalgo=tstarMassReco7jet --recoalgo=tstarMassReco8jet --scaleres=1.2 --scaleres=0.8"

for exttag in $exttaglist ; do
  for channel in $channel_list ; do
    echo "MakeRooFit  --channel $channel --fitmethod SimFit --fitfunc  Lognorm ${exttag}"
    echo "PlotLimit   --channel $channel --fitmethod SimFit --fitfunc  Lognorm ${exttag} -x $combine_method"
  done
done

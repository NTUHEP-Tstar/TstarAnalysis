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
era_list="Rereco"
combine_method="Asymptotic"

for method in $method_list ; do
   for fitfunc in $fitfunc_list ; do
      for era in $era_list ; do
         for channel in $channel_list ; do
            echo "MakeRooFit      --channel $channel --fitmethod $method --fitfunc $fitfunc --era $era "
            if [ $method == "SimFit" ] && [ $fitfunc == "Lognorm" ] ; then
              echo "PlotLimit       --channel $channel --fitmethod $method --fitfunc $fitfunc -x $combine_method --era $era"
              echo "DisableNuisance --channel $channel --fitmethod $method --fitfunc $fitfunc -x $combine_method --era $era"
              echo "PlotCombinePull --channel $channel --fitmethod $method --fitfunc $fitfunc --era $era --mass TstarM800"
              echo "PlotCombinePull --channel $channel --fitmethod $method --fitfunc $fitfunc --era $era --mass TstarM1200"
              echo "PlotCombinePull --channel $channel --fitmethod $method --fitfunc $fitfunc --era $era --mass TstarM1600"
            fi
         done
         if [ $method == "SimFit" ] && [ $fitfunc == "Lognorm" ] ; then
           echo "MergeCards      --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --channellist $channel_list --era $era"
           echo "PlotLimit       --channel SignalMerge --fitmethod $method --fitfunc $fitfunc -x $combine_method --era $era"
           echo "DisableNuisance --channel SignalMerge --fitmethod $method --fitfunc $fitfunc -x $combine_method --era $era"
           echo "PlotCombinePull --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --era $era --mass TstarM800"
           echo "PlotCombinePull --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --era $era --mass TstarM1200"
           echo "PlotCombinePull --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --era $era --mass TstarM1600"
         fi
      done
   done
done

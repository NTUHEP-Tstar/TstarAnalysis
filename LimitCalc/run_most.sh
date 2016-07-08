#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : run_most.sh
 #  Description : Running most of the availiable options
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

channel_list="MuonSignal ElectronSignal"
method_list="Template SimFit BiasSimFit"
fitfunc_list="Fermi Exo"
combine_method="Asymptotic"

for method in $method_list ; do
   for fitfunc in $fitfunc_list ; do
      for channel in $channel_list ; do
         echo "MakeRooFit --channel $channel --fitmethod $method --fitfunc $fitfunc"
         echo "PlotLimit  --channel $channel --fitmethod $method --fitfunc $fitfunc --combine_method $combine_method --runcombine"
      done
      echo "MergeCards --fitmethod $method --fitfunc $fitfunc"
      echo "PlotLimit --channel SignalMerge --fitmethod $method --fitfunc $fitfunc --combine_method $combine_method --runcombine"
   done
done

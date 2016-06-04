#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : RunEverything.sh
 #  Description : Quick file for generating commands to run everything
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

channels="MuonSignal ElectronSignal"
methods="Template SimFit"
FitFuncs="Fermi Exo"
combine_method="Asymptotic"
MODE="echo"


for channel in $channels ; do
   cmd="MakeSummaryTable $channel"
   $MODE $cmd
   cmd="KinematicCompare $channel"
   $MODE $cmd

done

for method in $methods ; do
   for fitfunc in $FitFuncs; do
      for channel in $channels; do
         cmd="MakeRooFit --method $method --channel $channel --fitfunc $fitfunc"
         $MODE $cmd
         cmd="PlotLimit --method $method --channel $channel --fitfunc $fitfunc --combine_method $combine_method --runcombine"
         $MODE $cmd
      done

      cmd="MergeCards --method $method --fitfunc $fitfunc"
      $MODE $cmd
      cmd="PlotLimit --method $method --channel SignalMerge --fitfunc $fitfunc --combine_method $combine_method --runcombine"
      $MODE $cmd
   done
done

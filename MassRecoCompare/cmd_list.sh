#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : cmd_list.sh
 #  Description : List of commands to run for this directory (don't make executable)
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

./CompareMethods -c MuonSignal -m 800  -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b
./CompareMethods -c MuonSignal -m 1200 -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b
./CompareMethods -c MuonSignal -m 1600 -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b

./CompareMethods -c MuonSignal -m 800  -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c MuonSignal -m 1200 -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c MuonSignal -m 1600 -o chisqstd     -x ChiSq6jet2b

./CompareMethods -c ElectronSignal -m 800  -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c ElectronSignal -m 1200 -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c ElectronSignal -m 1600 -o chisqstd     -x ChiSq6jet2b

./CompareMethods -c MuonSignal -m 800  -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b
./CompareMethods -c MuonSignal -m 1200 -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b
./CompareMethods -c MuonSignal -m 1600 -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b

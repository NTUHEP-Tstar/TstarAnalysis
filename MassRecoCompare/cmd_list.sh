#!/bin/bash
#*******************************************************************************
 #
 #  Filename    : cmd_list.sh
 #  Description : List of commands to run for this directory (don't make executable)
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
#*******************************************************************************

./CompareMethods -c MuonSignal     -m 800  -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b
./CompareMethods -c MuonSignal     -m 1200 -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b
./CompareMethods -c MuonSignal     -m 1600 -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b
./CompareMethods -c ElectronSignal -m 800  -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b
./CompareMethods -c ElectronSignal -m 1200 -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b
./CompareMethods -c ElectronSignal -m 1600 -o hitfitVchisq -x ChiSq6jet2b NoTopConstrain6j2b


./CompareMethods -c MuonSignal     -m 800  -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b
./CompareMethods -c MuonSignal     -m 1200 -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b
./CompareMethods -c MuonSignal     -m 1600 -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b
./CompareMethods -c ElectronSignal -m 800  -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b
./CompareMethods -c ElectronSignal -m 1200 -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b
./CompareMethods -c ElectronSignal -m 1600 -o chisqjetnum  -x ChiSq6jet2b ChiSq8jet2b

./CompareMethods -c MuonSignal     -m 800  -o gluonconstrain  -x ChiSq6jet2b ChiSq6jet2bgjet2 ChiSq8jet2bgjet4
./CompareMethods -c MuonSignal     -m 1200 -o gluonconstrain  -x ChiSq6jet2b ChiSq6jet2bgjet2 ChiSq8jet2bgjet4
./CompareMethods -c MuonSignal     -m 1600 -o gluonconstrain  -x ChiSq6jet2b ChiSq6jet2bgjet2 ChiSq8jet2bgjet4
./CompareMethods -c ElectronSignal -m 800  -o gluonconstrain  -x ChiSq6jet2b ChiSq6jet2bgjet2 ChiSq8jet2bgjet4
./CompareMethods -c ElectronSignal -m 1200 -o gluonconstrain  -x ChiSq6jet2b ChiSq6jet2bgjet2 ChiSq8jet2bgjet4
./CompareMethods -c ElectronSignal -m 1600 -o gluonconstrain  -x ChiSq6jet2b ChiSq6jet2bgjet2 ChiSq8jet2bgjet4

./CompareMethods -c MuonSignal     -m 800  -o masswidthcomp     -x ChiSq6jet2b ChiSq6jet2bwidetstar
./CompareMethods -c MuonSignal     -m 1200 -o masswidthcomp     -x ChiSq6jet2b ChiSq6jet2bwidetstar
./CompareMethods -c MuonSignal     -m 1600 -o masswidthcomp     -x ChiSq6jet2b ChiSq6jet2bwidetstar
./CompareMethods -c ElectronSignal -m 800  -o masswidthcomp     -x ChiSq6jet2b ChiSq6jet2bwidetstar
./CompareMethods -c ElectronSignal -m 1200 -o masswidthcomp     -x ChiSq6jet2b ChiSq6jet2bwidetstar
./CompareMethods -c ElectronSignal -m 1600 -o masswidthcomp     -x ChiSq6jet2b ChiSq6jet2bwidetstar

./CompareMethods -c MuonSignal     -m 800  -o ressourcecomp  -x ChiSq6jet2bpdgres ChiSq6jet2bmcres ChiSq6jet2bmcrestopshift
./CompareMethods -c MuonSignal     -m 1200 -o ressourcecomp  -x ChiSq6jet2bpdgres ChiSq6jet2bmcres ChiSq6jet2bmcrestopshift
./CompareMethods -c MuonSignal     -m 1600 -o ressourcecomp  -x ChiSq6jet2bpdgres ChiSq6jet2bmcres ChiSq6jet2bmcrestopshift
./CompareMethods -c ElectronSignal -m 800  -o ressourcecomp  -x ChiSq6jet2bpdgres ChiSq6jet2bmcres ChiSq6jet2bmcrestopshift
./CompareMethods -c ElectronSignal -m 1200 -o ressourcecomp  -x ChiSq6jet2bpdgres ChiSq6jet2bmcres ChiSq6jet2bmcrestopshift
./CompareMethods -c ElectronSignal -m 1600 -o ressourcecomp  -x ChiSq6jet2bpdgres ChiSq6jet2bmcres ChiSq6jet2bmcrestopshift

./CompareMethods -c MuonSignal     -m 800  -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c MuonSignal     -m 1200 -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c MuonSignal     -m 1600 -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c ElectronSignal -m 800  -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c ElectronSignal -m 1200 -o chisqstd     -x ChiSq6jet2b
./CompareMethods -c ElectronSignal -m 1600 -o chisqstd     -x ChiSq6jet2b

./CompareKeys -c MuonSignal -m 700 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 800 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 900 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 1000 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 1100 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 1200 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 1300 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 1400 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 1500 -x ChiSq6jet2b
./CompareKeys -c MuonSignal -m 1600 -x ChiSq6jet2b

./CompareKeys -c ElectronSignal -m 700 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 800 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 900 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 1000 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 1100 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 1200 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 1300 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 1400 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 1500 -x ChiSq6jet2b
./CompareKeys -c ElectronSignal -m 1600 -x ChiSq6jet2b

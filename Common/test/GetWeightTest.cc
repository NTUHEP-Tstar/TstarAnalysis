/*******************************************************************************
*
*  Filename    : GetWeightTest.cc
*  Description : Unit testing for weight getting functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"

#include "TFile.h"

#include <iostream>
using namespace std;

int
main( int argc, char const* argv[] )
{
  fwlite::Event ev(
    TFile::Open(
      "root://eoscms.cern.ch//store/user/yichen/tstar_store/massreco/ElectronSignal/TstarTstarToTgluonTgluon_M-1600_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root" ) );

  for( ev.toBegin(); !ev.atEnd(); ++ev ){
    cout << "Electron RECO Weight:" << GetElectronTrackWeight( ev )  << endl;
    cout << "Electron CUT  Weight:" << GetElectronCutWeight( ev )  << endl;
    cout << "Electron trigger weight" << GetElectronTriggerWeight( ev ) << endl;
  }

  return 0;
}

/*******************************************************************************
 *
 *  Filename    : readweights.cc
 *  Description : Testing the read weights functions for output files
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "TstarAnalysis/EventWeight/interface/GetEventWeight.hpp"

#include <iostream>
#include "TFile.h"

using namespace std;

int main(int argc, char const *argv[]) {
   fwlite::Event ev( TFile::Open("tstarMassReco.root") );
   for( ev.toBegin() ; !ev.atEnd() ; ++ev ){
      cout << "\nTotal event weight:" <<  GetEventWeight( ev ) << endl;
      cout << "\tPileup Weight:" << GetPileupWeight( ev ) << endl;
      cout << "\tElectron Weight:" << GetElectronWeight( ev ) << endl;
      cout << "\tbtag weight:" << GetBtagWeight( ev ) << endl;
      cout << "\tbtag weight (up):" << GetBtagWeightUp( ev ) << endl;
      cout << "\tbtag weight (down):" << GetBtagWeightDown( ev ) << endl;   
   }
   return 0;
}

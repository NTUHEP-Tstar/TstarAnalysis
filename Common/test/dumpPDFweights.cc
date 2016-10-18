/*******************************************************************************
*
*  Filename    : dumpPDFweights.cc
*  Description : Dumping information for PDF weighting
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Main recipe:
*  https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatGeneratorInterface
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include <iostream>
#include <map>

#include "TFile.h"
using namespace std;

int
main( int argc, char const* argv[] )
{

   fwlite::ChainEvent ev({
      "root://eoscms.cern.ch//store/user/yichen/tstar_store/tstarbaseline/Muon/TT_TuneCUETP8M1_13TeV-powheg-pythia8_0.root",
      "root://eoscms.cern.ch//store/user/yichen/tstar_store/tstarbaseline/Muon/TT_TuneCUETP8M1_13TeV-powheg-pythia8_1.root"
   });
   fwlite::Handle<LHEEventProduct> evtinfo;

   double originalXWGTUP = 0;
   double firstweight    = 0;
   map<int, double> weightmap;


   // Initializing everything
   ev.toBegin();
   evtinfo.getByLabel( ev, "externalLHEProducer" );
   for( const auto& weightinfo : evtinfo.ref().weights() ){
      weightmap[stoi(weightinfo.id)] = 0;
   }

   // Looping to get average
   for( ev.toBegin(); !ev.atEnd(); ++ev ){
      evtinfo.getByLabel( ev, "externalLHEProducer" );

      originalXWGTUP += fabs(evtinfo.ref().originalXWGTUP());
      firstweight    += fabs(evtinfo.ref().weights().front().wgt);

      for( const auto& weightinfo : evtinfo.ref().weights() ){
         weightmap[stoi(weightinfo.id)] += fabs(weightinfo.wgt);
      }
   }
   originalXWGTUP /= ev.size();
   firstweight    /= ev.size();

   // Printing information to cout
   cout << "originalXWGTUP avg value: " << originalXWGTUP << endl;
   cout << "first weight avg value:   " << firstweight    << endl;
   for( auto& mappair : weightmap ){
      mappair.second /= ev.size();
      cout << mappair.first << " "
           << mappair.second << " ("
           << mappair.second / originalXWGTUP << ", "
           << mappair.second / firstweight << ")" << endl;
   }
   return 0;
}

/*******************************************************************************
*
*  Filename    : DumpRunGenInfo.cc
*  Description : Dumping information for PDF weighting
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Main recipe:
*  https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatGeneratorInterface
*  https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatGeneratorInterface
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "TFile.h"

#include <iostream>
#include <map>
using namespace std;

int
main( int argc, char const* argv[] )
{

   fwlite::Run irun( TFile::Open( argv[1] ) );
   fwlite::Handle<LHERunInfoProduct> runinfo;

   runinfo.getByLabel( irun, "externalLHEProducer" );

   for( auto info = runinfo->headers_begin(); info != runinfo->headers_end(); ++info ){
      if( info->tag() == "initrwgt" ){
         for( const auto& line : info->lines() ){
            cout << line;
         }
      }
   }

   fwlite::Event ev( TFile::Open( argv[1] ) );
   fwlite::Handle<LHEEventProduct> evtinfo;

   evtinfo.getByLabel( ev, "externalLHEProducer" );
   map<string, double> weightmap;

   ev.toBegin();

   for( const auto& weightinfo : evtinfo.ref().weights() ){
      weightmap[weightinfo.id] = 0;
   }

   for( ev.toBegin(); !ev.atEnd(); ++ev ){
      const double mainweight = evtinfo.ref().originalXWGTUP();

      for( const auto& weightinfo : evtinfo.ref().weights() ){
         weightmap[weightinfo.id] += weightinfo.wgt / mainweight;
      }
   }

   for( auto& mappair : weightmap ){
      mappair.second /= ev.size();
      cout << mappair.first << " " << mappair.second << endl;
   }

   return 0;
}

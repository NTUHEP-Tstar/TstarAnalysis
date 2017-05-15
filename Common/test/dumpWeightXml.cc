/*******************************************************************************
*
*  Filename    : dumpWeightXML.cc
*  Description : Dumping XML information stored per run
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Main recipe:
*  https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatGeneratorInterface
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include <iostream>
#include <map>

#include "TFile.h"

using namespace std;

int
main( int argc, char const* argv[] )
{

   fwlite::Run irun( TFile::Open( argv[1] ) );
   fwlite::Handle<LHERunInfoProduct> runinfo;

   runinfo.getByLabel( irun, "externalLHEProducer" );

   // C++11 style loop not available :(
   for( auto info = runinfo->headers_begin(); info != runinfo->headers_end(); ++info ){
      if( info->tag() == "initrwgt" ){
         for( const auto& line : info->lines() ){
            cout << line;
         }
      }
   }

   return 0;
}

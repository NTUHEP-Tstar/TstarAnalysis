/*******************************************************************************
*
*  Filename    : pdfweight_test.cc
*  Description : Unit testing for PDF weighs
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Handle.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "TFile.h"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include <iostream>

using namespace std;
int
main( int argc, char const* argv[] )
{

   const auto& pdfidgroup = GetPdfIdGrouping( argv[1] );

   unsigned i = 0;

   for( const auto& idgroup : pdfidgroup ){
      cout << "Group " << i << endl;

      for( const auto& id : idgroup ){
         cout << "\t" << id;
      }

      cout << endl;
      ++i;
   }

   fwlite::Event ev( TFile::Open( argv[1] ) );
   fwlite::Handle<LHEEventProduct> evthandle;
   evthandle.getByLabel( ev, "externalLHEProducer" );


   for( ev.toBegin(); !ev.atEnd(); ++ev ){
      cout << GetPdfWeightError( ev, pdfidgroup ) << " " << GetScaleWeightError( ev, pdfidgroup ) << endl;
   }

   return 0;
}

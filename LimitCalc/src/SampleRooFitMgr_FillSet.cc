/*******************************************************************************
 *
 *  Filename    : SampleRooFitMgr_FillSet.cc
 *  Description : Main control flow for filling in the datasets
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"
#include "TstarAnalysis/EventWeight/interface/GetEventWeight.hpp"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "DataFormats/FWLite/interface/Handle.h"

#include <cstdio>

void SampleRooFitMgr::definesets()
{
   NewDataSet(""); // Default dataset
}

void SampleRooFitMgr::fillsets( mgr::SampleMgr& sample )
{
   fwlite::Handle<RecoResult>  chiHandle;

   // Getting the total weight of the objects
   double weight_sum = 0.;
   double weight     = 1. ;
   for( sample.Event().toBegin() ; !sample.Event().atEnd() ; ++sample.Event() ){
      weight_sum += GetEventWeight( sample.Event() ) ;
   }

   const double expyield = sample.ExpectedYield().CentralValue();
   const double scale    = expyield / weight_sum ;

   unsigned i = 0 ;
   for( sample.Event().toBegin() ; !sample.Event().atEnd() ; ++sample.Event(), ++i ){
      printf( "\rSample [%s|%s], Event[%6u/%6llu]..." ,
         Name().c_str(),
         sample.Name().c_str(),
         i ,
         sample.Event().size() );
      fflush(stdout);

      chiHandle.getByLabel( sample.Event() , "tstarMassReco" , "ChiSquareResult" , "TstarMassReco" );
      weight = GetEventWeight( sample.Event() ) * scale ;

      // Getting event weight
      if( chiHandle->ChiSquare() < 0 ){ continue; } // Skipping over unphysical results

      double tstarMass = chiHandle->TstarMass() ;
      if( MinMass() <=tstarMass && tstarMass <= MaxMass()  &&
          -1000.    <= weight   && weight    <= 1000. ){
         x() = tstarMass;
         DataSet()->add( RooArgSet(x()) , weight );

      }
   }
   printf( "Done!\n"); fflush( stdout );
}

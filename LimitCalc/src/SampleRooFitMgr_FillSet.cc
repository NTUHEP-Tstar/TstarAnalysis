/*******************************************************************************
 *
 *  Filename    : SampleRooFitMgr_FillSet.cc
 *  Description : Main control flow for filling in the datasets
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "DataFormats/FWLite/interface/Handle.h"

#include <cstdio>

void SampleRooFitMgr::definesets()
{
   NewDataSet(""); // Default dataset
}

void SampleRooFitMgr::fillsets( mgr::SampleMgr& sample )
{
   fwlite::Handle<LHEEventProduct>  lheHandle;
   fwlite::Handle<RecoResult>  chiHandle;
   double sample_weight =  1. ;

   if( !sample.IsRealData() ){
      sample_weight = sample.GetSampleWeight();
   }

   unsigned i = 0 ;
   for( sample.Event().toBegin() ; !sample.Event().atEnd() ; ++sample.Event() ){
      printf( "\rSample [%s|%s], Event[%6u/%6llu]..." ,
         Name().c_str(),
         sample.Name().c_str(),
         ++i ,
         sample.Event().size() );
      fflush(stdout);

      chiHandle.getByLabel( sample.Event() , "tstarMassReco" , "ChiSquareResult" , "TstarMassReco" );
      if( !sample.IsRealData() ){
         lheHandle.getByLabel( sample.Event() , "externalLHEProducer" );
      }

      if( chiHandle->ChiSquare() < 0 ){ continue; } // Skipping over unphysical results

      double tstarMass = chiHandle->TstarMass() ;
      double event_weight = 1.0 ;
      if( lheHandle.isValid() && lheHandle->hepeup().XWGTUP <= 0 ) { event_weight = -1.; }
      double weight = event_weight * sample_weight ;
      if( MinMass() <=tstarMass && tstarMass <= MaxMass()  &&
          -1000.    <= weight   && weight    <= 1000. ){
         x() = tstarMass;
         DataSet()->add( RooArgSet(x()) , weight );
      }
   }
   printf( "Done!\n"); fflush( stdout );
}

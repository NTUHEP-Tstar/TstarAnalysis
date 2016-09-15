/*******************************************************************************
*
*  Filename    : SampleRooFitMgr_FillSet.cc
*  Description : Main control flow for filling in the datasets
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/Efficiency.hpp"
#include "TstarAnalysis/EventWeight/interface/ComputeSelectionEff.hpp"
#include "TstarAnalysis/EventWeight/interface/GetEventWeight.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "DataFormats/FWLite/interface/Handle.h"

#include <cstdio>
#include <string>

void
SampleRooFitMgr::definesets()
{
   NewDataSet( "" );// Default dataset
   NewDataSet( "jecup" );
   NewDataSet( "jecdown" );

   // Additional shapes unique to Monte Carlo data set
   if( Name().find( "Data" ) == std::string::npos ){
      NewDataSet( "jetresup" );
      NewDataSet( "jetresdown" );
      NewDataSet( "btagup" );
      NewDataSet( "btagdown" );
   }
}

void
SampleRooFitMgr::fillsets( mgr::SampleMgr& sample )
{
   fwlite::Handle<RecoResult> chiHandle;

   const double selectedEvents = GetSelectedEventCount( sample );
   const double exp_yield      = sample.ExpectedYield().CentralValue();
   const double sampleweight   = exp_yield / selectedEvents;

   unsigned i = 1;

   for( sample.Event().toBegin(); !sample.Event().atEnd(); ++sample.Event(), ++i ){

      printf( "\rSample [%s|%s], Event[%6u/%6llu]...",
         Name().c_str(),
         sample.Name().c_str(),
         i,
         sample.Event().size() );
      fflush( stdout );

      const double weight = GetEventWeight( sample.Event() ) * sampleweight;

      // Getting event weight
      chiHandle.getByLabel( sample.Event(), "tstarMassReco", "ChiSquareResult", "TstarMassReco" );
      if( chiHandle->ChiSquare() < 0 ){ continue; }// Skipping over unphysical results

      double tstarMass = chiHandle->TstarMass();

      if( MinMass() <= tstarMass && tstarMass <= MaxMass()  &&
          -1000.    <= weight   && weight    <= 1000. ){
         x() = tstarMass;
         DataSet( "" )->add( RooArgSet( x() ), weight );

         x() = chiHandle->ComputeFromPaticleList( tstar::corr_up );
         DataSet( "jecup" )->add( RooArgSet( x() ), weight );

         x() = chiHandle->ComputeFromPaticleList( tstar::corr_down );
         DataSet( "jecdown" )->add( RooArgSet( x() ), weight );

         // Dataset unique to Monte-Carlo dataset
         if( Name().find( "Data" ) == std::string::npos ){
            x() = chiHandle->ComputeFromPaticleList( tstar::res_up );
            DataSet( "jetresup" )->add( RooArgSet( x() ), weight );

            x() = chiHandle->ComputeFromPaticleList( tstar::res_down );
            DataSet( "jetresdown" )->add( RooArgSet( x() ), weight );

            x() = tstarMass;
            const double btagweight     = GetBtagWeight( sample.Event() );
            const double btagweightup   = GetBtagWeightUp( sample.Event() );
            const double btagweightdown = GetBtagWeightDown( sample.Event() );
            DataSet( "btagup"  )->add( RooArgSet( x() ), weight * btagweightup   / btagweight );
            DataSet( "btagdown" )->add( RooArgSet( x() ), weight * btagweightdown / btagweight );
         }
      }
   }

   printf( "Done!\n" );
   fflush( stdout );
}

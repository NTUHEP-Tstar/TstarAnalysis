/*******************************************************************************
*
*  Filename    : SampleRooFitMgr_FillSet.cc
*  Description : Main control flow for filling in the datasets
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/ComputeSelectionEff.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
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
      NewDataSet( "puup" );
      NewDataSet( "pudown" );
      NewDataSet( "elecup" );
      NewDataSet( "elecdown" );
   }
}

void
SampleRooFitMgr::fillsets( mgr::SampleMgr& sample )
{
   fwlite::Handle<RecoResult> chiHandle;

   const double sampleweight = GetSampleWeight( sample );

   unsigned i = 1;

   for( sample.Event().toBegin(); !sample.Event().atEnd(); ++sample.Event(), ++i ){

      printf( "\rSample [%s|%s], Event[%6u/%6llu]...",
         Name().c_str(),
         sample.Name().c_str(),
         i,
         sample.Event().size() );
      fflush( stdout );

      const double weight = GetEventWeight( sample.Event() ) * sampleweight * GetSampleEventTopPtWeight( sample , sample.Event() );

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
            const double puweight       = GetPileupWeight( sample.Event() );
            const double puweightup     = GetPileupWeight71260 ( sample.Event() );
            const double puweightdown   = GetPileupWeight62000 ( sample.Event() );
            const double elecweight     = GetElectronWeight    ( sample.Event() );
            const double elecweightup   = GetElectronWeightUp  ( sample.Event() );
            const double elecweightdown = GetElectronWeightDown( sample.Event() );
            DataSet( "btagup"  )->add( RooArgSet( x() ), weight * btagweightup   / btagweight );
            DataSet( "btagdown" )->add( RooArgSet( x() ), weight * btagweightdown / btagweight );
            DataSet( "puup" )->add( RooArgSet( x() ), weight * puweightup / puweight );
            DataSet( "pudown" )->add( RooArgSet( x() ), weight * puweightdown / puweight );
            DataSet( "elecup" )->add( RooArgSet( x() ), weight * elecweightup / elecweight );
            DataSet( "elecdown" )->add( RooArgSet( x() ), weight * elecweightdown / elecweight );
         }
      }
   }

   printf( "Done!\n" );
   fflush( stdout );
}

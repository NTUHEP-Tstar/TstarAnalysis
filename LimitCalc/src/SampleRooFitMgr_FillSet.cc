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

#include "ManagerUtils/Maths/interface/Efficiency.hpp"

#include "DataFormats/FWLite/interface/Handle.h"

#include <cstdio>
#include <string>

void
SampleRooFitMgr::definesets()
{
   NewDataSet( "" );// Default dataset

   // Additional shapes unique to Monte Carlo data set
   if( Name().find( "Data" ) == std::string::npos ){
      NewDataSet( "jecup" );
      NewDataSet( "jecdown" );
      NewDataSet( "jetresup" );
      NewDataSet( "jetresdown" );
      NewDataSet( "btagup" );
      NewDataSet( "btagdown" );
      NewDataSet( "puup" );
      NewDataSet( "pudown" );
      NewDataSet( "lepup" );
      NewDataSet( "lepdown" );
      NewDataSet( "pdfup" );
      NewDataSet( "pdfdown" );
      NewDataSet( "scaleup" );
      NewDataSet( "scaledown" );
   }
}

void
SampleRooFitMgr::fillsets( mgr::SampleMgr& sample )
{
   fwlite::Handle<RecoResult> chiHandle;

   const double sampleweight = GetSampleWeight( sample );
   const auto& pdfidgroup = GetPdfIdGrouping( sample );

   unsigned i = 1;

   for( sample.Event().toBegin(); !sample.Event().atEnd(); ++sample.Event(), ++i ){

      printf( "\rSample [%s|%s], Event[%6u/%6llu]...",
         Name().c_str(),
         sample.Name().c_str(),
         i,
         sample.Event().size() );
      fflush( stdout );

      const double weight
         = GetEventWeight( sample.Event() )
           * sampleweight
           * GetSampleEventTopPtWeight( sample, sample.Event() );

      // Getting event weight
      chiHandle.getByLabel( sample.Event(), "tstarMassReco", "ChiSquareResult", "TstarMassReco" );
      if( chiHandle->ChiSquare() < 0 ){ continue; }// Skipping over unphysical results

      // Points to insert for all mass data types
      const double tstarmass = chiHandle->TstarMass();
      AddToDataSet( "", tstarmass, weight );

      // Masses to insert for MC sample
      if( Name().find( "Data" ) == std::string::npos ){
         const double tstarmass_jecup   = chiHandle->ComputeFromPaticleList( tstar::corr_up  );
         const double tstarmass_jecdown = chiHandle->ComputeFromPaticleList( tstar::corr_down );
         const double tstarmass_jerup   = chiHandle->ComputeFromPaticleList( tstar::res_up );
         const double tstarmass_jerdown = chiHandle->ComputeFromPaticleList( tstar::res_down );
         AddToDataSet( "jetresup",   tstarmass_jerup,   weight );
         AddToDataSet( "jetresdown", tstarmass_jerdown, weight );
         AddToDataSet( "jecup",      tstarmass_jecup,   weight );
         AddToDataSet( "jecdown",    tstarmass_jecdown, weight );

         const double btagweight      = GetBtagWeight( sample.Event() );
         const double btagweightup    = GetBtagWeightUp( sample.Event() );
         const double btagweightdown  = GetBtagWeightDown( sample.Event() );
         const double puweight        = GetPileupWeight( sample.Event() );
         const double puweightup      = GetPileupWeightXsecup( sample.Event() );
         const double puweightdown    = GetPileupWeightXsecdown( sample.Event() );
         const double elecweight      = GetElectronWeight( sample.Event() );
         const double elecweightup    = GetElectronWeightUp( sample.Event() );
         const double elecweightdown  = GetElectronWeightDown( sample.Event() );
         const double muonweight      = GetMuonWeight( sample.Event() );
         const double muonweightup    = GetMuonWeightUp( sample.Event() );
         const double muonweightdown  = GetMuonWeightDown( sample.Event() );
         const double pdfweightup     = 1 + GetPdfWeightError( sample.Event(), pdfidgroup );
         const double pdfweightdown   = 1 - GetPdfWeightError( sample.Event(), pdfidgroup );
         const double scaleweightup   = 1 + GetScaleWeightError( sample.Event(), pdfidgroup );
         const double scaleweightdown = 1 - GetScaleWeightError( sample.Event(), pdfidgroup );

         AddToDataSet( "btagup",    tstarmass, weight * btagweightup   / btagweight );
         AddToDataSet( "btagdown",  tstarmass, weight * btagweightdown / btagweight );
         AddToDataSet( "puup",      tstarmass, weight * puweightup / puweight );
         AddToDataSet( "pudown",    tstarmass, weight * puweightdown / puweight );
         AddToDataSet( "lepup",     tstarmass, weight * ( elecweightup/elecweight )   * ( muonweightup/muonweight ) );
         AddToDataSet( "lepdown",   tstarmass, weight * ( elecweightdown/elecweight ) * ( muonweightdown/muonweight ) );
         AddToDataSet( "pdfup",     tstarmass, weight * pdfweightup );
         AddToDataSet( "pdfdown",   tstarmass, weight * pdfweightdown );
         AddToDataSet( "scaleup",   tstarmass, weight * scaleweightup );
         AddToDataSet( "scaledown", tstarmass, weight * scaleweightdown );
      }
   }

   // Recalculating selection efficiency based on number of events pushed to central dataset
   const Parameter newseleceff = Efficiency( DataSet( "" )->sumEntries(), GetOriginalEventCount( sample ) );
   sample.SetSelectionEfficiency( newseleceff );

   printf( "Done!\n" );
   fflush( stdout );
}


void
SampleRooFitMgr::AddToDataSet( const std::string& datasetname, const double mass, const double weight )
{
   if( mass < MaxMass() && mass > MinMass() && weight > -1000. && weight < 1000. ){
      x() = mass;
      DataSet( datasetname )->add( RooArgSet( x() ), weight );
   }
}

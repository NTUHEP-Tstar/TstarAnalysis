/*******************************************************************************
*
*  Filename    : SampleRooFitMgr_FillSet.cc
*  Description : Main control flow for filling in the datasets
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "ManagerUtils/Maths/interface/Efficiency.hpp"
#include "ManagerUtils/SampleMgr/interface/MultiFile.hpp"

#include "DataFormats/FWLite/interface/Handle.h"

#include "RooDataSet.h"
#include <cstdio>
#include <iostream>
#include <string>

using namespace std;

/******************************************************************************/

void
SampleRooFitMgr::definesets()
{
   NewDataSet( "" );// Default dataset

   // Additional shapes unique to Monte Carlo data set
   if( Name().find( "Data" ) == std::string::npos ){
      for( const auto& source : uncsource ){
         NewDataSet( source + "Up" );
         NewDataSet( source + "Down" );
      }
   }
}

/******************************************************************************/

void
SampleRooFitMgr::fillsets( mgr::SampleMgr& sample )
{
   fwlite::Handle<RecoResult> chiHandle;

   const double sampleweight = sample.IsRealData()?
      1:mgr::SampleMgr::TotalLuminosity() * sample.CrossSection() / sample.OriginalEventCount();
   const auto& pdfidgroup    = GetPdfIdGrouping( sample );

   unsigned i = 1;

   mgr::MultiFileEvent myevt( sample.GlobbedFileList() );

   for( myevt.toBegin(); !myevt.atEnd(); ++myevt, ++i ){
      const auto& ev = myevt.Base();

      printf( "\rSample [%s|%s], Event[%6u/%6u]...",
         Name().c_str(),
         sample.Name().c_str(),
         i,
         myevt.size() );
      fflush( stdout );

      const double weight
         = GetEventWeight( ev )
           * sampleweight
           * GetSampleEventTopPtWeight( sample, ev );

      // Getting event weight
      chiHandle.getByLabel( ev, "tstarMassReco", "ChiSquareResult", "TstarMassReco" );
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
         AddToDataSet( "jetresUp",   tstarmass_jerup,   weight );
         AddToDataSet( "jetresDown", tstarmass_jerdown, weight );
         AddToDataSet( "jecUp",      tstarmass_jecup,   weight );
         AddToDataSet( "jecDown",    tstarmass_jecdown, weight );

         const double btagweight      = GetBtagWeight( ev );
         const double btagweightup    = GetBtagWeightUp( ev );
         const double btagweightdown  = GetBtagWeightDown( ev  );
         const double puweight        = GetPileupWeight( ev );
         const double puweightup      = GetPileupWeightXsecup( ev );
         const double puweightdown    = GetPileupWeightXsecdown( ev );
         const double elecweight      = GetElectronWeight( ev );
         const double elecweightup    = GetElectronWeightUp( ev );
         const double elecweightdown  = GetElectronWeightDown( ev );
         const double muonweight      = GetMuonWeight( ev );
         const double muonweightup    = GetMuonWeightUp( ev );
         const double muonweightdown  = GetMuonWeightDown( ev );
         const double pdfweightup     = 1 + GetPdfWeightError( ev, pdfidgroup );
         const double pdfweightdown   = 1 - GetPdfWeightError( ev, pdfidgroup );
         const double scaleweightup   = 1 + GetScaleWeightError( ev, pdfidgroup );
         const double scaleweightdown = 1 - GetScaleWeightError( ev, pdfidgroup );

         AddToDataSet( "btagUp",    tstarmass, weight * btagweightup   / btagweight );
         AddToDataSet( "btagDown",  tstarmass, weight * btagweightdown / btagweight );
         AddToDataSet( "puUp",      tstarmass, weight * puweightup / puweight );
         AddToDataSet( "puDown",    tstarmass, weight * puweightdown / puweight );
         AddToDataSet( "lepUp",     tstarmass, weight * ( elecweightup/elecweight )   * ( muonweightup/muonweight ) );
         AddToDataSet( "lepDown",   tstarmass, weight * ( elecweightdown/elecweight ) * ( muonweightdown/muonweight ) );
         AddToDataSet( "pdfUp",     tstarmass, weight * pdfweightup );
         AddToDataSet( "pdfDown",   tstarmass, weight * pdfweightdown );
         AddToDataSet( "scaleUp",   tstarmass, weight * scaleweightup );
         AddToDataSet( "scaleDown", tstarmass, weight * scaleweightdown );
      }
   }

   // Recalculating selection efficiency based on number of events pushed to central dataset
   sample.SetSelectedEventCount( DataSet( "" )->sumEntries() );

   printf( "Done!\n" );
   fflush( stdout );
}

/******************************************************************************/

void
SampleRooFitMgr::AddToDataSet( const std::string& datasetname, const double mass, const double weight )
{
   if( mass < MaxMass() && mass > MinMass() && weight > -1000. && weight < 1000. ){
      x() = mass;
      DataSet( datasetname )->add( RooArgSet( x() ), weight );
   }
}

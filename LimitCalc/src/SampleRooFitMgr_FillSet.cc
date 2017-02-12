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

#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedVars.hpp"
#include "ManagerUtils/SampleMgr/interface/MultiFile.hpp"

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include <boost/format.hpp>
#include <cstdio>
#include <iostream>
#include <string>

using namespace std;
using namespace mgr;

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
  fwlite::Handle<vector<pat::Muon> > muonHandle;

  const double sampleweight =
    sample.IsRealData() ?  1.0 :
    mgr::SampleMgr::TotalLuminosity() * sample.CrossSection().CentralValue() / sample.OriginalEventCount();

  const auto& pdfidgroup = GetPdfIdGrouping( sample );

  unsigned i = 1;

  mgr::MultiFileEvent myevt( sample.GlobbedFileList() );

  boost::format statusline( "\rSample [%s|%s], Event[%6u/%6u] (%lf %lf %lf)..." );

  for( myevt.toBegin(); !myevt.atEnd(); ++myevt, ++i ){
    const auto& ev = myevt.Base();

   

    muonHandle.getByLabel( ev, "skimmedPatMuons");
    if(limnamer.CheckInput("mucut")){
       if( (*muonHandle)[0].pt() < limnamer.GetInput<double>("mucut") )
           continue;
    }
    
    
    const double weight
      = GetEventWeight( ev )
        * sampleweight
        * GetSampleEventTopPtWeight( sample, ev );

    cout << statusline
      % Name()
      % sample.Name()
      % i
      % myevt.size()
      % GetEventWeight( ev )
      % sampleweight
      % GetSampleEventTopPtWeight( sample, ev )
         << flush;

    // Getting mass reconstruction results
    chiHandle.getByLabel( ev, "tstarMassReco", "ChiSquareResult", "TstarMassReco" );
    if( chiHandle->ChiSquare() < 0 ){ continue; }// Skipping over unphysical results

    // Points to insert for all mass data types
    const double tstarmass = chiHandle->TstarMass();
    if(limnamer.CheckInput("masscut")){
        if(tstarmass < limnamer.GetInput<double>("masscut"))
            continue;
    }


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
      AddToDataSet( "modelUp",   tstarmass, weight );
      AddToDataSet( "modelDown", tstarmass, weight );
    }
  }

  // Recalculating selection efficiency based on number of events pushed to central dataset
  sample.SetSelectedEventCount( DataSet( "" )->sumEntries() );

  cout << "Done!" << endl;
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

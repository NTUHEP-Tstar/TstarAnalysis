/*******************************************************************************
*
*  Filename    : SampleTableMgr.cc
*  Description : Implementations of SampleTableMgr class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Main objective: store the total error in weighting errors
*
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleTableMgr.hpp"

#include "ManagerUtils/SampleMgr/interface/MultiFile.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgrLoader.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include <boost/format.hpp>
#include <iostream>

using namespace std;
using namespace mgr;

/*******************************************************************************
*   Constructor/Destructor and related functions
*******************************************************************************/
SampleTableMgr::SampleTableMgr( const string& x, const ConfigReader& y ) :
  Named( x ),
  SampleGroup( x, y )
{
  // Caching addtional strings stored in json file
  LoadExtraString();
}

SampleTableMgr::~SampleTableMgr()
{}

/******************************************************************************/

void
SampleTableMgr::LoadExtraString()
{
  const mgr::ConfigReader& cfg = compnamer.MasterConfig();

  auto type = GetType( cfg );
  if( type == Undef ){
    LoadExtraString( Sample(), GetUndefConfig( cfg ) );

  } else if( type == Standard ){
    const auto samplecfg = GetSampleListConfig( cfg );

    for( auto& sample : SampleList() ){
      LoadExtraString( sample, samplecfg );
    }
  } else if( type == FileList ){
    for( const auto& samplecfg : GetConfigList( cfg ) ){
      for( const auto& sampletag : samplecfg.GetInstanceList() ){
        LoadExtraString( Sample( sampletag ), samplecfg );
      }
    }
  } else if( type == Single ){
    const auto samplecfg = GetSingleConfig( cfg );
    LoadExtraString( Sample(), samplecfg );
  }
}

/******************************************************************************/

void
SampleTableMgr::LoadExtraString( mgr::SampleMgr& sample, const mgr::ConfigReader& cfg )
{
  if( cfg.HasTag( sample.Name(), "Generator" ) ){
    sample.AddCacheString( "Generator", cfg.GetString( sample.Name(), "Generator" ) );
  }
  if( cfg.HasTag( sample.Name(), "Cross Section Source" ) ){
    sample.AddCacheString( "XsecSource", cfg.GetString( sample.Name(), "Cross Section Source" ) );
  }
}


/*******************************************************************************
*   Saving/Loading from text files
*******************************************************************************/
void
SampleTableMgr::LoadFromEDM()
{
  for( auto& sample : SampleList() ){
    InitSampleFromEDM( sample );  // common varaibles, see Common/InitSample.cc
    LoadFromEDM( sample );  // Self defined numbers, see below
    SaveCacheToFile( sample, SampleCacheFile( sample ) );
  }
}

void
SampleTableMgr::LoadFromFile()
{
  for( auto& sample : SampleList() ){
    LoadCacheFromFile( sample, SampleCacheFile( sample ) );
  }
}


/*******************************************************************************
*   EDM file interactions
*******************************************************************************/
void
SampleTableMgr::LoadFromEDM( mgr::SampleMgr& sample )
{
  double eventweightsum     = 0;
  double eventweightupsum   = 0;
  double eventweightdownsum = 0;

  double btagweightsum   = 0;
  double puweightsum     = 0;
  double leptonweightsum = 0;
  double topptweightsum  = 0;
  double pdfweightsum    = 0;


  const auto& pdfidgroup = GetPdfIdGrouping( sample );

  fwlite::Handle<std::vector<pat::Jet> > jethandle;

  // counter asdf
  unsigned evtcounter = 1;
  mgr::MultiFileEvent myevt( sample.GlobbedFileList() );

  boost::format progressbar( "\rSample %s Event: %u/%u ...(%lf)" );
  cout << progressbar % sample.Name()% evtcounter% myevt.size() % 1  << flush;

  for( myevt.toBegin(); !myevt.atEnd(); ++myevt, ++evtcounter ){
    const auto& ev = myevt.Base();

    const double btagweight     = GetBtagWeight( ev );
    const double btagweightup   = GetBtagWeightUp( ev );
    const double btagweightdown = GetBtagWeightDown( ev );

    const double puweight     = GetPileupWeight( ev );
    const double puweightup   = GetPileupWeightXsecup( ev );
    const double puweightdown = GetPileupWeightXsecdown( ev );

    const double leptonweight     = GetElectronWeight( ev ) * GetMuonWeight( ev );
    const double leptonweightup   = GetElectronWeightUp( ev ) * GetMuonWeightUp( ev );
    const double leptonweightdown = GetElectronWeightDown( ev ) * GetMuonWeightDown( ev );

    const double topptweight    = GetSampleEventTopPtWeight( sample, ev );
    const double pdfweighterr   = GetPdfWeightError( ev, pdfidgroup );
    const double scaleweighterr = GetScaleWeightError( ev, pdfidgroup );

    cout << progressbar
      % ( sample.Name() )
      % ( evtcounter )
      % ( myevt.size() )
      % ( leptonweight*btagweight*puweight/GetEventWeight( ev ) )
         << flush;

    const double sign        = GetEventWeight( ev ) > 0 ? 1 : -1;
    const double eventweight = GetEventWeight( ev ) * topptweight;

    const double eventweightup = btagweightup
                                 * puweightup
                                 * leptonweightup
                                 * topptweight
                                 * ( 1 + pdfweighterr )
                                 * ( 1 + scaleweighterr )
                                 * sign;
    const double eventweightdown = btagweightdown
                                   * puweightdown
                                   * leptonweightdown
                                   * topptweight
                                   * ( 1 - pdfweighterr )
                                   * ( 1 - scaleweighterr )
                                   * sign;

    eventweightsum     += eventweight;
    eventweightupsum   += eventweightup;
    eventweightdownsum += eventweightdown;

    btagweightsum   += btagweight;
    puweightsum     += puweight;
    leptonweightsum += leptonweight;
    topptweightsum  += topptweight;
    pdfweightsum    += 1 + pdfweighterr;
  }

  evtcounter--;
  cout << boost::format( "Done! AVG weight: btag[%lf], pu[%lf], lepton[%lf] , toppt[%lf],  pdf[%lf]" )
    % ( btagweightsum / evtcounter )
    % ( puweightsum / evtcounter )
    % ( leptonweightsum / evtcounter )
    % ( topptweightsum / evtcounter )
    % ( pdfweightsum / evtcounter )
       << endl;

  sample.AddCacheDouble( "EventWeightSum",     eventweightsum );
  sample.AddCacheDouble( "EventWeightUpSum",   eventweightupsum );
  sample.AddCacheDouble( "EventWeightDownSum", eventweightdownsum );
}

/*******************************************************************************
*
*  Filename    : FullCompare.cc
*  Description : Full kinematic compare with errors represented
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeErrHist.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"

#include <iostream>
using namespace std;

/*******************************************************************************
*   Main control flow
*******************************************************************************/
int
main( int argc, char* argv[] )
{
  compnamer.AddOptions( CompareOptions() ); //defined in src/Common.cc
  compnamer.SetNamingOptions( "era" );
  const int parse = compnamer.ParseOptions( argc, argv );
  if( parse == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  if( parse == mgr::OptNamer::PARSE_HELP  ){ return 0; }


  InitSampleStatic( compnamer );

  const mgr::ConfigReader& master = compnamer.MasterConfig();

  const string datatag = compnamer.GetChannelEXT( "Data Prefix" ) + compnamer.GetExt<string>( "era", "Data Postfix" );

  /*******************************************************************************
  *   Histogram manager initialization
  *   Whether to initialize from EDM files or existing root file is handle
  *   by the SampleErrHistMgr constructor.
  *******************************************************************************/
  // Defining data settings
  SampleErrHistMgr* data = new SampleErrHistMgr( datatag, master );
  data->LoadFromFile();

  // Defining out channels see data/Groups.json for sample settings
  vector<SampleErrHistMgr*> background;

  for( const auto bkggroup : master.GetStaticStringList( "Background List" ) ){
    background.push_back( new SampleErrHistMgr( bkggroup, master ) );
    background.back()->LoadFromFile();
    background.back()->Scale( mgr::SampleMgr::TotalLuminosity() );
  }


  // Declaring sample sample
  vector<SampleErrHistMgr*> siglist;

  for( const auto& signame : master.GetStaticStringList( "Signal List" ) ){
    siglist.push_back( new SampleErrHistMgr( signame, master ) );
    siglist.back()->LoadFromFile();
    siglist.back()->Scale( mgr::SampleMgr::TotalLuminosity() );
  }


  /*******************************************************************************
  *   Making full comparison Plots
  *   Data plotting is handled by the plotting functions
  *******************************************************************************/
  MakeFullComparePlot( data, background, siglist[1] );
  Normalize( data, background );
  MakeFullComparePlot( data, background, siglist[1], "normalized" );

  /*******************************************************************************
  *   Object clean up .
  *******************************************************************************/
  for( auto& histmgr : background ){
    delete histmgr;
  }

  for( auto& sig : siglist ){
    delete sig;
  }

  delete data;


  return 0;
}

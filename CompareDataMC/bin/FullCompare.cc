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

#include <boost/program_options.hpp>
#include <iostream>

using namespace std;
namespace opt = boost::program_options;

/*******************************************************************************
*   Main control flow
*******************************************************************************/
int
main( int argc, char* argv[] )
{
  opt::options_description desc( "Options for FullComparison" );
  desc.add_options()
    ( "masspoint,p", opt::value<string>()->default_value( "TstarM800" ), "SignalmMass to use as comparison" )
  ;

  compnamer.AddOptions( CompareOptions() ).AddOptions( desc );// defined in src/Common.cc
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
  SampleErrHistMgr* sigmgr =  new SampleErrHistMgr( compnamer.GetInput<string>("masspoint"), master );
  sigmgr->LoadFromFile();
  sigmgr->Scale( mgr::SampleMgr::TotalLuminosity() );

  /*******************************************************************************
  *   Making full comparison Plots
  *   Data plotting is handled by the plotting functions
  *******************************************************************************/
  MakeFullComparePlot( data, background, sigmgr );
  Normalize( data, background );
  MakeFullComparePlot( data, background, sigmgr, "normalized" );

  /*******************************************************************************
  *   Object clean up .
  *******************************************************************************/
  for( auto& histmgr : background ){
    delete histmgr;
  }

  delete sigmgr;
  delete data;

  return 0;
}

/*******************************************************************************
*
*  Filename    : ErrorCompare.cc
*  Description : Comparison of error
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
  opt::options_description desc( "Options for Error compare" );
  desc.add_options()
    ( "group,g",   opt::value<string>()->required(), "Which group of MC sample do you wish to compare errors" )
  ;

  compnamer.AddOptions( CompareOptions() );
  compnamer.AddOptions( desc );
  compnamer.SetNamingOptions( "group", "era" );
  const int parse = compnamer.ParseOptions( argc, argv );
  if( parse == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  if( parse == mgr::OptNamer::PARSE_HELP  ){ return 0; }

  InitSampleStatic( compnamer );

  const mgr::ConfigReader& master = compnamer.MasterConfig();

  /*******************************************************************************
  *   Loading histograms
  *******************************************************************************/
  vector<SampleErrHistMgr*> mgrlist;
  if( compnamer.GetInput<string>( "group" ).find( "Tstar" ) != string::npos ){
    mgrlist.push_back( new SampleErrHistMgr( compnamer.GetInput<string>( "group" ), master ) );
    mgrlist.back()->LoadFromFile();
    mgrlist.back()->Scale( mgr::SampleMgr::TotalLuminosity() );
  } else if( compnamer.GetInput<string>( "group" ) == "Background" ){
    for( const auto& bkg : master.GetStaticStringList( "Background List" ) ){
      mgrlist.push_back( new SampleErrHistMgr( bkg, master ) );
      mgrlist.back()->LoadFromFile();
      mgrlist.back()->Scale( mgr::SampleMgr::TotalLuminosity() );
    }
  } else {
    cerr << "Unsupported Channel! [" << compnamer.GetInput<string>("group") << "]" << endl;
    return 1;
  }

  /*******************************************************************************
  *   Calling the Error compare plotting function
  *******************************************************************************/
  for( const auto& err : histerrlist ){
    PlotErrCompare( mgrlist, "TstarMass", err );
    PlotErrCompare( mgrlist, "JetNum",    err );
    PlotErrCompare( mgrlist, "Jet1Pt",    err );
    PlotErrCompare( mgrlist, "LepPt",     err );
  }

  for( auto& mgr : mgrlist ){
    delete mgr ;
  }

  return 0;
}

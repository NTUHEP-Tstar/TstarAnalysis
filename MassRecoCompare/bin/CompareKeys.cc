/*******************************************************************************
*
*  Filename    : CompareKeys.cc
*  Description : Main control flow of Keys PDF comparison
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/KeysCompMgr.hpp"

#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>

using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{
  /*******************************************************************************
  *   Options definition
  *******************************************************************************/
  opt::options_description desc( "Options for comparing Keys Pdf results" );
  desc.add_options()
    ( "channel,c", opt::value<string>(), "Channel to run" )
    ( "mass,m", opt::value<int>(), "Masspoint to run" )
    ( "compare,x", opt::value<string>(), "Which reconstruction methods to use" )
  ;

  reconamer.SetNamingOptions( {"mass", "compare"} );
  const int run = reconamer.LoadOptions( desc, argc, argv );
  if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
  if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }

  /*******************************************************************************
  *   Making KeysCompMgr
  *******************************************************************************/
  KeysCompMgr::InitStaticVars();

  const string name    = reconamer.InputStr( "compare" );
  const string ltxname = reconamer.query_tree( "reco", name, "Root Name" );
  KeysCompMgr* compmgr = new KeysCompMgr( name, ltxname );

  /*******************************************************************************
  *   Filling and plotting
  *******************************************************************************/
  boost::format globformat( "/wk_cms/yichen/TstarAnalysis/EDMStore/recocomp/%s/*%d*.root" );
  const string globq = str( globformat % reconamer.InputStr( "channel" ) % reconamer.InputInt( "mass" ) );

  cout << globq << endl;

  for( const auto& file : mgr::Glob( globq ) ){
    cout << file << endl;
    compmgr->FillDataSet( file );
  }

  compmgr->MakeKeysPdf();

  MakeKeysPlots( compmgr );

  /*******************************************************************************
  *   Clearing up
  *******************************************************************************/
  delete compmgr;


  return 0;
}

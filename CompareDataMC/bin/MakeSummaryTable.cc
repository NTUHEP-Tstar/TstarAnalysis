/*******************************************************************************
*
*  Filename    : MakeSummaryTable.cc
*  Description : Generate Latex Summary table
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Common/interface/ConfigReader.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeTable.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleTableMgr.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace mgr;

int
main( int argc, char* argv[] )
{

  /******************************************************************************/
  // options parsing and object initialization
  compnamer.AddOptions( CompareOptions() ); // defined in src/Common.cc
  compnamer.SetNamingOptions( "era" );
  const int parse = compnamer.ParseOptions( argc, argv );
  if( parse == OptNamer::PARSE_ERROR ){ return 1; }
  if( parse == OptNamer::PARSE_HELP ){ return 0; }

  InitSampleStatic( compnamer );

  const ConfigReader& master = compnamer.MasterConfig();
  const string datatag       = compnamer.GetChannelEXT( "Data Prefix" ) + compnamer.GetExt<string>( "era", "Data Postfix" );


  /******************************************************************************/
  cout << "Declaring sample groups...." << endl;

  vector<SampleTableMgr> siglist;
  vector<SampleTableMgr> bkglist;

  for( const auto& tag : master.GetStaticStringList( "Signal List" ) ){
    siglist.emplace_back( tag, master );
  }

  for( const auto& tag : master.GetStaticStringList( "Background List" ) ){
    bkglist.emplace_back( tag, master );
  }

  SampleTableMgr data( datatag, master );


  /******************************************************************************/
  cout << "Loading variables from saved file" << endl;
  data.LoadFromFile();

  for( auto& group : bkglist ){
    group.LoadFromFile();
  }

  for( auto& group : siglist ){
    group.LoadFromFile();
  }

  /******************************************************************************/
  // Calling the samples
  cout << "Making complete summary table...." << endl;
  SummaryComplete( siglist, bkglist, data );

  cout << "Making simple summary table...." << endl;
  SummaryBrief( siglist, bkglist, data );

  cout << "Making lumi summary table...." << endl;
  SummaryMCLumi( siglist, bkglist );


  return 0;
}

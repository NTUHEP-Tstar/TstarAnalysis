/*******************************************************************************
*
*  Filename    : CompareMethods
*  Description : Make Plots for Comparision of reconstruction methods
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/MultiFile.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/CompareHistMgr.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "TFile.h"

using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{
  /*******************************************************************************
  *   Option parsing
  *******************************************************************************/
  opt::options_description desc( "Options for Method comparison" );
  desc.add_options()
    ( "channel,c", opt::value<string>(), "Channel to run" )
    ( "mass,m", opt::value<int>(), "Masspoint to run" )
    ( "outputtag,o", opt::value<string>(), "What output string to add" )
    ( "compare,x", opt::value<vector<string> >()->multitoken(), "Which reconstruction methods to compare" )
  ;

  reconamer.AddOptions( desc );
  reconamer.SetNamingOptions( "mass" );
  const int run = reconamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }

  if( !reconamer.CheckInput( "compare" ) ){
    cerr << "Error! No comparison tokens specified!" << endl;
    return 1;
  }

  /*******************************************************************************
  *   Making objects
  *******************************************************************************/
  vector<CompareHistMgr*> complist;

  for( const auto& tag : reconamer.GetInputList<string>( "compare" ) ){
    const string taglatex = reconamer.ExtQuery<string>( "reco", tag, "Root Name" );
    complist.push_back( new CompareHistMgr( tag, taglatex ) );
  }

  /*******************************************************************************
  *   Filling from file
  *******************************************************************************/
  boost::format globformat( "/wk_cms/yichen/TstarAnalysis/EDMStore/recocomp/%s/*%d*.root" );
  const std::string globq = boost::str(
    globformat
    % reconamer.GetInput<string>( "channel" )
    % reconamer.GetInput<int>( "mass" )
    );

  mgr::MultiFileEvent myevent( mgr::Glob( globq ) );
  unsigned i = 0;
  boost::format reportformat( "\rAt Event [%u|%u]" );

  for( myevent.toBegin(); !myevent.atEnd(); ++myevent, ++i ){
    cout << reportformat % i % myevent.size() << flush;

    for( const auto& mgr : complist ){
      mgr->AddEvent( myevent.Base() );
    }
  }

  cout << "Done" << endl;

  /*******************************************************************************
  *   Making compare plots
  *******************************************************************************/
  for( const auto& mgr : complist ){
    MatchPlot( mgr );
  }

  ComparePlot( reconamer.GetInput<string>( "outputtag" ), complist );

  return 0;
}

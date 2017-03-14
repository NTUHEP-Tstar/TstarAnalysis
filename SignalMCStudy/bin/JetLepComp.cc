/*******************************************************************************
*
*  Filename    : MCCompare.cc
*  Description : Simple Comparison graphs for MC sample Comparison
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/SignalMCStudy/interface/JetLepMgr.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace opt = boost::program_options;

/*******************************************************************************
*   Main control flow
*******************************************************************************/
int
main( int argc, char* argv[] )
{
  opt::options_description desc( "options for jet-lep num compare" );
  desc.add_options()
    ( "masslist,m", opt::value<vector<string> >()->multitoken(), "which mass point to compare" );
  ;
  opt::variables_map vm;
  store( parse_command_line( argc, argv, desc ), vm );
  opt::notify( vm );

  cout << "Running jet lep comparison" << endl;

  vector<JetLepMgr*> samplelist;
  for( const auto& mass : vm["masslist"].as<vector<string>>() ){
    samplelist.push_back( new JetLepMgr(mass) );
  }

  MakeJetLepCompare( samplelist );

  for( const auto& sample : samplelist ){
    delete sample ;
  }
  return 0;
}

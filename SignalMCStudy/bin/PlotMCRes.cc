/*******************************************************************************
*
*  Filename    : MCCompare.cc
*  Description : Simple Comparison graphs for MC sample Comparison
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/SignalMCStudy/interface/MCResMgr.hpp"

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
  opt::options_description desc( "options for mc resolution comparison" );
  desc.add_options()
    ( "mass,m", opt::value<string>(), "which mass point to compare" );
  ;
  opt::variables_map vm;
  store( parse_command_line( argc, argv, desc ), vm );
  opt::notify( vm );

  cout << "Running MC resolution plots" << endl;

  MCResMgr* mgr = new MCResMgr( vm["mass"].as<string>() );

  MakeMCResPlot( mgr );

  delete mgr;
  return 0;
}

/*******************************************************************************
*
*  Filename    : ExtractSystematic.cc
*  Description : Printing the average of systematic to std::out
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/DisableNuisance.hpp"

#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int
main( int argc, char* argv[] )
{
  limnamer.AddOptions( LimitOptions() ).AddOptions( ExtraCutOptions() );
  limnamer.SetNamingOptions( "fitmethod", "fitfunc", "era" );

  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }

  return 0;
}

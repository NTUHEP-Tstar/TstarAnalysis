/*******************************************************************************
*
*  Filename    : PlotCombinePull.cc
*  Description : Plotting the post-fit pull distribution of various paramters
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*   Additional comments
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/CombinePull.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int
main( int argc, char* argv[] )
{
  limnamer.AddOptions( LimitOptions() ).AddOptions( MassOptions() );
  limnamer.SetNamingOptions( "fitmethod","fitfunc", "era", "masspoint" );
  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }

  MakeCombinePull();

  MakePullPlot();

  return 0;
}

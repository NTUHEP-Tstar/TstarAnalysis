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

#include <boost/program_options.hpp>

using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{

  opt::options_description desc( "Options for PlotCombinePull" );

  desc.add_options()
    ( "channel,c", opt::value<string>(), "which channel to run" )
    ( "fitmethod,m", opt::value<string>(), "which method to run")
    ( "fitfunc,f", opt::value<string>(), "which fit function to use" )
    ( "era,e", opt::value<string>(), "Which data era to use" )
    ( "mass,x", opt::value<string>(), "Data set tag for mass point" )
  ;

  limnamer.SetNamingOptions( {"fitmethod","fitfunc", "era", "mass"} );
  const int run = limnamer.LoadOptions( desc, argc, argv );
  if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
  if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }

  MakeCombinePull();

  MakePullPlot();

  return 0;
}

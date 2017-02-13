/*******************************************************************************
*
*  Filename    : PlotLimit.cc
*  Description : Plot limit main control flow
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/Limit.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{
  opt::options_description desc( "Options for MakeRooFit" );
  desc.add_options()
    ( "combine,x", opt::value<string>(), "Which method to run with combine" )
    ( "drawdata,d", "Whether to plot the data limits or not" )
  ;
  limnamer.AddOptions( LimitOptions() ).AddOptions( desc );
  limnamer.SetNamingOptions( "fitmethod", "fitfunc", "era" );
  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }

  if( limnamer.CheckInput( "combine" ) ){
    cout << "Rerunning results higgs combine package with method ["
         << limnamer.GetInput<string>( "combine" ) << "] !" << endl;
    RunCombine();
  }

  cout << "Making limit plot" << endl;
  auto lim = MakeLimitPlot();

  cout << lim.first.CentralValue() << " " << lim.second.CentralValue() << endl;

  return 0;
}

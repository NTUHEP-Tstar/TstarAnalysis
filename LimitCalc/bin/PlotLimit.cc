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
      ( "channel,c", opt::value<string>(), "Which channel to run" )
      ( "fitmethod,m", opt::value<string>(), "Which fitting method to use" )
      ( "fitfunc,f", opt::value<string>(), "Which fitting function to use" )
      ( "combine,x", opt::value<string>(), "Which method to run with combine" )
      ( "era,e", opt::value<string>(), "Which data era to use" )
      ( "drawdata,d", "Whether to plot the data limits or not")
   ;

   limnamer.SetNamingOptions( {"fitmethod", "fitfunc", "era"} );
   const int run = limnamer.LoadOptions( desc, argc, argv );
   if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }
   if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }

   if( limnamer.GetMap().count( "combine" ) ){
      cout << "Rerunning results higgs combine package with method ["
           << limnamer.GetInput( "combine" ) << "] !" << endl;
      RunCombine();
   }

   cout << "Making limit plot" << endl;
   MakeLimitPlot();

   return 0;
}

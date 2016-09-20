/*******************************************************************************
*
*  Filename    : CompareScale.cc
*  Description : Generating plots for a scale
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/TstarMassReco/interface/Bin_Common.hpp"
#include "TstarAnalysis/TstarMassReco/interface/ScaleHistMgr.hpp"

#include <boost/program_options.hpp>
using namespace std;

namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{
   opt::options_description desc( "Options for Method comparison" );

   desc.add_options()
      ( "channel,c", opt::value<string>(), "Channel to run" )
      ( "mass,m", opt::value<string>(), "Masspoint to run" )
   ;

   reconamer.SetNamingOptions( {"mass"} );
   const int run = reconamer.LoadOptions( desc, argc, argv );
   if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }

   // Initializing static members
   InitSampleStatic( reconamer );

   // Making and filling histograms, see src/ScaleHistMgr.cc
   ScaleHistMgr* scalemgr = new ScaleHistMgr( reconamer.GetInput( "mass" ), reconamer.MasterConfigFile() );

   // Making plots, see src/ScalePlot.cc
   ScalePlot( scalemgr );

   delete scalemgr;

   return 0;
}

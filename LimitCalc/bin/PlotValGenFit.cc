/*******************************************************************************
*
*  Filename    : PlotValGenFit.cc
*  Description : Options for plotting ValGenFit
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{

   opt::options_description desc( "Options for PlotValGenFit" );

   desc.add_options()
      ( "channel,c", opt::value<string>(), "which channel to run" )
      ( "fitfunc,f", opt::value<string>(), "which fit function to use" )
      ( "era,e", opt::value<string>(), "Which data era to use" )
      ( "relmag,x", opt::value<double>(), "Relative magnitude of signal compared with prediction" )
      ( "absmag,a", opt::value<double>(), "Absolute magnitude of signal (number of events)" )
   ;

   limnamer.SetNamingOptions( {"fitfunc","era"} );
   const int run = limnamer.LoadOptions( desc, argc, argv );
   if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }

   if( limnamer.HasOption( "relmag" ) && limnamer.HasOption( "absmag" ) ){
      cerr << "Cannot set both relative and absolute magnitued" << endl;
      return 1;
   } else if( !limnamer.HasOption( "relmag" ) && !limnamer.HasOption( "absmag" ) ){
      cerr << "Must set either relative of absolute magnitude" << endl;
      return 1;
   }

   InitSampleStatic( limnamer );

   const mgr::ConfigReader& cfg = limnamer.MasterConfig();
   PlotGenFit( cfg.GetStaticStringList( "Signal List" ) );

   return 0;
}

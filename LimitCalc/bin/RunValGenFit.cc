/*******************************************************************************
*
*  Filename    : RunValGenFit.cc
*  Description : Options parsing for running validation on single mass point
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
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

   SampleRooFitMgr* data   = NULL;
   SampleRooFitMgr* mc     = NULL;
   SampleRooFitMgr* sigmgr = NULL;

   opt::options_description desc( "Options for RunValGenFit" );

   desc.add_options()
      ( "channel,c", opt::value<string>(), "which channel to run" )
      ( "fitfunc,f", opt::value<string>(), "which fit function to use" )
      ( "num,n", opt::value<int>(), "Run <num> times of gen-pseudo data + fit" )
      ( "mass,m", opt::value<string>(), "Dataset tag for mass point" )
      ( "relmag,x", opt::value<double>(), "Relative magnitude of signal compared with prediction" )
      ( "absmag,a", opt::value<double>(), "Absolute magnitude of signal (number of events)" )
   ;

   limnamer.SetNamingOptions( {"fitfunc", "mass"} );
   const int run = limnamer.LoadOptions( desc, argc, argv );
   if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }

   // Parsing options
   if( limnamer.HasOption( "relmag" ) && limnamer.HasOption( "absmag" ) ){
      cerr << "Cannot set both relative and absolute magnitued" << endl;
      return 1;
   } else if( !limnamer.HasOption( "relmag" ) && !limnamer.HasOption( "absmag" ) ){
      cerr << "Must set either relative of absolute magnitude" << endl;
      return 1;
   }
   if( !limnamer.HasOption( "num" ) ){
      cerr << "Must specify number of runs!" << endl;
      return 1;
   }

   InitSampleStatic( limnamer );
   InitRooFitSettings( limnamer );
   InitSingle( data,   limnamer.GetChannelEXT( "Data Tag" ) );
   InitSingle( sigmgr, limnamer.GetInput( "mass" ) );
   InitMC( mc );

   RunGenFit( data, mc, sigmgr );

   delete data;
   delete mc;
   delete sigmgr;

   return 0;
}

/*******************************************************************************
*
*  Filename    : KinematicCompare.cc
*  Description : Simple comparison of kinematic distribution of various
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeHist.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;
namespace opt = boost::program_options;

// ------------------------------------------------------------------------------
//   Main control flow
// ------------------------------------------------------------------------------
int
main( int argc, char* argv[] )
{
   opt::options_description desc( "Options for KinematicCompare" );
   desc.add_options()
      ( "channel,c", opt::value<string>(), "What channel to run" )
   ;

   const int parse = compare_namer.LoadOptions( desc, argc, argv );// defined in Compare_Common.hpp
   if( parse == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   if( parse == mgr::OptsNamer::PARSE_HELP ){ return 0; }

   InitSampleStatic( compare_namer );

   const mgr::ConfigReader master( compare_namer.MasterConfigFile() );
   const string datatag = compare_namer.GetChannelEXT( "Data Tag" );

   // Defining data settings
   SampleHistMgr* data = new SampleHistMgr( datatag, master );
   // Defining out channels see data/Groups.json for sample settings
   vector<SampleHistMgr*> background;

   for( const auto bkggroup : master.GetStaticStringList( "Background List" ) ){
      background.push_back( new SampleHistMgr( bkggroup, master ) );
   }

   // Declaring sample sample
   SampleHistMgr* sigmgr = new SampleHistMgr( "TstarM800", master );

   // Making combined stack plots
   MakeComparePlot( data, background, sigmgr );

   // Normalizing MC to data
   Normalize( data, background );

   // Remake combined stack plots with "normalize" filename tag;
   MakeComparePlot( data, background, sigmgr, "normalized" );

   // Cleaning up
   for( auto& histmgr : background ){
      delete histmgr;
   }

   delete data;
   delete sigmgr;
   return 0;
}

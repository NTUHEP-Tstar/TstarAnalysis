/*******************************************************************************
*
*  Filename    : MakeSummaryTable.cc
*  Description : Generate Latex Summary table
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"

#include "TstarAnalysis/CompareDataMC/interface/MakeTable.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;
using namespace mgr;
namespace opt = boost::program_options;


int
main( int argc, char* argv[] )
{

   /******************************************************************************/
   // options parsing and object initialization
   opt::options_description desc( "Options for KinematicCompare" );
   desc.add_options()
      ( "channel,c", opt::value<string>(), "What channel to run" )
   ;

   const int parse = compnamer.LoadOptions( desc, argc, argv );
   if( parse == OptsNamer::PARSE_ERROR ){ return 1; }
   if( parse == OptsNamer::PARSE_HELP ){ return 0; }

   InitSampleStatic( compnamer );

   const ConfigReader master( compnamer.MasterConfigFile() );


   /******************************************************************************/
   cout << "Declaring sample groups...." << endl;

   SampleGroup data( compnamer.GetChannelEXT( "Data Tag" ), master );
   vector<SampleGroup> siglist;
   vector<SampleGroup> bkglist;

   for( const auto& tag : master.GetStaticStringList( "Signal List" ) ){
      siglist.push_back( SampleGroup( tag, master ) );
   }


   for( const auto& tag : master.GetStaticStringList( "Background List" ) ){
      bkglist.push_back( SampleGroup( tag, master ) );
   }


   /******************************************************************************/
   cout << "Re-computing the selection efficiencies and caching variables!" << endl;

   InitGroupForTable( data );

   for( auto& group : bkglist ){
      InitGroupForTable( group );
   }

   for( auto& group : siglist ){
      InitGroupForTable( group );
   }

   /******************************************************************************/
   // Calling the samples
   cout << "Making complete summary table...." << endl;
   SummaryComplete( siglist, bkglist, data );

   cout << "Making simple summary table...." << endl;
   SummaryBrief( siglist, bkglist, data );

   cout << "Making lumi summary table...." << endl;
   SummaryMCLumi( siglist, bkglist );


   return 0;
}

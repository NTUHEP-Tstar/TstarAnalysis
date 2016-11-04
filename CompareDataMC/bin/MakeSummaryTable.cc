/*******************************************************************************
*
*  Filename    : MakeSummaryTable.cc
*  Description : Generate Latex Summary table
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeTable.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleTableMgr.hpp"

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
      ( "refresh,r", "Whether to recompute the value from EDM files" )
   ;

   const int parse = compnamer.LoadOptions( desc, argc, argv );
   if( parse == OptsNamer::PARSE_ERROR ){ return 1; }
   if( parse == OptsNamer::PARSE_HELP ){ return 0; }

   InitSampleStatic( compnamer );

   const ConfigReader master( compnamer.MasterConfigFile() );


   /******************************************************************************/
   cout << "Declaring sample groups...." << endl;

   vector<SampleTableMgr> siglist;
   vector<SampleTableMgr> bkglist;

   for( const auto& tag : master.GetStaticStringList( "Signal List" ) ){
      siglist.emplace_back( tag, master );
   }

   for( const auto& tag : master.GetStaticStringList( "Background List" ) ){
      bkglist.emplace_back( tag, master );
   }

   SampleTableMgr data( compnamer.GetChannelEXT( "Data Tag" ), master );


   /******************************************************************************/
   if( compnamer.HasOption( "refresh" ) ){
      cout << "Re-computing the selection efficiencies and caching variables!" << endl;

      data.LoadFromEDM();

      for( auto& group : bkglist ){
         group.LoadFromEDM();
      }

      for( auto& group : siglist ){
         group.LoadFromEDM();
      }
   } else {
      cout << "Loading variables from saved file" << endl;
      data.LoadFromFile();

      for( auto& group : bkglist ){
         group.LoadFromFile();
      }

      for( auto& group : siglist ){
         group.LoadFromFile();
      }
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

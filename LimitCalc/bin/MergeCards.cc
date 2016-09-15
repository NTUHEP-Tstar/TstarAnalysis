/*******************************************************************************
*
*  Filename    : MergeCards.cc
*  Description : Calling functions to merge datacards together
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"

#include <boost/program_options.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace opt = boost::program_options;

char buffer[65536];

int
main( int argc, char* argv[] )
{
   opt::options_description desc( "Options for MakeRooFit" );
   desc.add_options()
      ( "channel,c", opt::value<string>(), "Which channel to run" )
      ( "fitmethod,m", opt::value<string>(), "Which fitting method to use" )
      ( "fitfunc,f", opt::value<string>(), "Which fitting function to use" )
   ;

   const int run = limit_namer.LoadOptions( desc, argc, argv );
   if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }
   if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   limit_namer.SetNamingOptions( {"fitmethod", "fitfunc"} );

   const mgr::ConfigReader cfg( limit_namer.MasterConfigFile() );
   const vector<string> masspoint_list = cfg.GetStaticStringList( "Signal List" );

   for( auto& masspoint : masspoint_list ){
      limit_namer.SetChannel( "MuonSignal" );
      const string muonfile = limit_namer.TextFileName( "card", {masspoint} );
      limit_namer.SetChannel( "ElectronSignal" );
      const string elecfile = limit_namer.TextFileName( "card", {masspoint} );
      limit_namer.SetChannel( "SignalMerge" );
      const string combfile = limit_namer.TextFileName( "card", {masspoint} );

      sprintf(
         buffer, "cp %s temp1.txt && cp %s temp2.txt",
         muonfile.c_str(),
         elecfile.c_str()
         );
      system( buffer );

      sprintf(
         buffer, "combineCards.py  %s %s > %s",
         "temp1.txt",
         "temp2.txt",
         combfile.c_str()
         );
      system( buffer );

      system( "rm temp1.txt temp2.txt" );
   }

   return 0;
}

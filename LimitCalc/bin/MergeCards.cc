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
   opt::options_description desc( "Options for MergeCards" );
   desc.add_options()
      ( "fitmethod,m", opt::value<string>(), "Which fitting method to use" )
      ( "fitfunc,f", opt::value<string>(), "Which fitting function to use" )
      ( "channellist,c", opt::value<vector<string> >()->multitoken(), "Which channels to merge" )
   ;

   limit_namer.SetNamingOptions( {"fitmethod", "fitfunc"} );
   const int run = limit_namer.LoadOptions( desc, argc, argv );
   if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }
   if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }

   const mgr::ConfigReader cfg( limit_namer.MasterConfigFile() );
   const mgr::ConfigReader higgscfg( limit_namer.SettingsDir() + "higgs_combine_settings.json" );
   const vector<string> masspointlist = cfg.GetStaticStringList( "Signal List" );
   const vector<string> channellist = limit_namer.GetMap()["channellist"].as<vector<string> >() ;

   for( auto& masspoint : masspointlist ){
      vector<string> inputcardlist;


      for( const auto& channel : channellist ){
         limit_namer.SetChannel( channel );
         inputcardlist.push_back( limit_namer.TextFileName( "card", {masspoint} ) );
      }

      limit_namer.SetChannel( "SignalMerge" );
      const string combfile = limit_namer.TextFileName( "card", {masspoint} );

      FILE* tempscript = fopen( "temp.sh", "w" );
      fprintf( tempscript, "#!/bin/bash\n" );
      fprintf( tempscript, "cd %s/%s/src\n",
         higgscfg.GetStaticString( "Store Path" ).c_str(),
         higgscfg.GetStaticString( "CMSSW Version" ).c_str()
         );
      fprintf( tempscript, "eval `scramv1 runtime -sh`\n" );

      unsigned i = 0;

      for( const auto& cardfile : inputcardlist ){
         fprintf( tempscript, "cp %s temp%u.txt\n", cardfile.c_str(), i );
         ++i;
      }

      fprintf( tempscript, "combineCards.py " );

      for( i = 0; i < inputcardlist.size(); ++i ){
         fprintf( tempscript, "temp%u.txt ", i );

      }

      fprintf( tempscript, " > %s\n", combfile.c_str() );

      for( i = 0; i < inputcardlist.size(); ++i ){
         fprintf( tempscript, "rm temp%u.txt\n", i );
         ++i;
      }

      fclose( tempscript );

      system( "chmod +x temp.sh" );
      system( "./temp.sh" );
      system( "rm temp.sh" );
   }

   return 0;
}

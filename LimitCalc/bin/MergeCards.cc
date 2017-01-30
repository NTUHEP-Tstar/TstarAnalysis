/*******************************************************************************
*
*  Filename    : MergeCards.cc
*  Description : Calling functions to merge datacards together
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Common/interface/ConfigReader.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"
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
    ( "era,e", opt::value<string>(), "Which data era to use" )
  ;

  limnamer.SetNamingOptions( {"fitmethod", "fitfunc", "era"} );
  const int run = limnamer.LoadOptions( desc, argc, argv );
  if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }
  if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }

  const mgr::ConfigReader& cfg = limnamer.MasterConfig();
  const mgr::ConfigReader higgscfg( limnamer.SettingsDir() / "higgs_combine_settings.json" );
  const vector<string> masspointlist = cfg.GetStaticStringList( "Signal List" );
  const vector<string> channellist   = limnamer.GetMap()["channellist"].as<vector<string> >();

  for( auto& masspoint : masspointlist ){
    vector<string> inputcardlist;


    for( const auto& channel : channellist ){
      limnamer.SetChannel( channel );
      inputcardlist.push_back( limnamer.TextFileName( "card", {masspoint} ) );
    }

    limnamer.SetChannel( "SignalMerge" );
    const string combfile = limnamer.TextFileName( "card", {masspoint} );

    FILE* tempscript = fopen( "temp.sh", "w" );
    fprintf( tempscript, "#!/bin/bash\n" );
    fprintf( tempscript, "cd %s/%s/src\n",
      higgscfg.GetStaticString( "Store Path" ).c_str(),
      higgscfg.GetStaticString( "CMSSW Version" ).c_str()
      );
    fprintf( tempscript, "eval `scramv1 runtime -sh 2> /dev/null`\n" );

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

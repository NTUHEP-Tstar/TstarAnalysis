/*******************************************************************************
*
*  Filename    : MergeCards.cc
*  Description : Calling functions to merge datacards together
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Common/interface/ConfigReader.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgrLoader.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <fstream>
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
    ( "channellist,l", opt::value<vector<string> >()->multitoken(), "Which channels to merge" )
  ;

  limnamer.AddOptions( LimitOptions() ).AddOptions( desc );
  limnamer.SetNamingOptions( "fitmethod", "fitfunc", "era" );
  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }

  const mgr::ConfigReader& cfg = limnamer.MasterConfig();
  const mgr::ConfigReader higgscfg( limnamer.SettingsDir() / "higgs_combine_settings.json" );
  const vector<string> masspointlist = cfg.GetStaticStringList( "Signal List" );
  const vector<string> channellist   = limnamer.GetInputList<string>( "channellist" );
  const string outputchannel         = limnamer.GetInput<string>( "channel" );

  for( auto& masspoint : masspointlist ){
    vector<string> inputcardlist;
    double originalevtcount = 0;
    double selectedevtcount = 0;
    double scaleupcount     = 0;
    double scaledowncount   = 0;
    double pdfupcount       = 0;
    double pdfdowncount     = 0;

    /*******************************************************************************
    *   Loading old information
    *******************************************************************************/
    for( const auto& channel : channellist ){
      limnamer.SetChannel( channel );
      inputcardlist.push_back( limnamer.TextFileName( "card", masspoint ) );

      mgr::SampleMgr mgr( "temp" );
      mgr::LoadCacheFromFile( mgr, limnamer.CustomFileName( "txt", masspoint ) );
      originalevtcount += mgr.OriginalEventCount();
      selectedevtcount += mgr.SelectedEventCount();
      scaleupcount     += mgr.GetCacheDouble( "scaleup" );
      scaledowncount   += mgr.GetCacheDouble( "scaledown" );
      pdfupcount       += mgr.GetCacheDouble( "PDFup" );
      pdfdowncount     += mgr.GetCacheDouble( "PDFdown" );
    }

    limnamer.SetChannel( outputchannel );
    const string combfile = limnamer.TextFileName( "card", masspoint );

    /*******************************************************************************
    *   Combine card Call
    *******************************************************************************/

    ofstream tempscript( "temp.sh" );

    tempscript << "#!/bin/bash" << endl;
    tempscript << boost::format( "cd %s/%s/src" )
      % higgscfg.GetStaticString( "Store Path" )
      % higgscfg.GetStaticString( "CMSSW Version" )
               << endl;
    tempscript << "eval `scramv1 runtime -sh 2> /dev/null`" << endl;

    boost::format tmpcardfile( "temp%u.txt" );

    for( size_t i = 0; i < inputcardlist.size(); ++i ){
      const auto cardfile = inputcardlist.at( i );
      tempscript << "cp " << cardfile << " " << ( tmpcardfile%i ) << endl;
    }

    tempscript << "combineCards.py " << flush;

    for( size_t i = 0; i < inputcardlist.size(); ++i ){
      tempscript << ( tmpcardfile%i ) << " "  << flush;
    }

    tempscript << " > " << combfile << endl;

    for( size_t i = 0; i < inputcardlist.size(); ++i ){
      tempscript << "rm " << ( tmpcardfile%i ) << endl;
    }

    tempscript.close();

    system( "chmod +x temp.sh" );
    system( "./temp.sh" );
    system( "rm temp.sh" );

    /*******************************************************************************
    *   Outputing sample mgr cached event count
    *******************************************************************************/
    mgr::SampleMgr mgr( "temp" );
    mgr.SetOriginalEventCount( originalevtcount );
    mgr.SetSelectedEventCount( selectedevtcount );
    mgr.AddCacheDouble( "scaleup",   scaleupcount );
    mgr.AddCacheDouble( "scaledown", scaledowncount );
    mgr.AddCacheDouble( "PDFup",     pdfupcount );
    mgr.AddCacheDouble( "PDFdown",   pdfdowncount );
    mgr::SaveCacheToFile( mgr, limnamer.CustomFileName( "txt", masspoint ) );

  }

  return 0;
}

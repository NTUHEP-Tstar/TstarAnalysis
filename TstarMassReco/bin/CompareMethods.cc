/*******************************************************************************
 *
 *  Filename    : CompareMethods
 *  Description : Make Plots for Comparision of reconstruction methods
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/CompareHistMgr.hpp"
#include "TstarAnalysis/TstarMassReco/interface/Bin_Common.hpp"
#include "DataFormats/FWLite/interface/Event.h"

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "TFile.h"

using namespace std;
namespace opt = boost::program_options;

int main( int argc, char* argv[] )
{
   opt::options_description desc("Options for Method comparison") ;

   desc.add_options()
      ("channel,c"  , opt::value<string>(), "Channel to run" )
      ("mass,m"     , opt::value<string>(), "Masspoint to run" )
      ("outputtag,o", opt::value<string>(), "What output string to add" )
      ("compare,x"  , opt::value<vector<string>>()->multitoken(), "Which reconstruction methods to compare" )
   ;

   reconamer.SetNamingOptions({"mass"});
   const int run = reconamer.LoadOptions( desc, argc, argv );
   if( run == mgr::OptsNamer::PARSE_ERROR ) { return 1; }
   if( run == mgr::OptsNamer::PARSE_HELP  ) { return 0 ;}

   if( !reconamer.HasOption("compare") ){
      cerr << "Error! No comparison tokens specified!" << endl;
      return 1;
   }

   // Making run objects
   vector<CompareHistMgr*> complist;
   for( const auto& tag : reconamer.GetMap()["compare"].as<vector<string>>() ){
      const string taglatex = reconamer.query_tree("reco",tag,"Root Name");
      complist.push_back( new CompareHistMgr(tag,taglatex) );
   }

   // Filling histograms
   const string filename = reconamer.CustomFileName( "root", {reconamer.InputStr("mass")} );
   fwlite::Event event( TFile::Open( filename.c_str() ) );
   unsigned i = 0 ;
   for( event.toBegin() ; !event.atEnd() ; ++event ){
      cout << "\rAt Event [" << ++i << "]" << flush;
      for( const auto& mgr : complist ){
         mgr->AddEvent( event );
      }
   }

   // Making plots
   for( const auto& mgr : complist ){
      MatchPlot( mgr );
   }

   ComparePlot( reconamer.InputStr("outputtag")  , complist );

   return 0;
}

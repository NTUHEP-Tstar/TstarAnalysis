/*******************************************************************************
*
*  Filename    : FullCompare.cc
*  Description : Full kinematic compare with errors represented
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeErrHist.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;
namespace opt = boost::program_options;

/*******************************************************************************
*   Main control flow
*******************************************************************************/
int
main( int argc, char* argv[] )
{
   opt::options_description desc( "Options for KinematicCompare" );
   desc.add_options()
      ( "channel,c", opt::value<string>(), "What channel to run" )
      ( "drawdata,d", "options to add if you wish to add data into plotting")
   ;

   const int parse = compnamer.LoadOptions( desc, argc, argv );// defined in Compare_Common.hpp
   if( parse == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   if( parse == mgr::OptsNamer::PARSE_HELP ){ return 0; }

   InitSampleStatic( compnamer );

   const mgr::ConfigReader master( compnamer.MasterConfigFile() );
   const string datatag = compnamer.GetChannelEXT( "Data Tag" );

   /*******************************************************************************
   *   Histogram manager initialization
   *   Whether to initialize from EDM files or existing root file is handle
   *   by the SampleErrHistMgr constructor.
   *******************************************************************************/
   // Defining out channels see data/Groups.json for sample settings
   vector<SampleErrHistMgr*> background;
   for( const auto bkggroup : master.GetStaticStringList( "Background List" ) ){
      background.push_back( new SampleErrHistMgr( bkggroup, master ) );
      background.back()->LoadFromFile();
   }

   // Declaring sample sample
   vector<SampleErrHistMgr*> siglist;
   for( const auto& signame : master.GetStaticStringList( "Signal List" ) ){
      siglist.push_back( new SampleErrHistMgr( signame, master ) );
      siglist.back()->LoadFromFile();
   }

   // Defining data settings
   SampleErrHistMgr* data = new SampleErrHistMgr( datatag, master );
   data->LoadFromFile();

   /*******************************************************************************
   *   Plotting error comparison histograms
   *******************************************************************************/
   for( const auto& err : histerrlist ){
      for( const auto& sig : siglist ){
         PlotErrCompare( sig->Name(), {sig}, "TstarMass", err );
      }

      PlotErrCompare( "bkg", background, "TstarMass", err );
      PlotErrCompare( "bkg", background, "JetNum",    err );
      PlotErrCompare( "bkg", background, "Jet1Pt",    err );
      PlotErrCompare( "bkg", background, "LepPt",     err );
   }

   /*******************************************************************************
   *   Making full comparison Plots
   *   Data plotting is handled by the plotting functions 
   *******************************************************************************/
   MakeFullComparePlot( data, background, siglist[1] );
   Normalize( data, background );
   MakeFullComparePlot( data, background, siglist[1], "normalized" );

   /*******************************************************************************
   *   Object clean up .
   *******************************************************************************/
   for( auto& histmgr : background ){
      delete histmgr;
   }

   for( auto& sig : siglist ){
      delete sig;
   }

   delete data;


   return 0;
}

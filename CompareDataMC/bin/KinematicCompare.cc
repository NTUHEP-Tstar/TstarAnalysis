/*******************************************************************************
 *
 *  Filename    : KinematicCompare.cc
 *  Description : Simple comparison of kinematic distribution of various
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;
namespace opt = boost::program_options;
//------------------------------------------------------------------------------
//   Helper extern functions, see src/KinematicCompare.cc
//------------------------------------------------------------------------------
extern void MakeComparePlot(
   SampleHistMgr* data,
   vector<SampleHistMgr*>& bg,
   SampleHistMgr* signal,
   const string label = ""
);

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
   opt::options_description  desc("Options for KinematicCompare");
   desc.add_options()
      ("channel,c", opt::value<string>(), "What channel to run")
   ;

   const int parse =  compare_namer.LoadOptions( desc, argc, argv ); // defined in Compare_Common.hpp
   if( parse == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   if( parse == mgr::OptsNamer::PARSE_HELP ) { return 0; }

   InitSampleStatic( compare_namer );

   const mgr::ConfigReader master( compare_namer.MasterConfigFile() );
   const string           data_tag = compare_namer.GetChannelEXT("Data Tag");

   // Defining data settings
   SampleHistMgr* data = new SampleHistMgr( data_tag , master );
   // Defining out channels see data/Groups.json for sample settings
   vector<SampleHistMgr*>  background;
   background.push_back( new SampleHistMgr("TTJets"     , master ) );
   background.push_back( new SampleHistMgr("SingleTop"  , master ) );
   background.push_back( new SampleHistMgr("TTBoson"    , master ) );
   background.push_back( new SampleHistMgr("SingleBoson", master ) );
   background.push_back( new SampleHistMgr("DiBoson"    , master ) );

   // Declaring sample sample
   SampleHistMgr* signal_mgr = new SampleHistMgr( "tstar_M700" , master );

   // Making combined stack plots src/KinematicComp:are.cc
   MakeComparePlot( data, background, signal_mgr );

   // Normalizing MC to data
   Parameter bg_original_yield(0,0,0);
   for( const auto sample : background ){
      bg_original_yield += sample->ExpectedYield();
   }
   const double bg_scale = data->EventsInFile()/bg_original_yield.CentralValue();
   for( auto sample: background){
      sample->Scale(bg_scale);
   }

   MakeComparePlot( data, background, signal_mgr , "normalized");

   // Cleaning up
   for( auto& histmgr : background ){
      delete histmgr;
   }
   delete data;
   delete signal_mgr;
   return 0;
}

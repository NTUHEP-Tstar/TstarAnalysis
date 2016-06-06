/*******************************************************************************
 *
 *  Filename    : KinematicCompare.cc
 *  Description : Simple comparison of kinematic distribution of various
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"

#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"

#include "TCanvas.h"
#include "TLegend.h"
#include "THStack.h"
#include "TColor.h"
#include "TLatex.h"
#include "TLine.h"

#include <iostream>
using namespace std;

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
   if( argc != 2 ){
      cerr << "Error! Expected exactly one arguement!" << endl;
      return 1;
   }
   SetChannelType( argv[1] );

   mgr::ConfigReader static_cfg( "./data/Static.json" );
   mgr::ConfigReader group_cfg( "./data/Groups.json" );

   mgr::SampleMgr::InitStaticFromReader( static_cfg );
   mgr::SampleMgr::SetFilePrefix( GetEDMPrefix() );

   // Defining data settings
   SampleHistMgr* data = new SampleHistMgr( GetDataTag() , group_cfg );

   // Defining out channels see data/MuonSignal.json for sample settings
   vector<SampleHistMgr*>  background;
   background.push_back( new SampleHistMgr("TTJets"     , group_cfg ) );
   background.push_back( new SampleHistMgr("SingleTop"  , group_cfg ) );
   background.push_back( new SampleHistMgr("TTBoson"    , group_cfg ) );
   background.push_back( new SampleHistMgr("SingleBoson", group_cfg ) );
   background.push_back( new SampleHistMgr("DiBoson"    , group_cfg ) );


   // Declaring sample sample
   SampleHistMgr* signal_mgr = new SampleHistMgr( "tstar_M700" , group_cfg );

   // Making combined stack plots
   MakeComparePlot( data, background, signal_mgr );


   // Making normalized plots
   Parameter bg_original_yield(0,0,0);
   for( const auto sample : background ){
      bg_original_yield += sample->ExpectedYield();
   }
   const double bg_scale = 1./bg_original_yield.CentralValue();
   for( auto sample: background){
      sample->Scale(bg_scale);
   }
   signal_mgr->Scale(bg_scale);

   const double data_yield = data->EventsInFile();
   const double data_scale = 1./data_yield;
   data->Scale(data_scale);

   MakeComparePlot( data, background, signal_mgr , "normalized");

   // Cleaning up
   for( auto& histmgr : background ){
      delete histmgr;
   }
   delete data;
   delete signal_mgr;
   return 0;
}

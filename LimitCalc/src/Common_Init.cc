/*******************************************************************************
*
*  Filename    : Common_Init.cc
*  Description : Initialization functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include <vector>

using namespace std;

/*******************************************************************************
*   External variables for this subpackage
*******************************************************************************/
TstarNamer limnamer( "LimitCalc" );

/*******************************************************************************
*   Initialization functions
*******************************************************************************/
void
InitSingle( SampleRooFitMgr*& mgr, const string& tag )
{
   const mgr::ConfigReader cfg( limnamer.MasterConfigFile() );
   mgr = new SampleRooFitMgr( tag, cfg );
}

/******************************************************************************/

void
InitRooFitSettings( const TstarNamer& x )
{
   const mgr::ConfigReader cfg( x.MasterConfigFile() );
   const double mass_min = cfg.GetStaticDouble( "Mass Min" );
   const double mass_max = cfg.GetStaticDouble( "Mass Max" );
   SampleRooFitMgr::InitStaticVars( mass_min, mass_max );
   SampleRooFitMgr::x().setRange( "FitRange", mass_min, mass_max );
}

/******************************************************************************/

void
InitDataAndSignal( SampleRooFitMgr*& data, vector<SampleRooFitMgr*>& sig_list )
{
   const mgr::ConfigReader cfg( limnamer.MasterConfigFile() );
   data = new SampleRooFitMgr( limnamer.GetChannelEXT( "Data Tag" ), cfg );

   for( const auto& signal_tag : cfg.GetStaticStringList( "Signal List" ) ){
      sig_list.push_back( new SampleRooFitMgr( signal_tag, cfg ) );
   }
}

/******************************************************************************/

void
InitMC( SampleRooFitMgr*& mc )
{
   const mgr::ConfigReader cfg( limnamer.MasterConfigFile() );
   mc = new SampleRooFitMgr( "Background", cfg );
}

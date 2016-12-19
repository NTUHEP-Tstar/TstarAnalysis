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

extern const std::vector<std::string> uncsource = {
   "jec",
   "jetres",
   "btag",
   "pu",
   "lep",
   "pdf",
   "scale"
};

/*******************************************************************************
*   Initialization functions
*******************************************************************************/
void
InitSingle( SampleRooFitMgr*& mgr, const string& tag )
{
   const mgr::ConfigReader& cfg = limnamer.MasterConfig();
   mgr = new SampleRooFitMgr( tag, cfg );
}

/******************************************************************************/

void
InitRooFitSettings( const TstarNamer& x )
{
   const mgr::ConfigReader& cfg = x.MasterConfig();
   const double mass_min        = cfg.GetStaticDouble( "Mass Min" );
   const double mass_max        = cfg.GetStaticDouble( "Mass Max" );
   SampleRooFitMgr::InitStaticVars( mass_min, mass_max );
   SampleRooFitMgr::x().setRange( "FitRange", mass_min, mass_max );
}

/******************************************************************************/

void
InitDataAndSignal( SampleRooFitMgr*& data, vector<SampleRooFitMgr*>& siglist )
{
   const mgr::ConfigReader& cfg = limnamer.MasterConfig();
   const string datatag         = limnamer.GetChannelEXT( "Data Prefix" )
                                  + limnamer.GetExtName( "era", "Data Postfix" );

   for( const auto& signaltag : cfg.GetStaticStringList( "Signal List" ) ){
      siglist.push_back( new SampleRooFitMgr( signaltag, cfg ) );
   }

   cout << "Created RooFitMgr for data: " << datatag << endl;
   data = new SampleRooFitMgr( datatag, cfg );

}

/******************************************************************************/

void
InitMC( SampleRooFitMgr*& mc )
{
   const mgr::ConfigReader& cfg = limnamer.MasterConfig();
   mc = new SampleRooFitMgr( "Background", cfg );
}

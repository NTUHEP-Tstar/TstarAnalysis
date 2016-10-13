/*******************************************************************************
*
*  Filename    : GetEventWeight.cc
*  Description : Utility functions for getting event weights
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/EventBase.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"

#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"

#include "TstarAnalysis/Common/interface/ComputeSelectionEff.hpp"

#include "TFile.h"
#include <iostream>
#include <string>
using namespace std;

/*******************************************************************************
*   Static helper functions
*******************************************************************************/
static double GetWeightByTag(
   const fwlite::EventBase& ev,
   const string&            module_label,
   const string&            product_label
   );


/******************************************************************************/

double
GetEventWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "EventWeight", "EventWeight" );
}

/******************************************************************************/

double
GetPileupWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "PileupWeight", "PileupWeight" );
}

double
GetPileupWeightBestFit( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "PileupWeightBestFit", "PileupWeight" );
}

double
GetPileupWeightXsecup( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "PileupWeightXsecup", "PileupWeight" );
}

double
GetPileupWeightXsecdown( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "PileupWeightXsecdown", "PileupWeight" );
}

/******************************************************************************/

double
GetBtagWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "BtagWeight", "BtagWeight" );
}

double
GetBtagWeightUp( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "BtagWeight", "BtagWeightup" );
}

double
GetBtagWeightDown( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "BtagWeight", "BtagWeightdown" );
}


/******************************************************************************/

double
GetElectronWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight", "ElectronWeight" );
}

double
GetElectronCutWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight", "ElectronCutWeight" );
}

double
GetElectronTrackWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight", "ElectronGsfWeight" );
}

double GetElectronTriggerWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight", "ElectronTriggerWeight" );
}

double
GetElectronWeightUp( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight", "ElectronWeightup" );
}

double
GetElectronWeightDown( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight", "ElectronWeightdown" );
}


/******************************************************************************/

double
GetMuonWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "MuonWeight", "MuonWeight" );
}

double
GetMuonWeightUp( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "MuonWeight", "MuonWeightup" );
}

double
GetMuonWeightDown( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "MuonWeight", "MuonWeightdown" );
}

double
GetMuonTriggerWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "MuonWeight", "MuonTriggerWeight" );
}
double
GetMuonIDWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "MuonWeight", "MuonIDWeight" );
}

double
GetMuonIsoWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "MuonWeight", "MuonIsoWeight" );
}

/******************************************************************************/

double
GetEventTopPtWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "TopPtWeight", "TopPtWeight" );
}

double
SetSampleTopPtWeight( mgr::SampleMgr& sample )
{
   fwlite::Handle<double> count_handle;
   double ans = 0;

   try {
      for( const auto& file_path : sample.GlobbedFileList() ){
         fwlite::Run run( TFile::Open( file_path.c_str() ) );

         for( run.toBegin(); !run.atEnd(); ++run ){
            count_handle.getByLabel( run, "TopPtWeight", "TopPtWeightSum" );
            ans += *count_handle;
         }
      }
   } catch( std::exception ){
      if( sample.Name().find( "Run" ) == string::npos ){
         cerr << "Error! Top Pt weight sum not found for MC dataset!" << endl;
      }
      ans = sample.Event().size();// For data files
   }

   sample.AddCacheDouble( "TopPtWeightSum", ans );
   return ans;
}

double
GetSampleTopPtWeight( const mgr::SampleMgr& sample )
{
   return sample.GetCacheDouble( "TopPtWeightSum" );
}

double
GetSampleEventTopPtWeight( const mgr::SampleMgr& sample, const fwlite::EventBase& ev )
{
   const double sample_weightsum   = sample.Event().size();
   const double sample_ptweightsum = GetSampleTopPtWeight( sample );
   const double event_ptweight     = GetEventTopPtWeight( ev );
   return event_ptweight * sample_weightsum / sample_ptweightsum;
}

/*******************************************************************************
*   Helper function
*******************************************************************************/
static double
GetWeightByTag(
   const fwlite::EventBase& ev,
   const string&            module,
   const string&            product )
{
   fwlite::Handle<double> weighthandle;
   static const double maxweight = 10.;
   static const double minweight = -10.;

   // Directly skipping over data events
   if( ev.isRealData() ){ return 1.; }

   try {
      weighthandle.getByLabel( ev, module.c_str(), product.c_str() );
      if( *weighthandle < maxweight && *weighthandle > minweight ){
         return *weighthandle;
      } else {
         return 1.;
      }
   } catch( std::exception e ){
      return 1.;
   }
}

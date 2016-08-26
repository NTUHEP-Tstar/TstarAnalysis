/*******************************************************************************
 *
 *  Filename    : GetEventWeight.cc
 *  Description : Utility functions for getting event weights
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "DataFormats/FWLite/interface/EventBase.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include <iostream>
#include <string>
using namespace std;

//------------------------------------------------------------------------------
//   Declaring static helper function
//------------------------------------------------------------------------------
static double GetWeightByTag(
   const fwlite::EventBase& ev,
   const string& module_label,
   const string& product_label
);

//------------------------------------------------------------------------------
//   Primary functions
//      - assuming you are using the settings in python/EventWeight_cfi.py
//------------------------------------------------------------------------------

double GetEventWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "EventWeight", "EventWeight" );
}

double GetPileupWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "PileupWeight", "PileupWeight" );
}

double GetElectronWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight", "ElectronWeight" );
}

double GetElectronCutWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight", "elecCutWeight" );
}

double GetElectronTrackWeight( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "ElectronWeight" , "elecTrackWeight" );
}


//------------------------------------------------------------------------------
//   Helper functions - all assuming double weights
//------------------------------------------------------------------------------

static double GetWeightByTag(
   const fwlite::EventBase& ev,
   const string& module,
   const string& product)
{
   fwlite::Handle<double> weighthandle;
   static double maxweight =  10.;
   static double minweight = -10.;
   // Directly skipping over data events
   if( ev.isRealData() ) { return 1.; }

   try{
      weighthandle.getByLabel( ev , module.c_str() , product.c_str() );
      if( *weighthandle < maxweight && *weighthandle > minweight ){
         return *weighthandle;
      } else {
         return  1. ;
      }
   } catch ( std::exception e ){
      return 1. ;
   }
}

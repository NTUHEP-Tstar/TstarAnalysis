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
GetPileupWeight71260( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "PileupWeight", "PileupWeight71260" );
}

double
GetPileupWeight62000( const fwlite::EventBase& ev )
{
   return GetWeightByTag( ev, "PileupWeight", "PileupWeight62000" );
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

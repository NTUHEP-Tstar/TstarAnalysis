/*******************************************************************************
*
*  Filename    : RunCombine.cc
*  Description : One Line descrption of file contents
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/SysUtils/interface/HiggsCombineSubmitter.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include <string>
#include <vector>

using namespace std;
using namespace mgr;

/*******************************************************************************
*   Main control flow
*******************************************************************************/
static string AdditionalOptions( const string& method );


/******************************************************************************/

void
RunCombine( const string& hc_opt )
{
   const ConfigReader& cfg = limnamer.MasterConfig();
   const HiggsCombineSubmitter sub( limnamer.SettingsDir() + "higgs_combine_settings.json" );
   vector<CombineRequest> submit_list;

   for( const auto& sig : cfg.GetStaticStringList( "Signal List" ) ){

      const string cardfile  = limnamer.TextFileName( "card", {sig} );
      const string storefile = limnamer.RootFileName( "combine", {sig} );
      const string method    = limnamer.GetInput( "combine" );
      const string addopts   = AdditionalOptions( method );
      const int masspoint    = GetInt( sig );

      submit_list.push_back( CombineRequest(
            cardfile,
            storefile,
            masspoint,
            method,
            addopts
            ) );
   }

   sub.SubmitParallel( submit_list );
}


string
AdditionalOptions( const string& method )
{
   if( method == "HybridNew" ){
      return "--iteration=16";
   }

   return "";
}

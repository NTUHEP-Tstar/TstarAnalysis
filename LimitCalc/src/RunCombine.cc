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

// ------------------------------------------------------------------------------
//   Main control flows
// ------------------------------------------------------------------------------
void
RunCombine( const string& hc_opt )
{
   const ConfigReader cfg( limit_namer.MasterConfigFile() );
   const HiggsCombineSubmitter sub( limit_namer.SettingsDir() + "higgs_combine_settings.json" );
   vector<CombineRequest> submit_list;

   for( const auto& sig : cfg.GetStaticStringList( "Signal List" ) ){
      submit_list.push_back( CombineRequest(
            limit_namer.TextFileName( "card", {sig} ),  // card file
            limit_namer.RootFileName( "combine", {sig} ),// store store
            GetInt( sig ),           // mass point
            limit_namer.GetInput( "combine" )
            ) );
   }

   sub.SubmitParallel( submit_list );
}

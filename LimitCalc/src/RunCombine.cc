/*******************************************************************************
 *
 *  Filename    : RunCombine.cc
 *  Description : One Line descrption of file contents
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *   Additional comments
 *
*******************************************************************************/
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"
#include "ManagerUtils/SysUtils/interface/HiggsCombineSubmitter.hpp"
#include <vector>
#include <string>

using namespace std;
using namespace mgr;

//------------------------------------------------------------------------------
//   Main control flows
//------------------------------------------------------------------------------
void RunCombine( const string& hc_opt )
{
   const ConfigReader&        cfg = limit_namer.MasterConfig();
   const HiggsCombineSubmitter sub( limit_namer.SettingFilePath() + "higgs_combine_settings.json");
   vector<CombineRequest> submit_list;
   for( const auto& sig : cfg.GetStaticStringList("Signal List") ){
      submit_list.push_back( CombineRequest(
         limit_namer.TextFileName( "card", sig ),     //card file
         limit_namer.RootFileName( "combine" , sig ), //store store
         TagTree::GetInt( sig ),              //mass point
         limit_namer.GetCombineMethod()
      ));
   }
   sub.SubmitParallel( submit_list );
}

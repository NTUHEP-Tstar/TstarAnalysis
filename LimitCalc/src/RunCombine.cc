/*******************************************************************************
*
*  Filename    : RunCombine.cc
*  Description : One Line descrption of file contents
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/SysUtils/interface/HiggsCombineSubmitter.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"
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
  const HiggsCombineSubmitter sub( limnamer.SettingsDir() / "higgs_combine_settings.json" );
  vector<CombineRequest> submit_list;

  for( const auto& sig : cfg.GetStaticStringList( "Signal List" ) ){

    const string cardfile  = limnamer.TextFileName( "card", {sig} );
    const string storefile = limnamer.RootFileName( "combine", {sig} );
    const string method    = limnamer.GetInput<string>( "combine" );
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
  
    
  string ans = "";
  if( method == "HybridNew" ){
    ans += "  --iteration=16  ";
  }

  // ans += " --hintMethod=ProfileLikelihood ";
  ans += " --rAbsAcc=0.00000005 ";
  ans += " --rRelAcc=0.0000005  ";

  if (limnamer.CheckInput( "seed"  ) ){
    ans+= ( " --seed " +  boost::lexical_cast<string>( limnamer.GetInput<double>( "seed"  )  ) );
  }
  if (limnamer.CheckInput( "rMin"  ) ){
    ans+= (" --rMin " + boost::lexical_cast<string>( limnamer.GetInput<double>( "rMin"  )  ) );
  }
  if (limnamer.CheckInput( "rMax"  ) ){
    ans+= (" --rMax " + boost::lexical_cast<string>( limnamer.GetInput<double>( "rMax"  )  ) );
  }


  return ans;
}

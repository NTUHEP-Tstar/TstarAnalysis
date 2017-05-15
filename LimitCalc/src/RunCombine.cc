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
static string AdditionalOptions( const string& method, const int mass, const string& channel );


/******************************************************************************/

void
RunCombine( const string& addtag, const string& hc_opt )
{
  const ConfigReader& cfg = limnamer.MasterConfig();
  const HiggsCombineSubmitter sub( limnamer.SettingsDir() / "higgs_combine_settings.json" );
  vector<CombineRequest> submit_list;

  for( const auto& sig : cfg.GetStaticStringList( "Signal List" ) ){

    const string cardfile  = limnamer.TextFileName( "card", sig, addtag );
    const string storefile = limnamer.RootFileName( "combine", sig, addtag );
    const string method    = limnamer.GetInput<string>( "combine" );
    const int masspoint    = GetInt( sig );

    // Getting additional options
    const string addopts   = AdditionalOptions( method, masspoint, limnamer.GetChannel() );

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

/******************************************************************************/

string
AdditionalOptions( const string& method, const int mass, const string& channel )
{
  string ans = "";

  ans += " --hintMethod=ProfileLikelihood ";
  ans += " --rAbsAcc=0.00005 ";
  ans += " --rRelAcc=0.0005  ";
  //ans += " --minimizerAlgo=Minuit2  ";
  ans += " --minimizerStrategy=1   ";
  ans += " --minimizerTolerance=0.001 ";
  // ans += " --newExpected=off ";
  //ans += " --picky ";

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

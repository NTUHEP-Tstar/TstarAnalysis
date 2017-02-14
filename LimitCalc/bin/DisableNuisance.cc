/*******************************************************************************
*
*  Filename    : DisableNuisance.cc
*  Description : Reading existing data cards and disabling a specified nuisance parameters
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/DisableNuisance.hpp"
#include "TstarAnalysis/LimitCalc/interface/Limit.hpp"

using namespace std;
namespace opt = boost::program_options;

/******************************************************************************/

int
main( int argc, char* argv[] )
{
  limnamer.AddOptions( LimitOptions() ).AddOptions( HiggsCombineOptions() );
  limnamer.SetNamingOptions( "fitmethod", "fitfunc", "era" );

  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }

  const vector<string> siglist = limnamer.MasterConfig().GetStaticStringList( "Signal List" );
  map<string,pair<mgr::Parameter,mgr::Parameter>> limitresults;

  for( const auto nuisance : unclist ){

    if( limnamer.CheckInput( "combine" ) && nuisance != "" ){
      for( const auto sig : siglist ){
        MakeNewCard( sig, nuisance );
      }
      RunCombine( nuisance );
    }

    limitresults[nuisance] = MakeLimitPlot( nuisance );

    MakeLimitTable( limitresults );
  }


  return 0;
}

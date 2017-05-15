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
  opt::options_description desc( "Options for DisableNuisance" );
  desc.add_options()
    ( "masspoint", opt::value<string>(), "plot single mass point if specified." )
  ;
  limnamer.AddOptions( LimitOptions() )
  .AddOptions( HiggsCombineOptions() )
  .AddOptions( ExtraCutOptions() )
  .AddOptions( desc );
  limnamer.SetNamingOptions( "fitmethod", "fitfunc", "era" );
  limnamer.AddCutOptions( "smootheff" );

  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  InitSampleStatic( limnamer );

  const vector<string> siglist = limnamer.MasterConfig().GetStaticStringList( "Signal List" );
  map<string, pair<mgr::Parameter, mgr::Parameter> > limitresults;

  for( const auto nuisance : unclist ){

    if( limnamer.CheckInput( "combine" ) && nuisance != "" ){
      for( const auto sig : siglist ){
        MakeNewCard( sig, nuisance );
      }

      RunCombine( nuisance );
    }
    if( !limnamer.CheckInput( "masspoint" ) ){
      limitresults[nuisance] = MakeLimitPlot( nuisance );
    }

  }

  if( !limnamer.CheckInput( "masspoint" ) ){
    MakeLimitTable( limitresults );
  } else {
    MakeDeltaRPlot();
  }


  return 0;
}

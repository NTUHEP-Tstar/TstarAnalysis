/*******************************************************************************
*
*  Filename    : PlotValGenFit.cc
*  Description : Options for plotting ValGenFit
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace opt = boost::program_options;

/******************************************************************************/

int
main( int argc, char* argv[] )
{
  opt::options_description desc( "Options for MakeRooFit" );
  desc.add_options()
    ( "masstag", opt::value<string>()->multitoken(), "plot single mass point if specified." )
  ;

  limnamer
  .AddOptions( LimitOptions() )
  .AddOptions( PseudoExpOptions() )
  .AddOptions( PseudoExpExtraOptions() )
  .AddOptions( desc );
  limnamer.SetNamingOptions( "fitfunc", "era", "bkgtype" );
  // limnamer.AddCutOptions( "forcerho" ); Handled independetly 
  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( CheckPsuedoExpOptions() != mgr::OptNamer::PARSE_SUCESS ){ return 1; }

  InitSampleStatic( limnamer );
  const mgr::ConfigReader& cfg = limnamer.MasterConfig();
  const auto masslist          = cfg.GetStaticStringList( "Signal List" );

  if( limnamer.CheckInput( "masstag" ) ){
    const auto masstag = limnamer.GetInput<string>( "masstag" );
    if( std::find( masslist.begin(), masslist.end(),  masstag ) == masslist.end() ){
      cerr << "masstag [" << masstag << "] is not available!" << endl;
      return 1;
    } else {
      PlotSingleGenFit( masstag );
    }
  } else {
    PlotGenFit( cfg.GetStaticStringList( "Signal List" ) );
  }

  return 0;
}

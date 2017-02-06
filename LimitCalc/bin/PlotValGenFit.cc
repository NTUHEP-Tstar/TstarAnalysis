/*******************************************************************************
*
*  Filename    : PlotValGenFit.cc
*  Description : Options for plotting ValGenFit
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

/******************************************************************************/

int
main( int argc, char* argv[] )
{
  limnamer.AddOptions( LimitOptions() ).AddOptions( PsuedoExpOptions() );
  limnamer.SetNamingOptions( "fitfunc", "era" );
  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( CheckPsuedoExpOptions() != mgr::OptNamer::PARSE_SUCESS ){ return 1; }

  InitSampleStatic( limnamer );

  const mgr::ConfigReader& cfg = limnamer.MasterConfig();
  PlotGenFit( cfg.GetStaticStringList( "Signal List" ) );

  return 0;
}

/*******************************************************************************
*
*  Filename    : RunValGenFit.cc
*  Description : Options parsing for running validation on single mass point
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{
  SampleRooFitMgr* mc     = NULL;
  SampleRooFitMgr* sigmgr = NULL;
  SampleRooFitMgr* data   = NULL;

  opt::options_description desc( "Options for RunValGenFit" );

  desc.add_options()
    ( "num,n", opt::value<int>()->required(), "Run <num> times of gen-pseudo data + fit" )
  ;

  limnamer.AddOptions( LimitOptions() ).AddOptions( PsuedoExpOptions() ).AddOptions( MassOptions() ).AddOptions( desc );
  limnamer.SetNamingOptions( "fitfunc", "era", "bkgtype", "masspoint" );
  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( CheckPsuedoExpOptions() != mgr::OptNamer::PARSE_SUCESS ){ return 1; }

  InitSampleStatic( limnamer );
  InitRooFitSettings( limnamer );
  InitSingle( sigmgr, limnamer.GetInput<string>( "masspoint" ) );
  InitMC( mc );
  InitData( data );

  RunGenFit( mc, sigmgr, data );

  delete mc;
  delete sigmgr;

  return 0;
}

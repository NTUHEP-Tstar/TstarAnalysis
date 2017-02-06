/*******************************************************************************
*
*  Filename    : CompareFitFunc.cc
*  Description : Main Control flow for comparing various background function
*                fitting results ( aimed for MC background and Data Control region)
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/CompareBackground.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{
  SampleRooFitMgr* mgr = NULL;

  opt::options_description desc( "Options for MakeRooFit" );
  desc.add_options()
    ( "channel,c", opt::value<string>(), "Which channel to run" )
    ( "fitfunc,f", opt::value<vector<string> >()->multitoken(), "List of fitting function to use, first on will be taken as central" )
    ( "sample,s", opt::value<string>(), "Which sample to use (Data for background)" )
    ( "era,e", opt::value<string>(), "Which data era to use/scale to" )
  ;

  limnamer.AddOptions( desc );
  limnamer.SetNamingOptions( "sample", "era" );
  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }

  InitSampleStatic( limnamer );
  InitRooFitSettings( limnamer );

  /*******************************************************************************
  *   Initializing Samples (fitting in data sets)
  *******************************************************************************/
  if( limnamer.GetInput<string>( "sample" ) == "Background" ){
    InitSingle( mgr, "Background" );
  } else if( limnamer.GetInput<string>( "sample" ) == "Data" ){
    const string datatag = limnamer.GetChannelEXT( "Data Prefix" )
                           + limnamer.GetExt<string>( "era", "Data Postfix" );
    InitSingle( mgr, datatag );
  } else {
    cerr << "Unrecognized sample! only [Background] and [Data] is supported" << endl;
    return 1;
  }

  /*******************************************************************************
  *   Running fitting and plotting functions
  *******************************************************************************/
  CompareFitFunc( mgr );

  /*******************************************************************************
  *   Cleaning up
  *******************************************************************************/

  delete mgr;

  return 0;
}

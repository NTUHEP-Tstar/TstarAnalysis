/*******************************************************************************
*
*  Filename    : FillHistogram.cc
*  Description : Command for prefill the Histogram files, call each per group
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleTableMgr.hpp"

#include <boost/program_options.hpp>
#include <iostream>
using namespace std;
namespace opt = boost::program_options;

/*******************************************************************************
*   Main control flow
*******************************************************************************/
int
main( int argc, char* argv[] )
{
  opt::options_description desc( "Options for FillHistogram" );
  desc.add_options()
    ( "group,g",   opt::value<string>()->required(), "What group to fill" )
    ( "type,t",    opt::value<string>()->required(), "What type of histrogram to fill" )
  ;

  compnamer.AddOptions( CompareOptions() ); // Defined in src/Common.cc
  compnamer.AddOptions( desc );
  const int parse = compnamer.ParseOptions( argc, argv ); // defined in Compare_Common.hpp
  if( parse == mgr::OptNamer::PARSE_ERROR ){ return 1; }
  if( parse == mgr::OptNamer::PARSE_HELP ){ return 0; }

  /*******************************************************************************
  *   Additional parsing for this program
  *******************************************************************************/
  if( compnamer.GetInput<string>("group") == "Data" && !compnamer.CheckInput("era") ){
    cerr << "Error! filling of group [Data] must have [era] specified!" << endl;
    cerr << compnamer.GetDescription() << endl;
    return 0;
  }

  /*******************************************************************************
  *   Initializing
  *******************************************************************************/
  InitSampleStatic( compnamer );

  const mgr::ConfigReader& master = compnamer.MasterConfig();

  const string tag = ( compnamer.GetInput<string>( "group" ) == "Data" ) ?
                     compnamer.GetChannelEXT( "Data Prefix" )+ compnamer.GetExt<string>( "era", "Data Postfix" ) :
                     compnamer.GetInput<string>( "group" );

  if( compnamer.GetInput<string>( "type" ) == "Err" ){
    SampleErrHistMgr errmgr( tag, master );
    errmgr.FillFromSample();
  } else if( compnamer.GetInput<string>( "type" ) == "Qk" ){
    SampleHistMgr mgr( tag, master );
    mgr.FillFromSample();
  } else if( compnamer.GetInput<string>( "type" ) == "Tb" ){
    SampleTableMgr mgr( tag, master );
    mgr.LoadFromEDM();
  } else {
    cerr << "Unknown histogram type!" << endl;
    return 1;
  }

  return 0;
}

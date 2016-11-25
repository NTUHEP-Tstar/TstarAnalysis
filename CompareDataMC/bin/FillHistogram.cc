/*******************************************************************************
*
*  Filename    : FillHistogram.cc
*  Description : Command for prefill the Histogram files, call each per group
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleTableMgr.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"

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
   opt::options_description desc( "Options for KinematicCompare" );
   desc.add_options()
      ( "channel,c", opt::value<string>(), "What channel to run" )
      ( "group,g",   opt::value<string>(), "What group to fill" )
      ( "type,t",    opt::value<string>(), "What type of histrogram to fill" )
      ( "era,e",     opt::value<string>(), "What era of data to fill" )
   ;

   const int parse = compnamer.LoadOptions( desc, argc, argv );// defined in Compare_Common.hpp
   if( parse == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   if( parse == mgr::OptsNamer::PARSE_HELP ){ return 0; }

   InitSampleStatic( compnamer );

   const mgr::ConfigReader master =  compnamer.MasterConfig();

   const string tag = ( compnamer.InputStr( "group" ) == "Data" ) ?
                      compnamer.GetChannelEXT( "Data Prefix" )+ compnamer.GetExtName( "era", "Data Postfix") :
                      compnamer.InputStr( "group" );

   if( compnamer.InputStr( "type" ) == "Err" ){
      SampleErrHistMgr errmgr( tag, master );
      errmgr.FillFromSample();
   } else if( compnamer.InputStr("type") == "Qk") {
      SampleHistMgr mgr( tag, master );
      mgr.FillFromSample();
   } else if( compnamer.InputStr("type") == "Tb" ){
      SampleTableMgr mgr( tag, master );
      mgr.LoadFromEDM();
   }

   return 0;
}

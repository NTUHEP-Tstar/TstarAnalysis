/*******************************************************************************
 *
 *  Filename    : NameObj.cc
 *  Description : Declaration of global variables and adding initializing function
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include <iostream>

using namespace std;
namespace opt = boost::program_options ;
//------------------------------------------------------------------------------
//   Declaring Global variables
//------------------------------------------------------------------------------
static const string name_file = GetEnv("CMSSW_BASE") + "/src/TstarAnalysis/NameFormat/settings/name_settings.json";
TagTree compare_namer ( name_file, "CompareDataMC" );
TagTree massreco_namer( name_file, "TstarMassReco" );
TagTree limit_namer   ( name_file, "LimitCalc"     );

opt::options_description option_setting;
opt::variables_map       option_input;


//------------------------------------------------------------------------------
//   Option parsing function
//------------------------------------------------------------------------------

int InitOptions( int argc , char* const argv[] )
{
   option_setting.add_options()
      ("help","produce help message and exit")
      ("channel"       , opt::value<string>(), "Channel to run" )
      ("fitmethod"     , opt::value<string>(), "Which fitting method results to plot" )
      ("fitfunc"       , opt::value<string>(), "Which fitting function to to plot")
      ("combine_method", opt::value<string>(), "Which higgs combine method to use" )
      ("higgs_options" , opt::value<string>(), "Additional option for higgs combine package" )
      ("runcombine"    , "Re-run higgs combine package with higgs combine method")
   ;

   try{
      opt::store(opt::parse_command_line(argc, argv, option_setting), option_input);
      opt::notify(option_input);
   } catch( boost::exception& e ){
      cerr << "Error parsing command!" << endl;
      cerr << boost::diagnostic_information(e);
      return PARSE_ERROR;
   }

   if(option_input.count("help")) {
      cerr << option_setting << endl;
      return PARSE_HELP;
   }

   if(option_input.count("channel")){
      compare_namer .SetChannel( option_input["channel"].as<string>() );
      massreco_namer.SetChannel( option_input["channel"].as<string>() );
      limit_namer   .SetChannel( option_input["channel"].as<string>() );
   }

   if( option_input.count("fitmethod") ){
      limit_namer   .SetFitMethod( option_input["fitmethod"].as<string>() );
   }

   if( option_input.count("fitfunc") ){
      massreco_namer.SetFitFunc( option_input["fitfunc"].as<string>() );
      limit_namer   .SetFitFunc( option_input["fitfunc"].as<string>() );
   }

   if( option_input.count("combine_method") ){
      limit_namer.SetCombineMethod( option_input["combine_method"].as<string>() );
   }

   limit_namer.SetTag("fitset", "" );

   return PARSE_SUCESS;
}

void InitSampleSettings( const TagTree& x )
{
   mgr::SampleMgr::InitStaticFromReader( x.MasterConfig() );
   mgr::SampleMgr::SetFilePrefix       ( x.EDMFilePath() );
   mgr::SampleGroup::SetSampleCfgPrefix( x.SettingFilePath() );
}

//------------------------------------------------------------------------------
//   Manditory options functions
//------------------------------------------------------------------------------
int CheckManditory( const vector<string>& list )
{
   for( const auto& opt : list ){
      if( !option_input.count(opt) ){
         cerr << option_setting << endl;
         cerr << "Error! Options [" << opt << "] is mandetory!" << endl;
         return PARSE_ERROR;
      }
   }
   return PARSE_SUCESS;
}

void ShowManditory( const vector<string>& list )
{
   cerr << "Options:" ;
   for( const auto& opt : list ){
      cerr << " [" << opt << "],";
   }
   cerr << " are manditory!" << endl;
}

/*******************************************************************************
 *
 *  Filename    : TstarNamer.cc
 *  Description : Implementation of TstarNamer class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include <iostream>
using namespace std;
using namespace mgr;
namespace opt = boost::program_options;


//------------------------------------------------------------------------------
//   Constructor & Destructor
//------------------------------------------------------------------------------
TstarNamer::TstarNamer( const string& sub_package ):
   PackagePathMgr( "TstarAnalysis" , sub_package ),
   OptsNamer()
{

}

TstarNamer::~TstarNamer()
{

}

//------------------------------------------------------------------------------
//   Setting and file paths
//------------------------------------------------------------------------------
string TstarNamer::MasterConfigFile() const
{ return  SettingsDir() + "master.json"; }

//------------------------------------------------------------------------------
//   Channel specialization part
//------------------------------------------------------------------------------
int TstarNamer::LoadOptions( const opt::options_description& desc, int argc, char* argv[] )
{
   int parse_result = OptsNamer::LoadOptions(desc,argc,argv);
   if( parse_result == OptsNamer::PARSE_ERROR ){ return OptsNamer::PARSE_ERROR; }
   if( parse_result == OptsNamer::PARSE_HELP  ){ return OptsNamer::PARSE_HELP;  }
   if( GetMap().count("channel") ){
      _channel_tag = GetMap()["channel"].as<string>();
   }

   for( const auto& option : _naming_option_list ){
      if( !GetMap().count(option) ){
         cerr << "Missing option [" << option  << "]" << endl;
         cerr << desc << endl;
         return OptsNamer::PARSE_ERROR;
      }
   }
   LoadJsonFile( SettingsDir()+"name_settings.json" );
   return PARSE_SUCESS;
}

void TstarNamer::SetChannel( const std::string& x )
{ _channel_tag = x; }

string TstarNamer::GetChannelEDMPath() const
{ return SamplesDir() + GetChannel() + "/"; }

string TstarNamer::GetChannelEXT( const string& x ) const
{ return query_tree("channel",GetChannel(),x); }


//------------------------------------------------------------------------------
//   File name functions
//------------------------------------------------------------------------------
string TstarNamer::MakeFileName(
   const string& extension,
   const string& main_tag,
   const string& sub_tag
) const
{
   string ans = ResultsDir() + GetChannel() + "/" + main_tag ;
   for( const auto& option : _naming_option_list ){
      if( GetInput(option) != "" ){ ans += "_"+GetInput(option); }
   }
   if( sub_tag != "" )            { ans += "_" + sub_tag ; }
   ans += "." + extension;
   return ans;
}

string TstarNamer::TextFileName( const string& main_tag, const string& sub_tag ) const
{ return MakeFileName("txt",main_tag,sub_tag); }

string TstarNamer::PlotFileName( const string& main_tag, const string& sub_tag ) const
{ return MakeFileName( "png", main_tag, sub_tag ); }

string TstarNamer::TexFileName( const string& main_tag, const string& sub_tag ) const
{ return MakeFileName("tex",main_tag,sub_tag); }

string TstarNamer::RootFileName( const string& main_tag, const string& sub_tag ) const
{ return MakeFileName("root",main_tag,sub_tag); }

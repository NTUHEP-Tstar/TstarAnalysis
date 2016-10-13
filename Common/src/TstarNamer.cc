/*******************************************************************************
*
*  Filename    : TstarNamer.cc
*  Description : Implementation of TstarNamer class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <iostream>
using namespace std;
using namespace mgr;
namespace opt = boost::program_options;


/*******************************************************************************
*   Constructor and Destructor
*******************************************************************************/
TstarNamer::TstarNamer( const string& sub_package ) :
   PackagePathMgr( "TstarAnalysis", sub_package ),
   OptsNamer()
{

}

TstarNamer::~TstarNamer()
{
}

/*******************************************************************************
*   File paths functions
*******************************************************************************/
string
TstarNamer::MasterConfigFile() const
{
   return SettingsDir() + "master.json";
}

/*******************************************************************************
*   Channel specialization part
*******************************************************************************/
int
TstarNamer::LoadOptions(
   const opt::options_description& desc,
   int                             argc,
   char*                           argv[] )
{
   int parse_result = OptsNamer::LoadOptions( desc, argc, argv );
   if( parse_result == OptsNamer::PARSE_ERROR ){
      return OptsNamer::PARSE_ERROR;
   } else if( parse_result == OptsNamer::PARSE_HELP  ){
      return OptsNamer::PARSE_HELP;
   }

   if( GetMap().count( "channel" ) ){
      _channel_tag = GetMap()["channel"].as<string>();
   }

   for( const auto& option : _naming_option_list ){
      if( !GetMap().count( option ) ){
         cerr << "Missing option [" << option  << "]" << endl;
         cerr << desc << endl;
         return OptsNamer::PARSE_ERROR;
      }
   }

   LoadJsonFile( SettingsDir()+"name_settings.json" );

   return PARSE_SUCESS;
}

void
TstarNamer::SetChannel( const std::string& x )
{ _channel_tag = x; }

string
TstarNamer::GetChannelEDMPath() const
{ return SamplesDir() + GetChannel() + "/"; }

string
TstarNamer::GetChannelEXT( const string& x ) const
{ return query_tree( "channel", GetChannel(), x ); }


/*******************************************************************************
*   File Name functions
*******************************************************************************/
string
TstarNamer::CustomFileName(
   const string&         extension,
   const vector<string>& taglist
   ) const
{
   vector<string> mytaglist = taglist;// removing empty tags
   static string empty      = "";
   mytaglist.erase( remove( mytaglist.begin(), mytaglist.end(), empty ), mytaglist.end() );
   string ans = ResultsDir() + GetChannel() + '/';
   ans += boost::join( mytaglist, "_" );

   if( extension != "" ){
      ans += "." + extension;
   }
   return ans;
}


/******************************************************************************/


string
TstarNamer::MakeFileName(
   const string&         extension,
   const string&         main_tag,
   const vector<string>& subtaglist
   ) const
{
   vector<string> taglist;
   taglist.push_back( main_tag );

   for( const auto opt : _naming_option_list ){
      taglist.push_back( InputStr( opt ) );
   }

   taglist.insert( taglist.end(), subtaglist.begin(), subtaglist.end() );
   return CustomFileName( extension, taglist );
}

/******************************************************************************/

string
TstarNamer::TextFileName( const string&         maintag,
                          const vector<string>& subtaglist ) const
{ return MakeFileName( "txt", maintag, subtaglist ); }

string
TstarNamer::PlotFileName( const string&         maintag,
                          const vector<string>& subtaglist ) const
{ return MakeFileName( "pdf", maintag, subtaglist ); }

string
TstarNamer::TexFileName( const string&         maintag,
                         const vector<string>& subtaglist ) const
{ return MakeFileName( "tex", maintag, subtaglist ); }

string
TstarNamer::RootFileName( const string&         maintag,
                          const vector<string>& subtaglist ) const
{ return MakeFileName( "root", maintag, subtaglist ); }

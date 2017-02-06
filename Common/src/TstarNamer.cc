/*******************************************************************************
*
*  Filename    : TstarNamer.cc
*  Description : Implementation of TstarNamer class
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Common/interface/STLUtils.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
using namespace std;
using namespace mgr;
namespace opt = boost::program_options;


/*******************************************************************************
*   Constructor and Destructor
*******************************************************************************/
TstarNamer::TstarNamer( const string& sub_package ) :
  PackagePathMgr( "TstarAnalysis", sub_package ),
  OptNamer( SettingsDir()/"name_settings.json" ),
  _masterconfig( SettingsDir() / "master.json" )
{
}

TstarNamer::~TstarNamer()
{
}

/*******************************************************************************
*   Channel specialization part
*******************************************************************************/
int
TstarNamer::ParseOptions(
  int   argc,
  char* argv[] )
{
  int parse_result = OptNamer::ParseOptions( argc, argv );
  if( parse_result != OptNamer::PARSE_SUCESS ){
    return parse_result;
  }

  if( CheckInput( "channel" ) ){
    _channel = GetInput<string>( "channel" );
  }

  for( const auto& option : _namingoptionlist ){
    if( !CheckInput( option ) ){
      cerr << "Missing option [" << option  << "]" << endl;
      cerr << Description() << endl;
      return OptNamer::PARSE_ERROR;
    }
  }

  return OptNamer::PARSE_SUCESS;
}

/******************************************************************************/

const string&
TstarNamer::GetChannel() const
{
  return _channel;
}


/******************************************************************************/

void
TstarNamer::SetChannel( const std::string& x )
{
  _channel = x;
}

/******************************************************************************/

string
TstarNamer::GetChannelEDMPath() const
{
  return GetChannelEXT( "EDM path" );
}

/******************************************************************************/

string
TstarNamer::GetChannelEXT( const string& x ) const
{
  return ExtQuery<string>( "channel", GetChannel(), x );
}


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
  string ans = ResultsDir() / GetChannel()  / boost::join( mytaglist, "_" );

  if( extension != "" ){
    ans += "." + extension;
  }
  return ans;
}

/******************************************************************************/

string
TstarNamer::OptFileName(
  const string&         extension,
  const string&         main_tag,
  const vector<string>& subtaglist
  ) const
{
  vector<string> taglist;
  taglist.push_back( main_tag );

  for( const auto opt : _namingoptionlist ){
    string tag = "";

    if( tag == "" ){
      try {
        tag = GetInput<string>( opt );
      } catch( ... ){
      }
    }

    if( tag == "" ){
      try {
        tag = boost::lexical_cast<string>( GetInput<int>( opt ) );
      } catch( ... ){
      }
    }

    if( tag == "" ){
      try {
        tag = boost::lexical_cast<string>( GetInput<double>( opt ) );
      } catch( ... ){
      }
    }

    taglist.push_back( tag );
  }

  taglist.insert( taglist.end(), subtaglist.begin(), subtaglist.end() );
  return CustomFileName( extension, taglist );
}

/******************************************************************************/

string
TstarNamer::TextFileName(
  const string&         maintag,
  const vector<string>& subtaglist ) const
{
  return OptFileName( "txt", maintag, subtaglist );
}

string
TstarNamer::PlotFileName(
  const string&         maintag,
  const vector<string>& subtaglist ) const
{
  return OptFileName( "pdf", maintag, subtaglist );
}

string
TstarNamer::TexFileName(
  const string&         maintag,
  const vector<string>& subtaglist ) const
{
  return OptFileName( "tex", maintag, subtaglist );
}

string
TstarNamer::RootFileName(
  const string&         maintag,
  const vector<string>& subtaglist ) const
{
  return OptFileName( "root", maintag, subtaglist );
}

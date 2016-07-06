/*******************************************************************************
 *
 *  Filename    : TagTree.cc
 *  Description : Implementation of TagTree class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/NameFormat/interface/TagTree.hpp"
#include "ManagerUtils/SysUtils/interface/FilenameMgr.hpp"

#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace mgr;

//------------------------------------------------------------------------------
//   Constructor & Destructor
//------------------------------------------------------------------------------
TagTree::TagTree( const string& config_file, const string& sub_package ):
   FilenameMgr( "TstarAnalysis" , sub_package ),
   _channel_tag(""),
   _fit_method_tag(""),
   _fit_func_tag(""),
   _combine_method_tag(""),
   _master_config( SettingFilePath() + "master.json" )
{
   read_json( config_file, _tree );
}

TagTree::~TagTree()
{

}

//------------------------------------------------------------------------------
//   Static Helper function
//------------------------------------------------------------------------------
int TagTree::GetInt( const string& x )
{
   string ans_string;
   for( auto y : x ){
      if( y >= '0' && y <= '9'){
         ans_string.push_back(y);
      }
   }
   return stoi(ans_string);
}

//------------------------------------------------------------------------------
//   Setting and file paths
//------------------------------------------------------------------------------
const ConfigReader&  TagTree::MasterConfig() const
{ return _master_config; }

const string  TagTree::SettingFilePath() const
{ return SubPackageDir() + "settings/"; }

const string TagTree::EDMFilePath() const
{ return SubPackageDir() + "sample/" + GetChannel() + "/"; }


//------------------------------------------------------------------------------
//   Tree reading functions
//------------------------------------------------------------------------------
string TagTree::GetChannelLatex() const
{ return _tree.get<string>("Channel."+_channel_tag+".Latex Name");  }
string TagTree::GetChannelRoot() const
{ return _tree.get<string>("Channel."+_channel_tag+".Root Name");  }
string TagTree::GetChannelDataTag() const
{ return _tree.get<string>("Channel."+_channel_tag+".Data Tag");  }

string TagTree::GetFitMethodFull() const
{ return _tree.get<string>("Fit Method."+_fit_method_tag+".Latex Name");  }

string TagTree::GetFitFuncFull() const
{ return _tree.get<string>("FitFunc."+_fit_func_tag+".Full Name");  }
string TagTree::GetFitFuncLatex() const
{ return _tree.get<string>("FitFunc."+_fit_func_tag+".Latex Name");  }
string TagTree::GetFitFuncRoot() const
{ return _tree.get<string>("FitFunc."+_fit_func_tag+".Root Name");  }

string TagTree::GetCombineMethodFull() const
{ return _tree.get<string>("Combine."+_combine_method_tag+".Full Name");  }


//------------------------------------------------------------------------------
//   File name functions
//------------------------------------------------------------------------------
string TagTree::MakeFileName(
   const string& extension,
   const string& main_tag,
   const string& sub_tag
) const
{
   string ans = ResultsDir() + GetChannel() + "/" + main_tag ;
   if( GetFitMethod() != "" )     { ans += "_" + GetFitMethod(); }
   if( GetFitFunc() != "" )       { ans += "_" + GetFitFunc(); }
   if( GetCombineMethod() != "" && extension !="txt" ) { ans += "_" + GetCombineMethod(); }
   if( sub_tag != "" )            { ans += "_" + sub_tag ; }
   ans += "." + extension;
   return ans;
}

string TagTree::TextFileName( const string& main_tag, const string& sub_tag ) const
{ return MakeFileName("txt",main_tag,sub_tag); }

string TagTree::PlotFileName( const string& main_tag, const string& sub_tag ) const
{ return MakeFileName( img_extension(), main_tag, sub_tag ); }

string TagTree::TexFileName( const string& main_tag, const string& sub_tag ) const
{ return MakeFileName("tex",main_tag,sub_tag); }

string TagTree::RootFileName( const string& main_tag, const string& sub_tag ) const
{ return MakeFileName("root",main_tag,sub_tag); }


string TagTree::img_extension() const
{ return _tree.get<string>("Image format"); }

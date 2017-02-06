/*******************************************************************************
 *
 *  Filename    : namer_test.cc
 *  Description : Testing TstarNamer class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

using namespace std;
namespace opt = boost::program_options;

int main(int argc, char* argv[])
{
   opt::options_description  settings( "testing" );
   settings.add_options()
      ("channel,c"  , opt::value<string>() , "which channel to run" )
      ("fitmethod,m", opt::value<string>() , "option a")
      ("fitfunc,f"  , opt::value<string>() , "option b")
      ("combine,p",   opt::value<string>() , "option to skip")
   ;

   TstarNamer my_name("Common");
   my_name.SetNamingOptions( {"fitmethod","fitfunc"});
   my_name.AddOptions( settings );
   if( my_name.ParseOptions( argc, argv ) ) { return 1; }

   cout << my_name.GetChannel() << endl;
   cout << my_name.GetChannelEDMPath() << endl;
   cout << my_name.GetChannelEXT("Data Tag") << endl;
   cout << my_name.GetInput<string>("fitmethod") << endl;
   cout << my_name.GetExt<string>("fitfunc","Full Name") << endl;
   cout << my_name.GetInput<string>("combine") << endl;
   cout << my_name.TextFileName("test",{"subtest"}) << endl;


   return 0;
}

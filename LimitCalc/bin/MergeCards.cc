/*******************************************************************************
 *
 *  Filename    : MergeCards.cc
 *  Description : Calling functions to merge datacards together
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include <boost/program_options.hpp>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iostream>

using namespace std;
namespace opt = boost::program_options;

char buffer[65536];

int main(int argc, char* argv[])
{
   // const vector<string> manditory = {"fitmethod","fitfunc"};
   // const int run = InitOptions( argc, argv );
   // if( run == PARSE_HELP ){ ShowManditory( manditory ); return 0; }
   // if( run == PARSE_ERROR ){ return 1; }
   // if( CheckManditory(manditory)!=PARSE_SUCESS ){ return 1 ; }

   // const mgr::ConfigReader& cfg = limit_namer.MasterConfig();
   // const vector<string> masspoint_list = cfg.GetStaticStringList("Signal List");


   // for( auto& masspoint : masspoint_list ){
   //    limit_namer.SetChannel("MuonSignal");
   //    const string muonfile = limit_namer.TextFileName( "card", masspoint );
   //    limit_namer.SetChannel("ElectronSignal");
   //    const string elecfile = limit_namer.TextFileName( "card", masspoint );
   //    limit_namer.SetChannel("SignalMerge");
   //    const string combfile = limit_namer.TextFileName( "card", masspoint );

   //    sprintf( buffer, "cp %s temp1.txt && cp %s temp2.txt" ,
   //       muonfile.c_str(),
   //       elecfile.c_str()
   //    );
   //    system( buffer );

   //    sprintf( buffer , "combineCards.py  %s %s > %s",
   //         "temp1.txt",
   //         "temp2.txt",
   //         combfile.c_str() );
   //    system( buffer );

   //    system("rm temp1.txt temp2.txt");
   // }

   // return 0;
}

/*******************************************************************************
 *
 *  Filename    : MergeCards.cc
 *  Description : Calling functions to merge datacards together
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/

#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"

#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

using namespace std;
namespace opt = boost::program_options;

char buffer[65536];

int main(int argc, char* argv[]) {

   mgr::ConfigReader cfg( "./data/Groups.json" );
   vector<string> masspoint_list = cfg.GetStaticStringList("Signal List");

   opt::options_description desc("Merge options");
   opt::variables_map       vm;

   desc.add_options()
      ("help","produce help message and exit")
      ("method" , opt::value<string>(), "Which fitting method to run" )
      ("fitfunc", opt::value<string>(), "Which fitting function to use")
   ;

   try{
      opt::store(opt::parse_command_line(argc, argv, desc), vm);
      opt::notify(vm);
   } catch( boost::exception& e ){
      cerr << "Error parsing command!" << endl;
      cerr << boost::diagnostic_information(e);
      return 1;
   }

   if(vm.count("help")) {
      cerr << desc << endl;
      return 0;
   }

   if( !vm.count("method") ){
      cerr << "options [method] not found!" << endl;
      cerr << desc << endl;
      return 1;
   } else {
      SetMethod( vm["method"].as<string>() );
   }

   if( !vm.count("fitfunc") ) {
      cerr << "option [fitfunc] not found!" << endl;
      cerr << desc << endl;
      return 1;
   } else {
      SetFitFunc( vm["fitfunc"].as<string>() );
   }

   SetChannelType("SignalMerge");
   sprintf( buffer, "mkdir -p %s" , ResultsDir().c_str() );
   system( buffer );

   for( auto& masspoint : masspoint_list ){
      SetChannelType("MuonSignal");
      const string muonfile = GetCardFile( masspoint );
      SetChannelType("ElectronSignal");
      const string elecfile = GetCardFile( masspoint );
      SetChannelType("SignalMerge");
      const string combfile = GetCardFile( masspoint );

      sprintf( buffer, "cp %s temp1.txt && cp %s temp2.txt" ,
         muonfile.c_str(),
         elecfile.c_str()
      );
      system( buffer );

      sprintf( buffer , "combineCards.py  %s %s > %s",
           "temp1.txt",
           "temp2.txt",
           combfile.c_str() );
      system( buffer );

      system("rm temp1.txt temp2.txt");
   }

   return 0;
}

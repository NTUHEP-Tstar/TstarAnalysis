/*******************************************************************************
 *
 *  Filename    : PlotLimit.cc
 *  Description : Plot limit main control flow
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include <string>
#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"

using namespace std;
namespace opt = boost::program_options;

//------------------------------------------------------------------------------
//   External functions
//------------------------------------------------------------------------------
extern void RunCombine( const string& higgs_opt="" ); // src/RunCombine.cc
extern void MakeLimitPlot(); // src/PlotLimit.cc

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
int main(int argc, char* argv[]) {

   opt::options_description desc("MakeRooFit options");
   opt::variables_map       vm;

   desc.add_options()
      ("help","produce help message and exit")
      ("channel"       , opt::value<string>(), "Channel to run" )
      ("method"        , opt::value<string>(), "Which fitting method results to plot" )
      ("fitfunc"       , opt::value<string>(), "Which fitting function to to plot")
      ("combine_method", opt::value<string>(), "Which higgs combine method to use" )
      ("higgs_options" , opt::value<string>(), "Additional option for higgs combine package" )
      ("runcombine"    , "Re-run higgs combine package with higgs combine method")
   ;

   // Parsing command
   try{
      opt::store(opt::parse_command_line(argc, argv, desc), vm);
      opt::notify(vm);
   } catch( boost::exception& e ){
      cerr << "Error parsing command!" << endl;
      cerr << boost::diagnostic_information(e) << endl;
      cerr << desc << endl;
      return 1;
   }

   if(vm.count("help")) {
      cerr << desc << endl;
      return 0;
   }

   if( !vm.count("channel") ){
      cerr << "Error! [channel] not set!" << endl;
      cerr << desc << endl;
      return 1;
   }
   SetChannelType( vm["channel"].as<string>() );

   if( !vm.count("method") ){
      cerr << "Error! [method] not set!" << endl;
      cerr << desc << endl;
      return 1;
   }
   SetMethod( vm["method"].as<string>() );

   if( !vm.count("fitfunc") ){
      cerr << "Error! [method] not set!" << endl;
      cerr << desc << endl;
      return 1;
   }
   SetFitFunc( vm["fitfunc"].as<string>() );

   if( !vm.count("combine_method") ){
      cerr << "Error! [combine_method] not set!" << endl;
      cerr << desc << endl;
      return 1;
   }
   SetHCMethod( vm["combine_method"].as<string>() );

   if( vm.count("runcombine") ){
      cout << "Rerunning results higgs combine package with method [" << vm["combine_method"].as<string>() << "] !" << endl;
      if( vm.count("higgs_options") ){
         RunCombine( vm["higgs_options"].as<string>() );
      } else {
         RunCombine();
      }
   }

   cout << "Making limit plot" << endl;
   MakeLimitPlot();

   return 0;
}

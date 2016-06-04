/*******************************************************************************
 *
 *  Filename    : CompareFitPar.cc
 *  Description : Making plots to compare fitting results.cc
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"

#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

using namespace std;
namespace opt = boost::program_options;

//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
extern void MakeFitParPlots( const unsigned ); // see src/FitParPlot.cc

int main(int argc, char* argv[])
{
   opt::options_description desc("MakeRooFit options");
   opt::variables_map       vm;

   desc.add_options()
      ("help","produce help message and exit")
      ("channel", opt::value<string>(), "Channel to run" )
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

   if( vm.count("channel") ){
      SetChannelType( vm["channel"].as<string>() );
   } else {
      cerr << "Settings for option [channel] not found!" << endl;
      cerr << desc << endl;
      return 1;
   }
   if( !vm.count("fitfunc") ) {
      cerr << "option [fitfunc] not found!" << endl;
      cerr << desc << endl;
      return 1;
   } else {
      SetFitFunc( vm["fitfunc"].as<string>() );
   }

   if( GetFitFunc() == "Fermi" ){
      MakeFitParPlots(2); // Two floating variables
   } else if( GetFitFunc() == "Exo" ) {
      MakeFitParPlots(3); // 3 Floating variables
   }

   return 0;
}

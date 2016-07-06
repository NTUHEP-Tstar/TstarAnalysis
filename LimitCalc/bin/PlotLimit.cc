/*******************************************************************************
 *
 *  Filename    : PlotLimit.cc
 *  Description : Plot limit main control flow
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"
#include <iostream>
#include <string>


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

   const int run = InitOptions(argc,argv);
   if( run == PARSE_HELP ){
      cout << "Options [channel], [method], [fitfunc], [combine_method] are manditory!" << endl;
      return 0;
   } else if( run == PARSE_ERROR ){
      return 1;
   }

   if( option_input.count("runcombine") ){
      cout << "Rerunning results higgs combine package with method ["
           << limit_namer.GetCombineMethod() << "] !" << endl;
      RunCombine();
      // RunCombine( vm["higgs_options"].as<string>() );
   }

   cout << "Making limit plot" << endl;
   MakeLimitPlot();

   return 0;
}

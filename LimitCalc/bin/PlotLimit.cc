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
int main(int argc, char* argv[])
{
   const vector<string> manditory = {"channel","fitmethod","fitfunc","combine_method"};
   const int run = InitOptions( argc, argv );
   if( run == PARSE_HELP  ){ ShowManditory( manditory ); return 0; }
   if( run == PARSE_ERROR ){ return 1; }
   if( CheckManditory( manditory ) != PARSE_SUCESS) { return 1; }

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

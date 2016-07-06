/*******************************************************************************
 *
 *  Filename    : CompareFitPar.cc
 *  Description : Making plots to compare fitting results.cc
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"
#include <iostream>

using namespace std;
namespace opt = boost::program_options;

//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
extern void MakeFitParPlots( const unsigned ); // see src/FitParPlot.cc

int main(int argc, char* argv[])
{
   const int run = InitOptions(argc,argv);
   if( run == PARSE_HELP ){
      cout << "Options [channel], [fitfunc] are manditory!" << endl;
      return 0 ;
   } else if( run == PARSE_ERROR ){
      return 1 ;
   }

   if( limit_namer.GetFitFunc() == "Fermi" ){
      MakeFitParPlots(2); // Two floating variables
   } else if( limit_namer.GetFitFunc() == "Exo" ) {
      MakeFitParPlots(3); // 3 Floating variables
   }

   return 0;
}

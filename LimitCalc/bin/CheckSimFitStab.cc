/*******************************************************************************
 *
 *  Filename    : CheckSimFitStab
 *  Description : Checking the stability of the SimFit Algorithm
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

using namespace std;
namespace opt = boost::program_options;

int main(int argc, char* argv[]) {

   SampleRooFitMgr*   data = NULL ;
   SampleRooFitMgr*   mc   = NULL ;
   vector<SampleRooFitMgr*>   siglist;

   opt::options_description  desc("Options for SimFitCheck");

   desc.add_options()
      ("channel,c" , opt::value<string>(), "which channel to run" )
      ("fitfunc,f" , opt::value<string>(), "which fit function to use")
      ("run,r"     , opt::value<int>()   , "How many times ro generate datasets" )
      ("relmag,x"  , opt::value<double>(), "Relative magnitude of signal compared with prediction")
      ("absmag,a"  , opt::value<double>(), "Absolute magnitude of signal (number of events)")
   ;

   limit_namer.SetNamingOptions({"fitfunc"});
   const int run = limit_namer.LoadOptions( desc, argc, argv );
   if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }
   if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }

   if( limit_namer.GetMap().count("relmag") && limit_namer.GetMap().count("absmag") ){
      cerr << "Cannot set both relative and absolute magnitued" << endl;
      return 1;
   } else if( !limit_namer.GetMap().count("relmag") && !limit_namer.GetMap().count("absmag") ){
      cerr << "Must set either relative of absolute magnitude" << endl;
      return 1;
   }

   InitSampleStatic( limit_namer );
   InitRooFitSettings( limit_namer );
   InitDataAndSignal( data, siglist );
   InitMC( mc );

   bias::MakePsuedoData( mc , data->OriginalDataSet()->sumEntries() );

   RunValidate( mc, siglist );

   SampleRooFitMgr::ClearRooVars();
   return 0;
}
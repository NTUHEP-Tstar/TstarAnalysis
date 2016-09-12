/*******************************************************************************
 *
 *  Filename    : MakeRooFit.cc
 *  Description : Making RooFit objects to be passed over to Higgs Combine
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/Template.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFit.hpp"
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace opt = boost::program_options;

int main(int argc, char* argv[])
{
   SampleRooFitMgr* data = NULL ;
   SampleRooFitMgr* mc   = NULL ;
   vector<SampleRooFitMgr*> signal_list;

   opt::options_description desc("Options for MakeRooFit");
   desc.add_options()
      ("channel,c"  , opt::value<string>() , "Which channel to run" )
      ("fitmethod,m", opt::value<string>() , "Which fitting method to use" )
      ("fitfunc,f"  , opt::value<string>() , "Which fitting function to use" )
   ;

   limit_namer.SetNamingOptions( {"fitmethod","fitfunc"} );
   const int run = limit_namer.LoadOptions(desc,argc,argv);
   if( run == mgr::OptsNamer::PARSE_HELP  ){ return 0; }
   if( run == mgr::OptsNamer::PARSE_ERROR ){ return 1; }

   InitSampleStatic( limit_namer );
   InitRooFitSettings( limit_namer );

   InitDataAndSignal( data, signal_list );
   InitMC(mc);
   if( limit_namer.GetInput("fitmethod") == "SimFit"  ){
      cout  << "Running SimFit Method!" << endl;
      MakeSimFit(data,mc,signal_list);
   } else if( limit_namer.GetInput("fitmethod") == "Template" ){
      cout << "Running MC template method!" << endl;
      MakeTemplate(data,mc,signal_list);
   } else if( limit_namer.GetInput("fitmethod").find("Bias") != string::npos ){
      cout << "Running Bias check!" << endl;
      MakeBias(data,mc,signal_list);
   } else {
      cerr << "Unrecognized method!" << endl;
      return 1;
   }

   delete data;
   delete mc;
   for( auto& sig : signal_list ){ delete sig; }

   return 0;
}

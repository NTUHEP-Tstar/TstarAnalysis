/*******************************************************************************
 *
 *  Filename    : MakeRooFit.cc
 *  Description : Making RooFit objects to be passed over to Higgs Combine
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace opt = boost::program_options;

//------------------------------------------------------------------------------
//   Static data members
//------------------------------------------------------------------------------
static SampleRooFitMgr* data;
static SampleRooFitMgr* mc;
static vector<SampleRooFitMgr*> signal_list;

//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
void InitDataAndSignal();
void InitMC();

// in src/RooFit_SimFit.cc
extern void MakeSimFit(SampleRooFitMgr*, vector<SampleRooFitMgr*>&);

// in src/RooFit_SimFitMC.cc
extern void MakeSimFitMC(SampleRooFitMgr*, SampleRooFitMgr*, vector<SampleRooFitMgr*>& );

// in src/RooFit_MCTemplate.cc
extern void MakeTemplate(SampleRooFitMgr*,SampleRooFitMgr*,vector<SampleRooFitMgr*>& );

// in src/RooFit_Bias.cc
extern void MakeBias(SampleRooFitMgr*,SampleRooFitMgr*,vector<SampleRooFitMgr*>&);

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
   const vector<string> manditory = {"channel","fitmethod","fitfunc"};
   const int run = InitOptions( argc, argv );
   if( run == PARSE_HELP  ){ ShowManditory( manditory ); return 0; }
   if( run == PARSE_ERROR ){ return 1; }
   if( CheckManditory( manditory ) != PARSE_SUCESS) { return 1; }

   InitSampleSettings( limit_namer );

   SampleRooFitMgr::x().setRange("FitRange",SampleRooFitMgr::MinFitMass(),SampleRooFitMgr::MaxMass());

   if( limit_namer.GetFitMethod() == "SimFit"  ){
      cout  << "Running SimFit Method!" << endl;
      InitDataAndSignal();
      MakeSimFit(data,signal_list);
   } else if( limit_namer.GetFitMethod() == "Template" ){
      cout << "Running MC template method!" << endl;
      InitDataAndSignal();
      InitMC();
      MakeTemplate(data,mc,signal_list);
   } else if( limit_namer.GetFitMethod().find("Bias") != string::npos ){
      cout << "Running Bias check!" << endl;
      InitDataAndSignal();
      InitMC();
      MakeBias(data,mc,signal_list);
   } else if( limit_namer.GetFitMethod() == "SimFitMC"){
      cout << "Running SimFit with MC shape fixing!" << endl;
      // InitDataAndSignal();
      // InitMC();
      // MakeSimFitMC(data,mc,signal_list);
   } else {
      cerr << "Unrecognized method!" << endl;
      return 1;
   }

   return 0;
}


//------------------------------------------------------------------------------
//   Helper function implementations
//------------------------------------------------------------------------------
void InitDataAndSignal()
{
   const mgr::ConfigReader& cfg = limit_namer.MasterConfig();

   data = new SampleRooFitMgr( limit_namer.GetChannelDataTag() , cfg );
   data->MakeReduceDataSet("FitRange",RooFit::CutRange("FitRange"));

   for( const auto& signal_tag : cfg.GetStaticStringList("Signal List") ){
      signal_list.push_back( new SampleRooFitMgr( signal_tag, cfg ) );
   }
}

void InitMC()
{
   const mgr::ConfigReader& cfg = limit_namer.MasterConfig();
   mc = new SampleRooFitMgr( "Background", cfg );
}

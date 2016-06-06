/*******************************************************************************
 *
 *  Filename    : MakeRooFit.cc
 *  Description : Making RooFit objects to be passed over to Higgs Combine
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleRooFitMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include <string>
#include <vector>
#include <iostream>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/program_options.hpp>

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

// in src/RooFit_SideBand.cc
extern void MakeCheckPlot(SampleRooFitMgr*);
extern void MakeSideBand(SampleRooFitMgr*,SampleRooFitMgr*);

// in src/RooFit_MCTemplate.cc
extern void MakeTemplate(SampleRooFitMgr*,SampleRooFitMgr*,vector<SampleRooFitMgr*>& );

// in src/RooFit_Bias.cc
extern void MakeBias(SampleRooFitMgr*,SampleRooFitMgr*,vector<SampleRooFitMgr*>&);

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------

int main(int argc, char* argv[]) {

   opt::options_description desc("MakeRooFit options");
   opt::variables_map       vm;

   desc.add_options()
      ("help","produce help message and exit")
      ("channel", opt::value<string>(), "Channel to run" )
      ("method" , opt::value<string>(), "Which fitting method to run" )
      ("fitfunc", opt::value<string>(), "Which fitting function to use")
   ;

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

   if( vm.count("channel") ){
      SetChannelType( vm["channel"].as<string>() );
   } else {
      cerr << "Settings for option [channel] not found!" << endl;
      cerr << desc << endl;
      return 1;
   }

   SampleRooFitMgr::x().setRange("FitRange",SampleRooFitMgr::MinFitMass(),SampleRooFitMgr::MaxMass());

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

   if( GetMethod() == "SimFit"  ){
      cout  << "Running SimFit Method!" << endl;
      InitDataAndSignal();
      MakeSimFit(data,signal_list);
   } else if( GetMethod() == "Template" ){
      cout << "Running MC template method!" << endl;
      InitDataAndSignal();
      InitMC();
      MakeTemplate(data,mc,signal_list);
   } else if( GetMethod().find("Bias") != string::npos ){
      cout << "Running Bias check!" << endl;
      InitDataAndSignal();
      InitMC();
      MakeBias(data,mc,signal_list);
   } else if( GetMethod() == "SideBand"  ){
      cout << "Running side band method!" << endl;
      InitDataAndSignal();
      MakeCheckPlot(data);
      for( auto& sample : signal_list ){
         MakeSideBand(data,sample);
      }
   } else {
      cerr << "Unrecognized method!" << endl;
      return 1;
   }

   return 0;
}


//------------------------------------------------------------------------------
//   Helper function implementations
//------------------------------------------------------------------------------
static mgr::ConfigReader static_cfg( "./data/Static.json" );
static mgr::ConfigReader group_cfg( "./data/Groups.json" );
void InitDataAndSignal()
{
   mgr::SampleMgr::InitStaticFromReader( static_cfg );
   mgr::SampleMgr::SetFilePrefix( GetEDMPrefix() );

   data = new SampleRooFitMgr( GetDataTag() , group_cfg );
   data->MakeReduceDataSet("FitRange",RooFit::CutRange("FitRange"));

   for( const auto& signal_tag : group_cfg.GetStaticStringList("Signal List") ){
      signal_list.push_back( new SampleRooFitMgr( signal_tag , group_cfg ) );
   }
}

void InitMC()
{
   mc = new SampleRooFitMgr("Background", group_cfg );
}

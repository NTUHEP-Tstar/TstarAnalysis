/*******************************************************************************
*
*  Filename    : MakeRooFit.cc
*  Description : Making RooFit objects to be passed over to Higgs Combine
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFit.hpp"
#include "TstarAnalysis/LimitCalc/interface/Template.hpp"

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace opt = boost::program_options;

int
main( int argc, char* argv[] )
{
  SampleRooFitMgr* data = NULL;
  SampleRooFitMgr* mc   = NULL;
  vector<SampleRooFitMgr*> signal_list;

  limnamer.AddOptions( LimitOptions() ).AddOptions( ExtraCutOptions() );
  limnamer.SetNamingOptions( "fitmethod", "fitfunc", "era" );


  const int run = limnamer.ParseOptions( argc, argv );
  if( run == mgr::OptNamer::PARSE_HELP  ){ return 0; }
  if( run == mgr::OptNamer::PARSE_ERROR ){ return 1; }

  InitSampleStatic( limnamer );
  InitRooFitSettings( limnamer );

  InitDataAndSignal( data, signal_list );
  InitMC( mc );

  if( limnamer.CheckInput( "mucut" ) ){ limnamer.AddCutOptions( "mucut" ); }
  if( limnamer.CheckInput( "muiso" ) ){ limnamer.AddCutOptions( "muiso" ); }
  if( limnamer.CheckInput( "masscut" ) ){limnamer.AddCutOptions( "masscut" ); }


  if( limnamer.GetInput<string>( "fitmethod" ) == "SimFit"  ){
    cout << "Running SimFit Method!" << endl;
    MakeSimFit( data, mc, signal_list );
  } else if( limnamer.GetInput<string>( "fitmethod" ) == "Template" ){
    cout << "Running MC template method!" << endl;
    MakeTemplate( data, mc, signal_list );
  } else if( limnamer.GetInput<string>( "fitmethod" ).find( "Bias" ) != string::npos ){
    cout << "Running Bias check!" << endl;
    MakeBias( data, mc, signal_list );
  } else {
    cerr << "Unrecognized method!" << endl;
    return 1;
  }

  delete data;
  delete mc;

  for( auto& sig : signal_list ){
    delete sig;
  }

  return 0;
}

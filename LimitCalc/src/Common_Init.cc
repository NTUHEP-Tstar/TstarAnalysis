/*******************************************************************************
*
*  Filename    : Common_Init.cc
*  Description : Initialization functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include <boost/program_options.hpp>
#include <vector>

#include "Math/MinimizerOptions.h"

using namespace std;
using namespace mgr;
namespace opt = boost::program_options;

/*******************************************************************************
*   External variables for this subpackage
*******************************************************************************/
TstarNamer limnamer( "LimitCalc" );

/******************************************************************************/

extern const std::vector<std::string> uncsource = {
  "jec",
  "jetres",
  "btag",
  "pu",
  "lep",
  "pdf",
  "scale",
  "model"
};

/******************************************************************************/

extern opt::options_description
LimitOptions()
{
  opt::options_description ans( "Common option related to limit calculation" );
  ans.add_options()
    ( "fitmethod,m", opt::value<string>()->required(),              "Which fitting method to use" )
    ( "fitfunc,f",   opt::value<string>()->required(),              "Which fitting function to use" )
    ( "era,e",       opt::value<string>()->default_value( "Rereco" ), "Which data era to use" )
  ;
  return ans;
}

/******************************************************************************/

extern opt::options_description
MassOptions()
{
  opt::options_description ans( "Mass point choosing options" );
  ans.add_options()
    ( "masspoint,p", opt::value<string>()->required(), "Which mass point tag (ex TstarM1000) to use." )
  ;
  return ans;
}

/******************************************************************************/

extern opt::options_description
PsuedoExpOptions()
{
  opt::options_description ans( "Psuedo experiment settings" );
  ans.add_options()
    ( "relmag,x", opt::value<double>(), "Relative magnitude of signal compared with prediction" )
    ( "absmag,a", opt::value<double>(), "Absolute magnitude of signal (number of events)" )
  ;
  return ans;
}


extern opt::options_description
ExtraCutOptions()
{
  opt::options_description ans( "Apply extra cut");
  ans.add_options()
      ( "mucut,c"  , opt::value<double>(), "Set the pass threshold for the muon" )
      ( "masscut,t", opt::value<double>(), "Mass of gluon plus t" )
  ;
  return ans;
}

/******************************************************************************/

extern mgr::OptNamer::PARSE_RESULTS
CheckPsuedoExpOptions()
{
  if( limnamer.GetInput<string>( "fitmethod" ) != "SimFit" ){
    cerr << "Does not support any psuedo experiment method other than SimFit!" << endl;
    cerr << limnamer.GetDescription() << endl;
    return mgr::OptNamer::PARSE_ERROR;
  } else if( limnamer.CheckInput( "relmag" ) && limnamer.CheckInput( "absmag" ) ){
    cerr << "Cannot set both relative and absolute magnitued" << endl;
    cerr << limnamer.GetDescription() << endl;
    return mgr::OptNamer::PARSE_ERROR;
  } else if( !limnamer.CheckInput( "relmag" ) && !limnamer.CheckInput( "absmag" ) ){
    cerr << "Must set either relative of absolute magnitude" << endl;
    cerr << limnamer.GetDescription() << endl;
    return mgr::OptNamer::PARSE_ERROR;
  }
  return mgr::OptNamer::PARSE_SUCESS;
}


/*******************************************************************************
*   Initialization functions
*******************************************************************************/
void
InitSingle( SampleRooFitMgr*& mgr, const string& tag )
{
  const mgr::ConfigReader& cfg = limnamer.MasterConfig();
  mgr = new SampleRooFitMgr( tag, cfg );
}

/******************************************************************************/

void
InitRooFitSettings( const TstarNamer& x )
{
  const mgr::ConfigReader& cfg = x.MasterConfig();
  const double mass_min        = cfg.GetStaticDouble( "Mass Min" );
  const double mass_max        = cfg.GetStaticDouble( "Mass Max" );
  SampleRooFitMgr::InitStaticVars( mass_min, mass_max );
  SampleRooFitMgr::x().setRange( "FitRange", mass_min, mass_max );

  // Default fitting settings
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2","Migrad");
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);
  ROOT::Math::MinimizerOptions::SetDefaultMaxFunctionCalls( 10000 );
  ROOT::Math::MinimizerOptions::SetDefaultMaxIterations( 50000 );
  ROOT::Math::MinimizerOptions::SetDefaultPrecision( 1e-7 );
  ROOT::Math::MinimizerOptions::SetDefaultTolerance( 1e-7 );
}

/******************************************************************************/

void
InitDataAndSignal( SampleRooFitMgr*& data, vector<SampleRooFitMgr*>& siglist )
{
  const mgr::ConfigReader& cfg = limnamer.MasterConfig();
  const string datatag         = limnamer.GetChannelEXT( "Data Prefix" )
                                 + limnamer.GetExt<string>( "era", "Data Postfix" );

  for( const auto& signaltag : cfg.GetStaticStringList( "Signal List" ) ){
    siglist.push_back( new SampleRooFitMgr( signaltag, cfg ) );
  }

  cout << "Created RooFitMgr for data: " << datatag << endl;
  data = new SampleRooFitMgr( datatag, cfg );

}

/******************************************************************************/

void
InitMC( SampleRooFitMgr*& mc )
{
  const mgr::ConfigReader& cfg = limnamer.MasterConfig();
  mc = new SampleRooFitMgr( "Background", cfg );
}

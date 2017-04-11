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
    ( "bkgtype",  opt::value<string>()->default_value( "" ), "Which alternative background to use" )
  ;
  return ans;
}

/******************************************************************************/

extern opt::options_description
ExtraCutOptions()
{
  opt::options_description ans( "Apply extra cut" );
  ans.add_options()
    ( "mucut",     opt::value<double>(), "Set the pass threshold for the muon" )
    ( "muiso",     opt::value<double>(), "Threshold for muon isolation" )
    ( "masscut",   opt::value<double>(), "Mass of gluon plus t" )
    ( "leadjetpt", opt::value<double>(), "Leading jet pt cut" )
    ( "useparam",  opt::value<int>(),    "Whether or not to use a parametric signal shape (no by default)" )
    ( "recoalgo",  opt::value<string>(), "Which reconstruction algorithm to use" )
    ( "scaleres",  opt::value<double>(), "Scaling the resolution manually" )
  ;
  return ans;
}

extern opt::options_description
ExtraLimitOptions()
{
  opt::options_description ans( "Investigate limit fluctuation"  );
  ans.add_options()
    ( "seed,s", opt::value<double>(), "change the random seed "  )
    ( "rMin",   opt::value<double>(), "override the r min value"  )
    ( "rMax",   opt::value<double>(), "override the r max value"  )
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

/******************************************************************************/

extern opt::options_description
HiggsCombineOptions()
{
  opt::options_description ans( "Extra options for higgs combine" );
  ans.add_options()
    ( "combine,x", opt::value<string>(), "Which method to run with combine" )
    ( "drawdata,d", "Whether to plot the data limits or not" )
  ;
  return ans;
}

/******************************************************************************/

extern mgr::OptNamer::PARSE_RESULTS
CheckDisableOptions()
{
  if( !limnamer.CheckInput( "disable" ) ){
    return mgr::OptNamer::PARSE_SUCESS;
  }

  for( const auto& unc : uncsource ){
    if( limnamer.GetInput<string>( "disable" ) == unc ){
      return mgr::OptNamer::PARSE_SUCESS;
    }
  }

  if( limnamer.GetInput<string>( "disable" ) == "stat"
      || limnamer.GetInput<string>( "disable" ) == "lumi"
      || limnamer.GetInput<string>( "disable" ) == "bg" ){
    return mgr::OptNamer::PARSE_SUCESS;
  }

  cerr << "Unknown uncertainty to disable!" << endl;
  return mgr::OptNamer::PARSE_ERROR;
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
  double mass_min              = cfg.GetStaticDouble( "Mass Min" );
  const double mass_max        = cfg.GetStaticDouble( "Mass Max" );

  if( x.CheckInput( "masscut" ) ){
    mass_min = x.GetInput<double>( "masscut" );
  }

  SampleRooFitMgr::InitStaticVars( mass_min, mass_max );
  SampleRooFitMgr::x().setRange( "FitRange", mass_min, mass_max );

}
/******************************************************************************/
void
InitData( SampleRooFitMgr*& data )
{
  const string datatag = limnamer.GetChannelEXT( "Data Prefix" )
                         + limnamer.GetExt<string>( "era", "Data Postfix" );
  InitSingle( data, datatag );
}

/******************************************************************************/

void
InitDataAndSignal( SampleRooFitMgr*& data, vector<SampleRooFitMgr*>& siglist )
{
  const mgr::ConfigReader& cfg = limnamer.MasterConfig();

  for( const auto& signaltag : cfg.GetStaticStringList( "Signal List" ) ){
    siglist.push_back( new SampleRooFitMgr( signaltag, cfg ) );
  }

  InitData( data );
}

/******************************************************************************/

void
InitMC( SampleRooFitMgr*& mc )
{
  if( limnamer.CheckInput( "bkgtype" ) ){
    const string bkgtype = limnamer.GetInput<string>( "bkgtype" );
    InitSingle( mc, "Background"+bkgtype );
  } else {
    InitSingle( mc, "Background" );
  }
}

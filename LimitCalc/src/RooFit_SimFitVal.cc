/*******************************************************************************
*
*  Filename    : RooFit_SimFit_Val.cc
*  Description : Validation of simfit method via signal injection
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/MakeKeysPdf.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFit.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"
#include "TstarAnalysis/LimitCalc/interface/Template.hpp"

#include "ManagerUtils/Common/interface/Random.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "RooDataSet.h"
#include "RooRandom.h"

using namespace std;

/*******************************************************************************
*   Helper functions
*******************************************************************************/
void
RunGenFit(
  SampleRooFitMgr* mc,
  SampleRooFitMgr* sigmgr,
  SampleRooFitMgr* data
  )
{
  RooFitResult* bgconstrain = FitBackgroundTemplate( mc, "" );
  const double bkgnum       = data->DataSet()->sumEntries();
  const double signum
    = limnamer.CheckInput( "relmag" ) ? sigmgr->ExpectedYield() * limnamer.GetInput<double>( "relmag" ) :
      limnamer.GetInput<double>( "absmag" );

  vector<RooRealVar*> paramlist = mc->VarContains( "template" );
  const double param1           = paramlist[0]->getVal();
  const double param2           = paramlist[1]->getVal();

  RooAbsPdf* bkgfunc = mc->Pdf( "template" );
  RooAbsPdf* sigfunc = MakeSingleKeysPdf( sigmgr, "" );

  const string strgthtag = SigStrengthTag();
  const string filename  = limnamer.TextFileName( "valsimfit",  strgthtag );

  ofstream result;
  if( !boost::filesystem::exists( filename ) ){
    // If doesn't already exists create new file and print first fitting value.
    result.open( filename, ios_base::out );
    cout << "Writting central value to file " << filename << endl;
    result << boost::format( "%lf %lf %lf %lf" ) % bkgnum % signum % param1 % param2  << endl;
  } else {
    // Else open in append mode.
    cout << "Appending fit to file " << filename << endl;
    result.open( filename, ios_base::app );
  }

  // Setting random seed to present time to allow multiple reruns
  RooRandom::randomGenerator()->SetSeed( time( NULL ) );

  for( int i = 0; i < limnamer.GetInput<int>( "num" ); ++i ){
    const string pseudosetname = "pseudo" + mgr::RandomString( 6 );

    const double sigmax = std::max( signum*3, 10000. );
    const double sigmin = ( signum > 100 ) ? 0 : -sigmax;

    RooRealVar nb( "nb", "nb", bkgnum, 0, 3*bkgnum );
    RooRealVar ns( "ns", "ns", signum, sigmin, sigmax );

    RooRealVar& p1 = *paramlist[0];
    RooRealVar& p2 = *paramlist[1];

    p1.setConstant( kFALSE );
    p2.setConstant( kFALSE );

    // Resetting value
    p1 = param1;
    p2 = param2;

    RooAddPdf model(
      "model", "model",
      RooArgList( *bkgfunc, *sigfunc ),
      RooArgList( nb,       ns )
      );

    RooDataSet* set = model.generate( SampleRooFitMgr::x(), RooFit::Extended( kTRUE ) );

    RooFitResult* ans = NULL;
    unsigned iter = 0;
    while( !ans && iter < 5 ){
      ans = model.fitTo( *set,
        RooFit::Verbose( kFALSE ),
        RooFit::PrintLevel( -1 ),
        RooFit::PrintEvalErrors( -1 ),
        RooFit::Warnings( kFALSE ),
        RooFit::Save()
        );

      if( ans->status() ){
        delete ans;
        ans = NULL;
        ++iter;
      }
    }
    delete ans;

    result << boost::format(  "%20.12lf %20.12lf \t %20.12lf %20.12lf \t %20.12lf %20.12lf \t %20.12lf %20.12lf" )
      % nb.getValV() % nb.getError()
      % ns.getValV() % ns.getError()
      % p1.getValV() % p1.getError()
      % p2.getValV() % p2.getError()
           << endl;

    delete set;
  }

  delete bgconstrain;

  result.close();
}

/******************************************************************************/

string
SigStrengthTag()
{
  static boost::format secfmt( "%1%%2%" );
  return limnamer.CheckInput( "relmag" ) ?
         str( secfmt % "rel" % limnamer.GetInput<double>( "relmag" ) ) :
         str( secfmt % "abs" % limnamer.GetInput<double>( "absmag" ) );
}

/******************************************************************************/
string
RhoTag()
{
  static boost::format rhofmt( "forcerho%2%" );
  return limnamer.CheckInput( "forcerho" ) ?
         str( rhofmt % "rel" % limnamer.GetInput<double>( "forcerho" ) ) :
         "";
}

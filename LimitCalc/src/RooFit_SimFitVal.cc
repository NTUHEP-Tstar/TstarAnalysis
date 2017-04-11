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
  const string filename  = limnamer.TextFileName( "valsimfit", strgthtag );

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

    RooDataSet* psuedoset = bkgfunc->generate(
      SampleRooFitMgr::x(),
      bkgnum,
      RooFit::Extended(),
      RooFit::Name( pseudosetname.c_str() )
      );
    RooDataSet* sigset = sigfunc->generate(
      SampleRooFitMgr::x(),
      signum,
      RooFit::Extended()
      );
    psuedoset->append( *sigset );
    delete sigset;

    MakeFullKeysPdf( sigmgr );

    // Passes ownership to data
    mc->AddDataSet( psuedoset );
    mc->SetConstant( kFALSE );

    auto fitres                 = SimFitSingle( mc, sigmgr, pseudosetname, bgconstrain );
    const string pseudobgname   = SimFitBGPdfName( pseudosetname, sigmgr->Name() );
    const string pseudocombname = SimFitCombinePdfName( pseudosetname, sigmgr->Name() );
    const RooRealVar* bkgfit    = (RooRealVar*)( fitres->floatParsFinal().find( ( mc->Name() + pseudocombname+"nb" ).c_str() ) );
    const RooRealVar* sigfit    = (RooRealVar*)( fitres->floatParsFinal().find( ( mc->Name() + pseudocombname+"ns" ).c_str() ) );
    const RooRealVar* p1fit     = (RooRealVar*)( fitres->floatParsFinal().find( ( mc->Name() + pseudobgname + "a" ).c_str() ) );
    const RooRealVar* p2fit     = (RooRealVar*)( fitres->floatParsFinal().find( ( mc->Name() + pseudobgname + "b" ).c_str() ) );

    if( fitres->status() ){// Only if status is 0
      // Getting results

      result << boost::format(  "%lf %lf \t %lf %lf \t %lf %lf \t %lf %lf" )
        % bkgfit->getValV() % bkgfit->getError()
        % sigfit->getValV() % sigfit->getError()
        % p1fit->getValV() % p1fit->getError()
        % p2fit->getValV() % p2fit->getError()
             << endl;

      // Saving plots for special cases
      if( ( bkgfit->getVal() - bkgnum )/bkgfit->getError() > 3 ){
        MakeSimFitPlot( mc, sigmgr, fitres, pseudosetname, "bkgexcess" );
      } else if( ( bkgfit->getVal() - bkgnum )/bkgfit->getError() < -3 ){
        MakeSimFitPlot( mc, sigmgr, fitres, pseudosetname, "bkgdifficient" );
      } else if( ( sigfit->getVal() - signum )/sigfit->getError() > 3 ){
        MakeSimFitPlot( mc, sigmgr, fitres, pseudosetname, "sigexcess" );
      } else if( ( sigfit->getVal() - signum )/sigfit->getError() < -3 ){
        MakeSimFitPlot( mc, sigmgr, fitres, pseudosetname, "sigdifficient" );
      } else if( i%100 == 0 ){
        MakeSimFitPlot( mc, sigmgr, fitres, pseudosetname );
      }
    } else {
      --i;
    }

    // Deleting to avoid taking up to much memory
    mc->RemoveDataSet( pseudosetname );
    mc->RemovePdf( pseudobgname );
    mc->RemovePdf( pseudocombname );
    mc->RemoveVar( pseudocombname + "nb" );
    mc->RemoveVar( pseudocombname + "ns" );
    mc->RemoveVar( pseudobgname + "a" );
    mc->RemoveVar( pseudobgname + "b" );

  }

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

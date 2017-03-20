/*******************************************************************************
*
*  Filename    : KeysCompMgr.cc
*  Description : Definition of how objects are stored and data is loaded
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/MassRecoCompare/interface/KeysCompMgr.hpp"

#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"

#include "RooAddPdf.h"
#include "RooBifurGauss.h"
#include "RooConstVar.h"
#include "RooDataSet.h"
#include "RooFFTConvPdf.h"
#include "RooGaussian.h"
#include "RooKeysPdf.h"
#include "RooRealVar.h"
#include "TFile.h"

#include <boost/format.hpp>
#include <iostream>

using namespace std;
using namespace mgr;

/*******************************************************************************
*   Declaring global RooRealVars
*******************************************************************************/
void
KeysCompMgr::InitStaticVars()
{
  StaticNewVar( "x", "m_{t+jet}", "GeV/c^{2}", 400, 2500 );
  StaticNewVar( "w", "w",         "",        -1000, 1000 );
  StaticNewVar( "n", "n",         "",           -1,    7 );
}

/******************************************************************************/

RooRealVar&
KeysCompMgr::x(){ return *StaticVar( "x" ); }

/******************************************************************************/

RooRealVar&
KeysCompMgr::w(){ return *StaticVar( "w" ); }

/******************************************************************************/

RooRealVar&
KeysCompMgr::n(){ return *StaticVar( "n" );}

/*******************************************************************************
*   Constructor and data set definition
*******************************************************************************/
KeysCompMgr::KeysCompMgr( const std::string& name, const std::string& latexname ) :
  Named( name ),
  RooFitMgr( name )
{
  SetLatexName( latexname );
  RooDataSet* set = new RooDataSet(
    "",
    "",
    RooArgSet( x(), n(), w() ),
    RooFit::WeightVar( w() )
    );
  AddDataSet( set );
}


/******************************************************************************/

KeysCompMgr::~KeysCompMgr()
{
  for( const auto& name : VarNameList() ){
    RooRealVar* var = Var( name );
    cout << boost::format( "%-50s %8.4lf %8.4f %8.4f" )
      % var->GetName()
      % var->getVal()
      % var->getErrorHi()
      % var->getErrorLo()
         << endl;
  }

  for( const auto& name : FuncNameList() ){
    RooAbsReal* func = Func( name );
    cout << boost::format( "%50s %8.1lf" )
    % func->GetName()
    %func->getVal()
    << endl;
  }
}

/*******************************************************************************
*   Filling by event looping
*******************************************************************************/
void
KeysCompMgr::FillDataSet( const std::string& filename )
{
  fwlite::Event evt( TFile::Open( filename.c_str() ) );
  fwlite::Handle<RecoResult> resulthandle;

  for( evt.toBegin(); !evt.atEnd(); ++evt ){
    resulthandle.getByLabel( evt, Name().c_str(), LabelName( Name() ).c_str(), ProcessName().c_str() );

    const double evtweight = GetEventWeight( evt );
    const double tstarmass = resulthandle.ref().TstarMass();

    if( tstarmass > x().getMax() ){ continue; }
    if( tstarmass < x().getMin() ){ continue; }
    if( evtweight > w().getMax() ){ continue; }
    if( evtweight < w().getMin() ){ continue; }
    x() = tstarmass;
    n() = (double)NumCorrectAssign( resulthandle.ref() );
    DataSet( "" )->add( RooArgSet( x(), n() ), evtweight );
  }

  cout << "Done!" << endl;
}


/******************************************************************************/

void
KeysCompMgr::MakeKeysPdf()
{
  RooDataSet& dataset = *( dynamic_cast<RooDataSet*>( DataSet( "" ) ) );

  double na = dataset.sigma( x() );
  double n2 = dataset.sigma( x(), "n>=3"  );
  double n3 = dataset.sigma( x(), "n>=4"  );
  double n4 = dataset.sigma( x(), "n>=5"  );

  static const vector<double> rholist = {
    1.364,
    1.166,
    1.660
  };
  cout << "Computed Value: "
       << sqrt( na/n2 ) << " "
       << sqrt( na/n3 ) << " "
       << sqrt( na/n4 ) << " "
       << na << endl;

  RooKeysPdf* pdf;

  boost::format nameformat( "rho%.2lf" );
  boost::format titleformat( "#rho = %.2lf" );

  for( const auto rho : rholist ){
    const string name  = str( nameformat % rho );
    const string title = str( titleformat % rho );
    pdf = new RooKeysPdf(
      name.c_str(),
      title.c_str(),
      x(),
      dataset,
      RooKeysPdf::NoMirror,
      rho
      );
    AddPdf( pdf );
  }
}

/******************************************************************************/

void
KeysCompMgr::MakeGauss()
{
  RooRealVar* m  = NewVar( "m", reconamer.GetInput<int>( "mass" ), 400, 3000 );
  RooRealVar* s  = NewVar( "s", 200, 0, 1000 );
  RooAbsPdf* pdf = new RooGaussian( "gauss", "", x(), *m, *s );
  AddPdf( pdf );
  pdf->fitTo(
    *DataSet( "" ),
    RooFit::Minimizer( "Minuit", "Migrad" ),
    RooFit::Minos( kTRUE ),
    RooFit::SumW2Error( kTRUE ),
    RooFit::Verbose( kFALSE ),
    RooFit::PrintLevel( -1 ),
    RooFit::PrintEvalErrors( -1 ),
    RooFit::Warnings( kFALSE )
    );

  RooRealVar* m1         = NewVar( "m1", reconamer.GetInput<int>( "mass" ), 400, 3000 );
  RooRealVar* m2         = NewVar( "m2", reconamer.GetInput<int>( "mass" )+200, 400, 3000 );
  RooRealVar* s1         = NewVar( "s1", 200, 0, 1000 );
  RooRealVar* s2         = NewVar( "s2", 200, 0, 1000 );
  RooRealVar* frac       = NewVar( "frac", 0.5, 0, 1 );
  RooAbsPdf* gauss1      = new RooGaussian( "guass2_1", "", x(), *m1, *s1 );
  RooAbsPdf* gauss2      = new RooGaussian( "guass2_2", "", x(), *m2, *s2 );
  RooAbsPdf* doublegauss = new RooAddPdf( "doublegauss", "", *gauss1, *gauss2, *frac );
  AddPdf( doublegauss );
  doublegauss->fitTo(
    *DataSet( "" ),
    RooFit::Minimizer( "Minuit", "Migrad" ),
    RooFit::Minos( kTRUE ),
    RooFit::SumW2Error( kTRUE ),
    RooFit::Verbose( kFALSE ),
    RooFit::PrintLevel( -1 ),
    RooFit::PrintEvalErrors( -1 ),
    RooFit::Warnings( kFALSE )
    );

}

void
KeysCompMgr::MakeConv()
{
  // Single bifurcation
  RooRealVar* bimean = NewVar( "bimean", 1200, 700, 2000 );
  RooRealVar* bil    = NewVar( "bil", 200, 0, 1000 );
  RooRealVar* bir    = NewVar( "bir", 300, 0, 1000 );
  RooAbsPdf* bipdf   = new RooBifurGauss( "bigauss", "", x(), *bimean, *bil, *bir );

  RooConstVar* gmean = new RooConstVar( "convgmean", "", 0 );
  RooRealVar* gsig   = NewVar( "convgsig", 100, 0, 1000 );
  RooAbsPdf* gpdf    = new RooGaussian( "convgauss", "", x(), *gmean, *gsig );

  RooAbsPdf* convpdf = new RooFFTConvPdf( "conv", "conv", x(), *bipdf, *gpdf );

  convpdf->fitTo(// Forcing fit, and setting to constant
    *DataSet( "" ),
    RooFit::Minimizer( "Minuit", "Migrad" ),
    RooFit::Minos( kTRUE ),
    RooFit::SumW2Error( kTRUE ),
    RooFit::Verbose( kFALSE ),
    RooFit::PrintLevel( -1 ),
    RooFit::PrintEvalErrors( -1 ),
    RooFit::Warnings( kFALSE )
    );
  AddPdf( bipdf );
  AddPdf( gpdf );
  AddPdf( convpdf );
  AddFunc( gmean );

  // Double bifurcation
  RooRealVar* bimean1 = NewVar( "bimean1", 1000, 400, 2000 );
  RooRealVar* bil1    = NewVar( "bil1", 200, 0, 1000 );
  RooRealVar* bir1    = NewVar( "bir1", 300, 0, 1000 );
  RooAbsPdf* bipdf1   = new RooBifurGauss( "bigauss1", "", x(), *bimean1, *bil1, *bir1 );

  RooConstVar* gmean1 = new RooConstVar( "convgmean1", "", 0 );
  RooRealVar* gsig1   = NewVar( "convgsig1", 100, 0, 1000 );
  RooAbsPdf* gpdf1    = new RooGaussian( "convgauss1", "", x(), *gmean1, *gsig1 );

  RooAbsPdf* conv1pdf = new RooFFTConvPdf( "conv1", "", x(), *bipdf1, *gpdf1 );

  RooRealVar* bimean2 = NewVar( "bimean2", 1600, 400, 2000 );
  RooRealVar* bil2    = NewVar( "bil2", 100, 0, 1000 );
  RooRealVar* bir2    = NewVar( "bir2", 200, 0, 1000 );
  RooAbsPdf* bipdf2   = new RooBifurGauss( "bigauss2", "", x(), *bimean2, *bil2, *bir2 );

  RooConstVar* gmean2 = new RooConstVar( "convgmean2", "", 0 );
  RooRealVar* gsig2   = NewVar( "convgsig2", 100, 0, 1000 );
  RooAbsPdf* gpdf2    = new RooGaussian( "convgauss2", "", x(), *gmean2, *gsig2 );

  RooAbsPdf* conv2pdf = new RooFFTConvPdf( "conv2", "", x(), *bipdf2, *gpdf2 );

  RooRealVar* bico = NewVar( "bicoeff2", 0.6, 0, 1 );

  RooAbsPdf* bisum = new RooAddPdf( "convsum2", "", *conv1pdf, *conv2pdf, *bico );

  bisum->fitTo(// Forcing fit, and setting to constant
    *DataSet( "" ),
    RooFit::Minimizer( "Minuit", "Migrad" ),
    RooFit::Minos( kTRUE ),
    RooFit::SumW2Error( kTRUE ),
    RooFit::Verbose( kFALSE ),
    RooFit::PrintLevel( -1 ),
    RooFit::PrintEvalErrors( -1 ),
    RooFit::Warnings( kFALSE )
    );

  // make sample claim ownership of pdfs

  AddPdf( bipdf1 );
  AddPdf( gpdf1 );
  AddPdf( conv1pdf );
  AddPdf( bipdf2 );
  AddPdf( gpdf2 );
  AddPdf( conv2pdf );
  AddPdf( bisum );

  AddFunc( gmean1 );
  AddFunc( gmean2 );
}

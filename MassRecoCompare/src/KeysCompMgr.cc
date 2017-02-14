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

#include "TFile.h"
#include <RooDataSet.h>
#include <RooKeysPdf.h>
#include <RooRealVar.h>

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
  StaticNewVar( "x", "M_{t+g}", "GeV/c^{2}", 400, 2500 );
  StaticNewVar( "w", "w",       "",        -1000, 1000 );
  StaticNewVar( "n", "n",       "",           -1,    7 );
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
{}

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
    1.382,
    1.176,
    1.676
  };
  cout << "Computed Value: "
       << sqrt( na/n2 ) << " "
       << sqrt( na/n3 ) << " "
       << sqrt( na/n4 ) << " "
       << na << endl;

  RooKeysPdf* pdf;

  boost::format nameformat( "rho%lf" );
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

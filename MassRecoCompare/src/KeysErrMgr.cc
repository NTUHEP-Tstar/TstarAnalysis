/*******************************************************************************
*
*  Filename    : KeysErrMgr.cc
*  Description : Classes for managing keys PDf comparison for different nuisance
*                parameters
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/KeysErrMgr.hpp"

#include "TFile.h"
#include <RooDataSet.h>
#include <RooKeysPdf.h>
#include <RooRealVar.h>

#include <boost/format.hpp>
#include <iostream>

using namespace std;
using namespace mgr;


/******************************************************************************/

void
KeysErrMgr::InitStaticVars()
{
  StaticNewVar( "x", "M_{t+g}", "GeV/c^{2}", 400, 2500 );
  StaticNewVar( "w", "w",       "",        -1000, 1000 );
}

/******************************************************************************/

RooRealVar&
KeysErrMgr::x(){ return *StaticVar( "x" ); }

/******************************************************************************/

RooRealVar&
KeysErrMgr::w(){ return *StaticVar( "w" ); }

/*******************************************************************************
*   Delcaring default datasets
*******************************************************************************/
KeysErrMgr::KeysErrMgr( const std::string& name, const std::string& latexname ) :
  Named( name ),
  RooFitMgr( name )
{
  SetLatexName( latexname );

  AddDataSet( new RooDataSet(
      "",
      "",
      RooArgSet( x(), w() ),
      RooFit::WeightVar( w() )
      ) );

  for( const auto& err : histerrlist ){
    const auto& unc = err.tag;
    AddDataSet( new RooDataSet(
        ( unc+"Up" ).c_str(),
        ( unc+"Up" ).c_str(),
        RooArgSet( x(), w() ),
        RooFit::WeightVar( w() )
        ) );
    AddDataSet( new RooDataSet(
        ( unc+"Down" ).c_str(),
        ( unc+"Down" ).c_str(),
        RooArgSet( x(), w() ),
        RooFit::WeightVar( w() )
        ) );
  }
}

/******************************************************************************/

KeysErrMgr::~KeysErrMgr()
{}

/*******************************************************************************
*   Filling by event looping
*******************************************************************************/
void
KeysErrMgr::FillDataSet( const std::string& filename )
{
  fwlite::Event evt( TFile::Open( filename.c_str() ) );
  fwlite::Handle<RecoResult> resulthandle;

  auto pdfidgroup = GetPdfIdGrouping( filename );

  for( evt.toBegin(); !evt.atEnd(); ++evt ){
    resulthandle.getByLabel( evt, Name().c_str(), LabelName( Name() ).c_str(), ProcessName().c_str() );

    const double evtweight       = GetEventWeight( evt );
    const double tstarmass       = resulthandle.ref().TstarMass();
    const double btagweight      = GetBtagWeight( evt );
    const double btagweightup    = GetBtagWeightUp( evt );
    const double btagweightdown  = GetBtagWeightDown( evt  );
    const double puweight        = GetPileupWeight( evt );
    const double puweightup      = GetPileupWeightXsecup( evt );
    const double puweightdown    = GetPileupWeightXsecdown( evt );
    const double elecweight      = GetElectronWeight( evt );
    const double elecweightup    = GetElectronWeightUp( evt );
    const double elecweightdown  = GetElectronWeightDown( evt );
    const double muonweight      = GetMuonWeight( evt );
    const double muonweightup    = GetMuonWeightUp( evt );
    const double muonweightdown  = GetMuonWeightDown( evt );
    const double pdfweightup     = 1 + GetPdfWeightError( evt, pdfidgroup );
    const double pdfweightdown   = 1 - GetPdfWeightError( evt, pdfidgroup );
    const double scaleweightup   = 1 + GetScaleWeightError( evt, pdfidgroup );
    const double scaleweightdown = 1 - GetScaleWeightError( evt, pdfidgroup );

    if( tstarmass > x().getMax() ){ continue; }
    if( tstarmass < x().getMin() ){ continue; }
    if( evtweight > w().getMax() ){ continue; }
    if( evtweight < w().getMin() ){ continue; }
    x() = tstarmass;
    DataSet( "" )->add( RooArgSet( x() ), evtweight );
    DataSet( "btagUp" )->add( RooArgSet( x() ), evtweight * btagweightup / btagweight );
    DataSet( "btagDown" )->add( RooArgSet( x() ), evtweight * btagweightdown / btagweight );
    DataSet( "puUp" )->add( RooArgSet( x() ), evtweight * puweightup / puweight );
    DataSet( "puDown" )->add( RooArgSet( x() ), evtweight * puweightdown / puweight );
    DataSet( "leptonUp" )->add( RooArgSet( x() ), evtweight * elecweightup * muonweightup / elecweight / muonweight );
    DataSet( "leptonDown" )->add( RooArgSet( x() ), evtweight * elecweightdown * muonweightdown / elecweight / muonweight );
    DataSet( "pdfUp" )->add( RooArgSet( x() ), evtweight * pdfweightup  );
    DataSet( "pdfDown" )->add( RooArgSet( x() ), evtweight * pdfweightdown  );
    DataSet( "scaleUp" )->add( RooArgSet( x() ), evtweight * scaleweightup );
    DataSet( "scaleDown" )->add( RooArgSet( x() ), evtweight * scaleweightdown );

    x() = resulthandle.ref().ComputeFromPaticleList( tstar::corr_up );
    DataSet( "jecUp" )->add( RooArgSet( x() ), evtweight );
    x() = resulthandle.ref().ComputeFromPaticleList( tstar::corr_down );
    DataSet( "jecDown" )->add( RooArgSet( x() ), evtweight );
    x() = resulthandle.ref().ComputeFromPaticleList( tstar::res_up );
    DataSet( "jerUp" )->add( RooArgSet( x() ), evtweight );
    x() = resulthandle.ref().ComputeFromPaticleList( tstar::res_down );
    DataSet( "jerDown" )->add( RooArgSet( x() ), evtweight );
  }

  cout << "Done!" << endl;
}


/******************************************************************************/

void
KeysErrMgr::MakeKeysPdf()
{
  for( const auto setname : SetNameList() ){
    RooDataSet* dataset = dynamic_cast<RooDataSet*>( DataSet( setname ) );

    RooAbsPdf* pdf = new RooKeysPdf(
      setname.c_str(),
      setname.c_str(),
      x(),
      *dataset,
      RooKeysPdf::NoMirror,
      1.36
      );
    AddPdf( pdf );
  }

}

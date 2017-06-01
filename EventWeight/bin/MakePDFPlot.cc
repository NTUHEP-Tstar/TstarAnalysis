/*******************************************************************************
*
*  Filename    : MakePDFPlot.cc
*  Description : Making plot for PDF variables
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/

#include "LHAPDF/LHAPDF.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TGraphAsymmErrors.h"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
using namespace std;

/*******************************************************************************
*   Global objects
*******************************************************************************/
const LHAPDF::PDFSet set( "NNPDF30_lo_as_0130_nf_4" );
const size_t nmem               = set.size();
const double Q                  = 10.0;  // Scale factor in GeV
const vector<LHAPDF::PDF*> pdfs = set.mkPDFs();


/*******************************************************************************
*   Helper functions
*******************************************************************************/
TGraphAsymmErrors* MakePDFGraph( const int flavour );

/*******************************************************************************
*   Main control flow
*******************************************************************************/
int
main( int argc, char* argv[] )
{
  TGraphAsymmErrors* gluonpdf   = MakePDFGraph( 21 );
  TGraphAsymmErrors* uppdf      = MakePDFGraph( 2 );
  TGraphAsymmErrors* upbarpdf   = MakePDFGraph( -2 );
  TGraphAsymmErrors* downpdf    = MakePDFGraph( 1 );
  TGraphAsymmErrors* downbarpdf = MakePDFGraph( 1 );
  TGraphAsymmErrors* strangepdf = MakePDFGraph( 3 );

  TCanvas* c = mgr::NewCanvas();
  gluonpdf->Draw( "AL3" );
  strangepdf->Draw( "SAMEL3" );
  uppdf->Draw( "SAMEL3" );
  upbarpdf->Draw( "SAMEL3" );
  downpdf->Draw( "SAMEL3" );
  downbarpdf->Draw( "SAMEL3" );

  gluonpdf->SetMaximum( 20 );
  gluonpdf->SetMinimum( 1e-3 );
  gluonpdf->SetTitle( "" );
  gluonpdf->GetXaxis()->SetTitle( "Bjorken x" );
  gluonpdf->GetYaxis()->SetTitle( "f(x,Q^{2}), Q=10GeV" );

  gluonpdf->SetLineColor( kGreen+3 );
  gluonpdf->SetFillColor( kGreen-9 );
  strangepdf->SetLineColor( kOrange-3 );
  strangepdf->SetFillColor( kOrange-9 );

  uppdf->SetLineColor( kAzure-1 );
  uppdf->SetFillColor( kAzure-9 );
  upbarpdf->SetLineColor( kCyan+2 );
  upbarpdf->SetFillColor( kCyan-9 );

  downpdf->SetLineColor( kPink-1 );
  downpdf->SetFillColor( kPink-9 );
  downbarpdf->SetLineColor( kMagenta+2 );
  downbarpdf->SetFillColor( kMagenta-9 );

  TLegend* leg = mgr::NewLegend( 0.7, 0.6 );
  leg->SetNColumns( 2 );
  leg->AddEntry( uppdf,      "u",           "f" );
  leg->AddEntry( upbarpdf,   "#bar{u}",     "f" );
  leg->AddEntry( downpdf,    "d",           "f" );
  leg->AddEntry( downbarpdf, "#bar{d}",     "f" );
  leg->AddEntry( gluonpdf,   "g",           "f" );
  leg->AddEntry( strangepdf, "s / #bar{s}", "f" );
  leg->Draw();

  mgr::SetSinglePad( c );
  mgr::SetAxis( gluonpdf );
  c->SetLogy( kTRUE );
  c->SetLogx( kTRUE );

  mgr::SaveToPDF( c, "results/PDFPlot.pdf" );
  delete gluonpdf;
  delete uppdf;
  delete downpdf;
  delete c;
}


/*******************************************************************************
*   Helper function implementation
*******************************************************************************/

TGraphAsymmErrors*
MakePDFGraph( const int flavour )
{
  static const size_t npoints = 500;
  static const double xmin    = 0.01;
  static const double xmax    = 1.;
  static const double xsep    = pow( ( xmax-0.01 )/xmin, 1/(double)( npoints+1 ) );
  TGraphAsymmErrors* ans      = new TGraphAsymmErrors( (int)npoints );

  // Fill vectors xgAll and xuAll using all PDF members.
  // Could replace xg, xu by cross section, acceptance etc.
  vector<double> fvals( npoints );

  double x = xmin;// momentum fraction

  for( int i = 0; i < (int)npoints; ++i, x *= xsep ){
    fvals.clear();// Clear all values ;

    for( size_t imem = 0; imem < nmem; imem++ ){
      fvals.push_back( pdfs[imem]->xfxQ( flavour, x, Q ) );
    }

    const LHAPDF::PDFUncertainty err = set.uncertainty( fvals, 68, true );
    const double y                   = err.central;
    const double yup                 = err.errplus;
    const double ydown               = err.errminus;

    ans->SetPoint( i, x, y );
    ans->SetPointError( i, 0, 0, ydown, yup );
  }

  return ans;
}

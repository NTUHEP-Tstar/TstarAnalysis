/*******************************************************************************
*
*  Filename    : KeysPlot.cc
*  Description : Implementation for Keys Comparison plots
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"

#include "ManagerUtils/Maths/interface/RooFitExt.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"

#include <boost/format.hpp>

using namespace std;

void
MakeKeysPlots( KeysCompMgr* mgr )
{
  TCanvas* c     = mgr::NewCanvas();
  RooPlot* frame = KeysCompMgr::x().frame();

  RooAbsData* dataset = mgr->DataSet( "" );

  TGraph* setplot = mgr::PlotOn(
    frame, dataset,
    RooFit::DrawOption( PGS_DATA ),
    RooFit::Binning( 40, KeysCompMgr::x().getMin(), KeysCompMgr::x().getMax() )// Reducing to 40 bin
    );

  vector<TGraph*> keyplotlist;
  vector<RooAbsPdf*> pdflist;

  for( const auto& pdfname : mgr->PdfNameList() ){
    if( pdfname.find( "rho" ) != string::npos ){
      pdflist.push_back( mgr->Pdf( pdfname ) );
      keyplotlist.push_back( mgr::PlotOn( frame, mgr->Pdf( pdfname ) ) );
    }
  }

  frame->Draw();

  // Styling
  mgr::SetFrame( frame );
  mgr::DrawCMSLabel( SIMULATION );

  for( size_t i = 0; i < keyplotlist.size(); ++i ){
    keyplotlist.at( i )->SetLineColor( Color_Sequence[i] );
  }

  // Adding additional label
  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
  .WriteLine( mgr->LatexName() );

  // Making legend
  TLegend* leg = mgr::NewLegend( 0.55, 0.6 );

  boost::format setfmt( "Signal (%dGeV/c^{2})" );
  const string setentry = boost::str( setfmt % reconamer.GetInput<int>( "mass" ) );
  leg->AddEntry( setplot, setentry.c_str(), "lp" );

  boost::format pdffmt( "PDF approx (%s)" );

  for( size_t i = 0; i < keyplotlist.size(); ++i ){
    const TGraph* pdfplot = keyplotlist.at( i );
    const RooAbsPdf* pdf  = pdflist.at( i );
    const string pdfentry = str( pdffmt % pdf->GetTitle() );
    leg->AddEntry( pdfplot, pdfentry.c_str(), "l" );
  }

  leg->Draw();

  // Y axis range settings
  keyplotlist.push_back( setplot );
  const double ymax = mgr::GetYmax( keyplotlist );

  // for Linear scale
  frame->SetMinimum( 0 );
  frame->SetMaximum( ymax * 1.2 );
  mgr::SaveToPDF( c, reconamer.PlotFileName( "keyscomp" ) );

  // for Log scale
  frame->SetMinimum( 0.3 );
  frame->SetMaximum( pow( 10, ( log10( ymax )+0.5 )*1.2 ) );
  c->SetLogy( kTRUE );
  mgr::SaveToPDF( c, reconamer.PlotFileName( "keyscomp", "log" ) );

  delete c;
  delete frame;
}

/******************************************************************************/

void
MakeGaussPlots( KeysCompMgr* mgr )
{
  TCanvas* c     = mgr::NewCanvas();
  RooPlot* frame = KeysCompMgr::x().frame();

  TGraph* setplot = mgr::PlotOn(
    frame,
    mgr->DataSet( "" ),
    RooFit::DrawOption( PGS_DATA ),
    RooFit::Binning( 40, KeysCompMgr::x().getMin(), KeysCompMgr::x().getMax() )// Reducing to 40 bin
    );

  TGraph* keysplot = mgr::PlotOn(
    frame,
    mgr->Pdf( "rho1.36" )
    );

  TGraph* gaussplot = mgr::PlotOn(
    frame,
    mgr->Pdf( "gauss" )
    );

  TGraph* doublegauss = mgr::PlotOn(
    frame,
    mgr->Pdf( "doublegauss" )
    );

  frame->Draw();

  // Styling
  mgr::SetFrame( frame );
  mgr::DrawCMSLabel( SIMULATION );

  keysplot->SetLineColor( Color_Sequence[0] );
  gaussplot->SetLineColor( Color_Sequence[1] );
  doublegauss->SetLineColor( Color_Sequence[2] );

  const double kskeys   = KSTest( *( mgr->DataSet( "" ) ), *( mgr->Pdf( "rho1.36" ) ), KeysCompMgr::x() );
  const double ksgauss  = KSTest( *( mgr->DataSet( "" ) ), *( mgr->Pdf( "gauss" ) ), KeysCompMgr::x() );
  const double ksdgauss = KSTest( *( mgr->DataSet( "" ) ), *( mgr->Pdf( "doublegauss" ) ), KeysCompMgr::x() );


  // Making legend
  TLegend* leg = mgr::NewLegend( 0.45, 0.6 );
  boost::format setfmt( "Signal (%dGeV/c^{2})" );
  const string setentry = boost::str( setfmt % reconamer.GetInput<int>( "mass" ) );

  boost::format pdffmt( "%s (K=%.3lf)" );
  leg->AddEntry( setplot,     setentry.c_str(),                                               "lp" );
  leg->AddEntry( keysplot,    boost::str( pdffmt % "Kernel approximation" % kskeys ).c_str(), "l" );
  leg->AddEntry( gaussplot,   boost::str( pdffmt % "Gaussian" % ksgauss ).c_str(),            "l" );
  leg->AddEntry( doublegauss, boost::str( pdffmt % "Double Gaussian" % ksdgauss ).c_str(),    "l" );
  leg->Draw();

  // Adding additional label
  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MAX, 0.6-TEXT_MARGIN, TOP_RIGHT )
  .WriteLine( mgr->LatexName() );

  // Y axis range settings
  const double ymax = mgr::GetYmax( setplot, keysplot, gaussplot, doublegauss );

  // for Linear scale
  frame->SetMinimum( 0 );
  frame->SetMaximum( ymax * 1.6 );
  mgr::SaveToPDF( c, reconamer.PlotFileName( "gausscomp" ) );

  // for Log scale
  frame->SetMinimum( 0.3 );
  frame->SetMaximum( pow( 10, ( log10( ymax )+0.5 )*1.2 ) );
  c->SetLogy( kTRUE );
  mgr::SaveToPDF( c, reconamer.PlotFileName( "guasscomp", "log" ) );

  delete c;
  delete frame;

}

/******************************************************************************/

void
MakeConvPlots( KeysCompMgr* mgr )
{
  TCanvas* c     = mgr::NewCanvas();
  RooPlot* frame = KeysCompMgr::x().frame();

  TGraph* setplot = mgr::PlotOn(
    frame,
    mgr->DataSet( "" ),
    RooFit::DrawOption( PGS_DATA ),
    RooFit::Binning( 40, KeysCompMgr::x().getMin(), KeysCompMgr::x().getMax() )// Reducing to 40 bin
    );

  TGraph* keysplot = mgr::PlotOn(
    frame,
    mgr->Pdf( "rho1.36" )
    );

  TGraph* convplot = mgr::PlotOn(
    frame,
    mgr->Pdf( "conv" )
    );

  TGraph* conv2plot = mgr::PlotOn(
    frame,
    mgr->Pdf( "convsum2" )
    );



  frame->Draw();

  // Styling
  mgr::SetFrame( frame );
  mgr::DrawCMSLabel( SIMULATION );

  keysplot->SetLineColor( Color_Sequence[0] );
  convplot->SetLineColor( Color_Sequence[1] );
  conv2plot->SetLineColor( Color_Sequence[2] );

  const double kskeys  = KSTest( *( mgr->DataSet( "" ) ), *( mgr->Pdf( "rho1.36" ) ), KeysCompMgr::x() );
  const double ksconv  = KSTest( *( mgr->DataSet( "" ) ), *( mgr->Pdf( "conv" ) ), KeysCompMgr::x() );
  const double ksconv2 = KSTest( *( mgr->DataSet( "" ) ), *( mgr->Pdf( "convsum2" ) ), KeysCompMgr::x() );


  // Making legend
  TLegend* leg = mgr::NewLegend( PLOT_X_TEXT_MIN, 0.65 );
  boost::format setfmt( "Signal (%dGeV/c^{2})" );
  const string setentry = boost::str( setfmt % reconamer.GetInput<int>( "mass" ) );

  boost::format pdffmt( "%s (K=%.3lf)" );
  leg->AddEntry( setplot,   setentry.c_str(),                                                               "lp" );
  leg->AddEntry( keysplot,  boost::str( pdffmt % "Kernel approximation" % kskeys ).c_str(),                 "l" );
  leg->AddEntry( convplot,  boost::str( pdffmt % "Bif.Gaus #otimes Gauss" % ksconv ).c_str(),               "l" );
  leg->AddEntry( conv2plot, boost::str( pdffmt % "(Bif.Gaus #otimes Gauss)+(Bif.Gauss #otimes Gauss)" % ksconv2 ).c_str(), "l" );
  leg->Draw();

  // Adding additional label
  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MAX, 0.6-TEXT_MARGIN, TOP_RIGHT )
  .WriteLine( mgr->LatexName() );

  // Y axis range settings
  const double ymax = mgr::GetYmax( setplot, keysplot, convplot, conv2plot );

  // for Linear scale
  frame->SetMinimum( 0 );
  frame->SetMaximum( ymax * 1.6 );
  mgr::SaveToPDF( c, reconamer.PlotFileName( "convcomp" ) );

  // for Log scale
  frame->SetMinimum( 0.3 );
  frame->SetMaximum( pow( 10, ( log10( ymax )+0.5 )*1.2 ) );
  c->SetLogy( kTRUE );
  mgr::SaveToPDF( c, reconamer.PlotFileName( "convcomp", "log" ) );

  delete c;
  delete frame;

}

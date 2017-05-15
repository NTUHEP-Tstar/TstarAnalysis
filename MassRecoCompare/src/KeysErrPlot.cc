/*******************************************************************************
*
*  Filename    : KeysErrPlot.cc
*  Description : Keys PDF constructed from different nuisance parameters
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/KeysErrMgr.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"

#include <boost/format.hpp>
#include <iostream>

using namespace std;

void
MakeKeysErrPlots( KeysErrMgr* mgr )
{

  for( const auto histerr : histerrlist ){
    const string& tag = histerr.tag;

    TCanvas* c   = mgr::NewCanvas();
    TPad* toppad = mgr::NewTopPad();
    TPad* botpad = mgr::NewBottomPad();

    RooPlot* frame    = KeysErrMgr::x().frame();
    const double xmin = KeysErrMgr::x().getMin();
    const double xmax = KeysErrMgr::x().getMax();

    RooAbsPdf* central = mgr->Pdf( "" );
    RooAbsPdf* uppdf   = mgr->Pdf( tag+"Up" );
    RooAbsPdf* downpdf = mgr->Pdf( tag+"Down" );


    /*******************************************************************************
    *   Drawing top pad
    *******************************************************************************/
    toppad->Draw();
    toppad->cd();

    const double centralnorm = mgr->DataSet( "" )->sumEntries();
    const double upnorm      = ( tag == "pdf" || tag == "scale" ) ? centralnorm : mgr->DataSet( tag+"Up" )->sumEntries();
    const double downnorm    = ( tag == "pdf" || tag == "scale" ) ? centralnorm : mgr->DataSet( tag+"Down" )->sumEntries();

    TGraph* cenplot  = mgr::PlotOn( frame, central, RooFit::Normalization( centralnorm, RooAbsReal::NumEvent ) );
    TGraph* upplot   = mgr::PlotOn( frame, uppdf,   RooFit::Normalization( upnorm, RooAbsReal::NumEvent ) );
    TGraph* downplot = mgr::PlotOn( frame, downpdf, RooFit::Normalization( downnorm, RooAbsReal::NumEvent ) );
    frame->Draw();

    const double pltmax = mgr::GetYmax( cenplot, upplot, downplot );

    c->cd();
    /*******************************************************************************
    *   Drawing bottom plot
    *******************************************************************************/
    botpad->Draw();
    botpad->cd();
    TGraph* uprel   = mgr::DividedGraphSimple( upplot, cenplot );
    TGraph* downrel = mgr::DividedGraphSimple( downplot, cenplot );

    uprel->Draw( "AL" );
    downrel->Draw( "L" );

    TLine cenline( xmin, 1, xmax, 1  );
    TLine upline( xmin, 2, xmax, 2 );
    TLine downline( xmin, 0, xmax, 0 );

    cenline.Draw();
    upline.Draw();
    downline.Draw();

    c->cd();

    /*******************************************************************************
    *   Plot objects styling
    *******************************************************************************/
    mgr::SetTopPlotAxis( frame );
    frame->GetYaxis()->SetTitle( "Arbitrary unit" );
    frame->SetTitle( "" );
    frame->SetMinimum( 0.3 );
    frame->SetMaximum( pltmax * 1.8 );

    cenplot->SetLineColor( kBlack );
    upplot->SetLineColor( KBLUE );
    downplot->SetLineColor( KRED );
    uprel->SetLineColor( KBLUE );
    downrel->SetLineColor( KRED );

    mgr::SetBottomPlotAxis( uprel );
    uprel->GetXaxis()->SetTitle( frame->GetXaxis()->GetTitle() );
    uprel->GetXaxis()->SetRangeUser( xmin, xmax );
    uprel->GetYaxis()->SetTitle( "#frac{up,down}{central}" );
    uprel->SetMaximum( 2.2 );
    uprel->SetMinimum( -0.2 );

    upline.SetLineStyle( 3 );
    upline.SetLineColor( kBlack );
    downline.SetLineStyle( 3 );
    downline.SetLineColor( kBlack );

    /*******************************************************************************
    *   Drawing additional objects
    *******************************************************************************/
    mgr::DrawCMSLabel( SIMULATION );

    TLegend* legend    = mgr::NewLegend( 0.5, 0.75 );
    const string label = ( histerr.label != "" ) ? histerr.label : "1#sigma";
    legend->AddEntry( cenplot,  "Central Value",                                   "l" );
    legend->AddEntry( upplot,   ( histerr.rootname + "(+" + label + ")" ).c_str(), "l" );
    legend->AddEntry( downplot, ( histerr.rootname + "(-" + label + ")" ).c_str(), "l" );
    legend->Draw();

    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_TEXT_MAX, 0.75-TEXT_MARGIN, TOP_RIGHT )
    .WriteLine( mgr->LatexName() )
    .WriteLine( boost::str( boost::format( "M_{t*}=%dGeV/c^{2}" )%reconamer.GetInput<int>( "mass" ) ) );

    mgr::SaveToPDF( c, reconamer.PlotFileName( "errcomp", tag ) );

    delete uprel;
    delete downrel;
    delete toppad;
    delete botpad;
    delete frame;
    delete c;
  }
}

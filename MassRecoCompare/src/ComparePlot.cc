/*******************************************************************************
*
*  Filename    : ComparePlot.cc
*  Description : Making The comparison plots
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/NameParse.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/CompareHistMgr.hpp"

#include <algorithm>
#include <boost/format.hpp>
#include <string>
#include <vector>

#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"
#include "THStack.h"

using namespace std;
using namespace mgr;


void
ComparePlot( const string& comp_name, const vector<CompareHistMgr*> method_list  )
{
  static const double legend_xmin = 0.45;
  static const double legend_ymin = 0.70;

  for( const auto& histname : {"TstarMass", "ChiSq", "LepTopMass", "HadTopMass", "HadWMass"} ){

    TCanvas* c = mgr::NewCanvas();
    TLegend* l = mgr::NewLegend( legend_xmin, legend_ymin );

    mgr::SetSinglePad( c );

    double max = 0;

    for( const auto& method : method_list ){
      max = std::max( max, method->Hist( histname )->GetMaximum() );
    }

    for( const auto& method : method_list ){
      method->Hist( histname )->SetMaximum( max*1.5 );
      method->Hist( histname )->SetTitle( "" );
    }

    unsigned int i = 0;

    for( const auto& method : method_list ){
      method->SetLineColor( Color_Sequence[i] );
      mgr::SetAxis( method->Hist( histname ) );
      method->Hist( histname )->Draw( PS_HIST PS_SAME );
      l->AddEntry( method->Hist( histname ), method->LatexName().c_str(), "l" );
      ++i;
    }

    l->Draw();
    mgr::DrawCMSLabel( SIMULATION );

    TLatex ltx;
    ltx.SetNDC( kTRUE );
    ltx.SetTextFont( FONT_TYPE );
    ltx.SetTextSize( AXIS_TITLE_FONT_SIZE );
    ltx.SetTextAlign( TOP_RIGHT );
    ltx.DrawLatex( PLOT_X_MAX - 0.04, legend_ymin, str( boost::format( "M_{t*}=%dGeV/c^{2}" )%GetInt( reconamer.InputStr( "mass" ) ) ).c_str() );

    const string rootfile = reconamer.PlotRootFile();
    const string filename = reconamer.PlotFileName( comp_name, {histname} );
    mgr::SaveToPDF( c, filename );
    mgr::SaveToROOT( c, rootfile, Basename( filename ) );
    delete c;
    delete l;
  }
}

/*******************************************************************************
*   Monte-Carlo truth matching algorithm
*******************************************************************************/
void
MatchPlot( CompareHistMgr* mgr )
{
  MatchPlot1D( mgr );
  MatchPlot2D( mgr );
  MatchMassPlot( mgr );
}

/******************************************************************************/

void
MatchPlot2D( CompareHistMgr* mgr )
{
  TCanvas* c = new TCanvas( "c", "c", 750, 700 );

  c->SetLeftMargin( 0.2 );
  c->SetRightMargin( 0.15 );
  c->SetBottomMargin( PLOT_Y_MIN );
  c->SetTopMargin( 1 - PLOT_Y_MAX );


  TEfficiency* eff = new TEfficiency( *( mgr->Hist2D( "JetMatchPass" ) ), *( mgr->Hist2D( "JetMatchTotal" ) ) );

  TH2* plot = eff->CreateHistogram();

  plot->Draw( "COLZTEXT" );
  c->cd();

  mgr::SetAxis( plot );

  plot->SetMaximum( 1. );
  plot->SetMinimum( 0.01 );
  plot->SetStats( 0 );

  plot->SetTitle( mgr->LatexName().c_str() );

  plot->GetXaxis()->SetTitleOffset( 1.5 );
  plot->GetXaxis()->SetTitle( "Type from Fit" );
  plot->GetXaxis()->SetBinLabel( 1, "Lep. b" );
  plot->GetXaxis()->SetBinLabel( 2, "Lep. g" );
  plot->GetXaxis()->SetBinLabel( 3, "Had. W jet" );
  plot->GetXaxis()->SetBinLabel( 4, "Had. b" );
  plot->GetXaxis()->SetBinLabel( 5, "Had. g" );

  plot->GetYaxis()->SetTitleOffset( 3.2 );
  plot->GetYaxis()->SetTitle( "Type from MC Truth" );
  plot->GetYaxis()->SetBinLabel( 1, "Lep b" );
  plot->GetYaxis()->SetBinLabel( 2, "Lep g" );
  plot->GetYaxis()->SetBinLabel( 3, "Had W jet" );
  plot->GetYaxis()->SetBinLabel( 4, "Had b" );
  plot->GetYaxis()->SetBinLabel( 5, "Had g" );
  plot->GetYaxis()->SetBinLabel( 6, "unknown" );

  c->SetLogz( 1 );

  const string filename = reconamer.PlotFileName( "jetmatchmap", {mgr->Name()} );
  mgr::SaveToPDF( c, filename );
  mgr::SaveToROOT( c, reconamer.PlotRootFile(), Basename( filename ) );

  delete c;
}

/******************************************************************************/

void
MatchPlot1D( CompareHistMgr* mgr )
{
  TCanvas* c       = mgr::NewCanvas();
  TEfficiency* eff = new TEfficiency( *( mgr->Hist( "CorrPermPass" ) ), *( mgr->Hist( "CorrPermTotal" ) ) );
  eff->SetStatisticOption( TEfficiency::kBUniform ); // Uniform Bayesian

  TGraph* plot = eff->CreateGraph();
  TLegend* tl  = mgr::NewLegend( 0.5, 0.7 );

  mgr::SetSinglePad( c );
  plot->Draw( "APE" );
  mgr::DrawCMSLabel( SIMULATION );

  mgr::SetAxis( plot );
  tstar::SetDataStyle( plot );

  plot->GetXaxis()->SetLimits( -0.5, 7.5 );
  plot->SetMinimum( 0.01 );
  plot->SetMaximum( mgr::GetYmax( plot ) * 3 );
  plot->GetYaxis()->SetTitle( "Efficiency" );

  tl->AddEntry( plot, mgr->LatexName().c_str(), "PL" );
  tl->Draw();

  c->SetLogy( kTRUE );

  const string filename = reconamer.PlotFileName( "jetmatcheff", {mgr->Name()} );

  mgr::SaveToPDF( c, filename );
  mgr::SaveToROOT( c, reconamer.PlotRootFile(), Basename( filename ) );

  delete c;
  delete eff;
  delete plot;
}

/******************************************************************************/

void
MatchMassPlot( CompareHistMgr* mgr )
{
  static const int StackColorSeqeunce[7] = {
    TColor::GetColor( "#FFCC88" ),
    TColor::GetColor( "#996600" ),
    TColor::GetColor( "#FF3333" ),
    TColor::GetColor( "#33EEEE" ),
    TColor::GetColor( "#0066EE" ),
    TColor::GetColor( "#000000" ),
    TColor::GetColor( "#00EE00" ),
  };


  for( const auto& histname : {"TstarMass", "ChiSq", "LepTopMass", "HadTopMass", "HadWMass"} ){
    TCanvas* c     = mgr::NewCanvas();
    THStack* stack = new THStack( histname, "" );
    TLegend* tl    = mgr::NewLegend( 0.5, 0.6 );

    mgr::SetSinglePad( c );

    boost::format legend_entry( "%d jets correctly matched" );
    double histmax = 0;

    for( int i = 0; i <= 6; ++i ){
      TH1D* hist = mgr->Hist( mgr->CorrPermMassHistName( histname, i ) );
      hist->SetLineColor( StackColorSeqeunce[i] );
      hist->SetFillColor( StackColorSeqeunce[i] );
      hist->SetFillStyle( 1001 );

      tl->AddEntry( hist, str( legend_entry%i ).c_str(), "fl" );
      stack->Add( hist, "HIST" );
      histmax += mgr::GetYmax( hist );
    }

    stack->Draw();
    mgr::SetAxis( stack );
    stack->GetXaxis()->SetTitle( mgr->Hist( histname )->GetXaxis()->GetTitle() );
    stack->GetYaxis()->SetTitle( mgr->Hist( histname )->GetYaxis()->GetTitle() );

    tl->Draw();

    stack->SetMaximum( histmax * 1.5 );
    mgr::SaveToPDF( c, reconamer.PlotFileName( "jetmatchcomp", {mgr->Name(), histname} ) );
    c->SetLogy( kTRUE );
    stack->SetMaximum( histmax * 30 );
    mgr::SaveToPDF( c, reconamer.PlotFileName( "jetmatchcomp", {mgr->Name(), histname, "log"} ) );

    delete c;
    delete stack;
    delete tl;
  }
}

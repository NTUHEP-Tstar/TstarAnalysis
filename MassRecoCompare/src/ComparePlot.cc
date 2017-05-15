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
  static const double legend_xmin = PLOT_X_TEXT_MIN;
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
      method->Hist( histname )->SetMaximum( max*1.6 );
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
    mgr::DrawCMSLabelOuter( SIMULATION );

    boost::format entryfmt( "M_{t*}=%dGeV/c^{2}" );
    const string entry = boost::str( entryfmt % reconamer.GetInput<int>( "mass" ) );

    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_TEXT_MAX, legend_ymin-TEXT_MARGIN, TOP_RIGHT )
    .WriteLine( entry );

    const string rootfile = reconamer.PlotRootFile();
    const string filename = reconamer.PlotFileName( comp_name, {histname} );
    mgr::SaveToPDF( c, filename );
    mgr::SaveToROOT( c, rootfile, Basename( filename ) );
    delete c;
    delete l;
  }
}

/******************************************************************************/

void
CompareTstar( CompareHistMgr* mgr )
{
  static const double legend_xmin = 0.45;
  static const double legend_ymin = 0.70;

  TCanvas* c = mgr::NewCanvas();
  TLegend* l = mgr::NewLegend( legend_xmin, legend_ymin );

  mgr::SetSinglePad( c );

  double max = 0;

  const vector<string> masslist  = {"TstarMass", "LepTstarMass", "HadTstarMass" };
  const vector<string> massentry = {
    "Average value",
    "Leptonic",
    "Hadronic"
  };

  for( const auto& histname : masslist ){
    max = std::max( max, mgr->Hist( histname )->GetMaximum() );
  }

  for( const auto& histname : masslist ){
    mgr->Hist( histname )->SetMaximum( max*1.5 );
    mgr->Hist( histname )->SetTitle( "" );
  }

  unsigned int i = 0;

  for( const auto& histname : masslist ){
    mgr->Hist( histname )->SetLineColor( Color_Sequence[i] );
    mgr::SetAxis( mgr->Hist( histname ) );
    mgr->Hist( histname )->Draw( PS_HIST PS_SAME );
    l->AddEntry( mgr->Hist( histname ), massentry[i].c_str(), "l" );
    ++i;
  }

  l->Draw();
  mgr::DrawCMSLabel( SIMULATION );

  boost::format entryfmt( "M_{t*}=%dGeV/c^{2}" );
  const string entry = boost::str( entryfmt % reconamer.GetInput<int>( "mass" ) );

  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MAX, legend_ymin-TEXT_MARGIN, TOP_RIGHT )
  .WriteLine( entry )
  .WriteLine( mgr->LatexName() )
  .WriteLine( reconamer.GetChannelEXT( "Root Name" ) );

  const string rootfile = reconamer.PlotRootFile();
  const string filename = reconamer.PlotFileName( "tstarcomp", mgr->Name() );
  mgr::SaveToPDF( c, filename );
  mgr::SaveToROOT( c, rootfile, Basename( filename ) );
  delete c;
  delete l;

}


/*******************************************************************************
*   Monte-Carlo truth matching algorithm
*******************************************************************************/
void
MatchPlot( CompareHistMgr* mgr )
{
  CompareTstar( mgr );
  MatchPlot1D( mgr );
  MatchPlot2D( mgr );
  JetPTPlot2D( mgr );
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

  const string filename = reconamer.PlotFileName( "jetmatchmap", mgr->Name() );
  mgr::SaveToPDF( c, filename );
  mgr::SaveToROOT( c, reconamer.PlotRootFile(), Basename( filename ) );

  delete c;
}

/******************************************************************************/

void
JetPTPlot2D( CompareHistMgr* mgr )
{
  TCanvas* c = new TCanvas( "c", "c", 750, 700 );

  c->SetLeftMargin( 0.2 );
  c->SetRightMargin( 0.15 );
  c->SetBottomMargin( PLOT_Y_MIN );
  c->SetTopMargin( 1 - PLOT_Y_MAX );


  TEfficiency* eff = new TEfficiency( *( mgr->Hist2D( "JetPTMatch" ) ), *( mgr->Hist2D( "JetPTMatchTotal" ) ) );

  TH2* plot = eff->CreateHistogram();

  plot->Draw( "COLZTEXT" );
  c->cd();

  mgr::SetAxis( plot );

  plot->SetMaximum( 1. );
  plot->SetMinimum( 0.01 );
  plot->SetStats( 0 );

  plot->SetTitle( mgr->LatexName().c_str() );

  plot->GetXaxis()->SetTitleOffset( 1.5 );
  plot->GetXaxis()->SetTitle( "Jet Order" );
  plot->GetXaxis()->SetBinLabel( 1, "1^{st}" );
  plot->GetXaxis()->SetBinLabel( 2, "2^{nd}" );
  plot->GetXaxis()->SetBinLabel( 3, "3^{rd}" );
  plot->GetXaxis()->SetBinLabel( 4, "4^{th}" );
  plot->GetXaxis()->SetBinLabel( 5, "5^{th}" );
  plot->GetXaxis()->SetBinLabel( 6, "6^{th}" );

  plot->GetYaxis()->SetTitleOffset( 3.2 );
  plot->GetYaxis()->SetTitle( "Type from MC Truth" );
  plot->GetYaxis()->SetBinLabel( 1, "Lep b" );
  plot->GetYaxis()->SetBinLabel( 2, "Lep g" );
  plot->GetYaxis()->SetBinLabel( 3, "Had W jet" );
  plot->GetYaxis()->SetBinLabel( 4, "Had b" );
  plot->GetYaxis()->SetBinLabel( 5, "Had g" );
  plot->GetYaxis()->SetBinLabel( 6, "Non-signal" );
  plot->GetYaxis()->SetBinLabel( 6, "Unmatched" );
  plot->GetYaxis()->SetBinLabel( 7, "Match any" );

  c->SetLogz( 1 );

  const string filename = reconamer.PlotFileName( "jetptmatch", mgr->Name() );
  mgr::SaveToPDF( c, filename );
  mgr::SaveToROOT( c, reconamer.PlotRootFile(), Basename( filename ) );

  delete c;
}

/******************************************************************************/

void
MatchPlot1D( CompareHistMgr* mgr )
{
  {
    TCanvas* c       = mgr::NewCanvas();
    TEfficiency* eff = new TEfficiency( *( mgr->Hist( "CorrPermPass" ) ), *( mgr->Hist( "CorrPermTotal" ) ) );
    eff->SetStatisticOption( TEfficiency::kBUniform );// Uniform Bayesian

    TGraph* plot = eff->CreateGraph();
    TLegend* tl  = mgr::NewLegend( 0.5, 0.85 );

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
    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_TEXT_MAX, 0.85-TEXT_MARGIN, TOP_RIGHT )
    .WriteLine( boost::str( boost::format( "M_{t*}=%dGeV/c^{2}" )%reconamer.GetInput<int>( "mass" ) ) );


    c->SetLogy( kTRUE );

    const string filename = reconamer.PlotFileName( "jetmatcheff", mgr->Name() );

    mgr::SaveToPDF( c, filename );
    mgr::SaveToROOT( c, reconamer.PlotRootFile(), Basename( filename ) );

    delete c;
    delete eff;
    delete plot;
  }
  {
    TCanvas* c       = mgr::NewCanvas();
    TEfficiency* eff = new TEfficiency( *( mgr->Hist( "NumSigJets" ) ), *( mgr->Hist( "NumSigJetsTot" ) ) );
    eff->SetStatisticOption( TEfficiency::kBUniform );// Uniform Bayesian

    TGraph* plot = eff->CreateGraph();
    TLegend* tl  = mgr::NewLegend( 0.5, 0.85 );

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
    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_TEXT_MAX, 0.85-TEXT_MARGIN, TOP_RIGHT )
    .WriteLine( boost::str( boost::format( "M_{t*}=%dGeV/c^{2}" )%reconamer.GetInput<int>( "mass" ) ) );

    c->SetLogy( kTRUE );

    const string filename = reconamer.PlotFileName( "numsigjet", mgr->Name() );

    mgr::SaveToPDF( c, filename );
    mgr::SaveToROOT( c, reconamer.PlotRootFile(), Basename( filename ) );

    delete c;
    delete eff;
    delete plot;
  }
}

/******************************************************************************/

void
MatchMassPlot( CompareHistMgr* mgr )
{
  static const int StackColorSeqeunce[] = {
    TColor::GetColor( "#FFCC88" ),
    TColor::GetColor( "#FFCC88" ),
    TColor::GetColor( "#FFCC88" ),
    TColor::GetColor( "#996600" ),
    TColor::GetColor( "#FF3333" ),
    TColor::GetColor( "#33EEEE" ),
    TColor::GetColor( "#33EEEE" ),
  };


  for( const auto& histname : {"TstarMass", "ChiSq", "LepTopMass", "HadTopMass", "HadWMass"} ){
    TCanvas* c     = mgr::NewCanvas();
    THStack* stack = new THStack( histname, "" );
    TH1D* total    = (TH1D*)( mgr->Hist( mgr->CorrPermMassHistName( histname, 0 ) )->Clone() );
    TLegend* tl    = mgr::NewLegend( 0.4, 0.6 );

    mgr::SetSinglePad( c );

    boost::format legend_entry( ">=%d correct match (s=%.0lf)" );

    total->Reset();

    for( int i = 6; i >= 0; --i ){
      TH1D* hist = mgr->Hist( mgr->CorrPermMassHistName( histname, i ) );
      hist->SetLineColor( StackColorSeqeunce[i] );
      hist->SetFillColor( StackColorSeqeunce[i] );
      hist->SetFillStyle( 1001 );

      stack->Add( hist, "HIST" );
      total->Add( hist );

      if( i == 5 || i == 4 || i == 3 || i == 0  ){
        tl->AddEntry( hist, str( legend_entry%i%total->GetRMS() ).c_str(), "f" );
      }
    }

    const double histmax = mgr::GetYmax( total );

    stack->Draw();
    mgr::SetAxis( stack );
    stack->GetXaxis()->SetTitle( mgr->Hist( histname )->GetXaxis()->GetTitle() );
    stack->GetYaxis()->SetTitle( mgr->Hist( histname )->GetYaxis()->GetTitle() );

    tl->Draw();

    boost::format massfmt( "M_{t*}=%dGeV/c^{2}" );
    const string massstr = boost::str( massfmt % reconamer.GetInput<int>( "mass" ) );

    mgr::DrawCMSLabel( SIMULATION );
    mgr::LatexMgr latex;
    latex.SetOrigin( PLOT_X_MIN, PLOT_Y_MAX + TEXT_MARGIN/2, BOTTOM_LEFT )
    .WriteLine( reconamer.GetChannelEXT( "Root Name" ) )
    .SetOrigin( PLOT_X_TEXT_MAX, 0.6-TEXT_MARGIN, TOP_RIGHT )
    .WriteLine( mgr->LatexName() )
    .WriteLine( massstr );

    stack->SetMaximum( histmax * 1.5 );
    mgr::SaveToPDF( c, reconamer.PlotFileName( "jetmatchcomp", {mgr->Name(), histname} ) );
    c->SetLogy( kTRUE );
    stack->SetMaximum( histmax * 30 );
    mgr::SaveToPDF( c, reconamer.PlotFileName( "jetmatchcomp", {mgr->Name(), histname, "log"} ) );

    delete c;
    delete stack;
    delete tl;
  }

  const double tstarrms  = mgr->Hist( "TstarDiff" )->GetRMS();
  const double leptoprms = mgr->Hist( "LepTopDiff" )->GetRMS();
  const double hadtoprms = mgr->Hist( "HadTopDiff" )->GetRMS();
  const double hadwrms   = mgr->Hist( "HadWDiff" )->GetRMS();

  cout << ">>> Method: " << mgr->Name() << endl;
  cout << ">>> RMS of mass differences: "
       << tstarrms << " : "
       << leptoprms << " : "
       << hadtoprms << " : "
       << hadwrms << endl;
  cout << ">>> Relative RMS of mass differences: "
       << tstarrms/tstarrms << " : "
       << leptoprms / tstarrms << " : "
       << hadtoprms / tstarrms << " : "
       << hadwrms / tstarrms << endl;
}

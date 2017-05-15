/*******************************************************************************
*
*  Filename    : MakePileupWeightPlot.cc
*  Description : Writing pile up weights to a single file.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TFile.h"
#include "TH1D.h"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;


int
main( int argc, char const* argv[] )
{
  TFile* mcfile   = TFile::Open( "data/MCPileUp.root" );
  TFile* datafile = TFile::Open( "data/PileUp.root"   );
  TH1D* mchist    = (TH1D*)( mcfile->Get( "mcpu" )->Clone() );
  TH1D* datahist  = (TH1D*)( datafile->Get( "pileup" )->Clone() );
  datahist->Scale( 1/datahist->Integral() );

  TCanvas* c = mgr::NewCanvas();


  /******************************************************************************/
  TPad* top = mgr::NewTopPad();
  top->Draw();
  top->cd();
  mchist->Draw(PS_HIST);
  datahist->Draw(PS_SAME PS_DATA);
  mchist->SetStats(0);
  datahist->SetStats(0);

  mchist->SetFillColor(kCyan);
  mchist->SetTitle("");
  mchist->SetLineColor(kCyan);
  mgr::SetTopPlotAxis( mchist );
  tstar::SetDataStyle( datahist );


  c->cd();


  TPad* bot = mgr::NewBottomPad();
  bot->Draw();
  bot->cd();
  TH1D* rel = mgr::DivideHist( datahist, mchist, 1 );
  rel->Draw(PS_SAME PS_DATA);
  tstar::SetDataStyle( rel );
  mgr::SetBottomPlotAxis( rel );

  TLine* line     = new TLine( 0, 1, 75, 1 );
  TLine* line_top = new TLine( 0, 2, 75, 2 );
  TLine* line_bot = new TLine( 0, 0, 75, 0 );
  line->Draw( PS_SAME );
  line_top->Draw( PS_SAME );
  line_bot->Draw( PS_SAME );
  line->SetLineColor( kRed );
  line->SetLineStyle( 1 );
  line_top->SetLineColor( kBlack );
  line_bot->SetLineColor( kBlack );
  line_top->SetLineStyle( 3 );
  line_bot->SetLineStyle( 3 );

  c->cd();

  rel->GetXaxis()->SetTitle("Pileup");
  rel->SetTitle("");
  mchist->GetYaxis()->SetTitle("Probability");
  mchist->SetMaximum( std::max(mchist->GetMaximum(),datahist->GetMaximum())*1.4 );

  for( int i = 1 ; i < rel->GetNcells() ; ++i ){
    rel->SetBinError(i,0.01);
  }
  rel->SetMaximum(2.2);
  rel->SetMinimum(-0.2);
  rel->GetYaxis()->SetTitle("S.F.");

  TLegend* leg = mgr::NewLegend(0.6,0.7);
  leg->AddEntry(datahist,"Data","lp");
  leg->AddEntry(mchist,"Simulation","f");
  leg->Draw();

  mgr::DrawCMSLabel(PRELIMINARY);
  mgr::DrawLuminosity(35867.060);

  mgr::SaveToPDF( c, "results/pileupweighting.pdf" );

  mcfile->Close();
  datafile->Close();

  return 0;
}

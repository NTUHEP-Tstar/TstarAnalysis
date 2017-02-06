/*******************************************************************************
*
*  Filename    : Compare_Common.cc
*  Description : Implementation of the functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/RootMgr/interface/HistMgr.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"

#include <string>
#include <vector>

using namespace std;

// Common function for drawing standard ratio plot
void
MakePlot(
  THStack*                        stack,
  TH1D*                           bkgerror,
  TH1D*                           datahist,
  TH1D*                           sighist,
  TH1D*                           bkgrel,
  TH1D*                           datarel,
  TLegend*                        legend,
  const std::string               filenametag,
  const std::vector<std::string>& taglist
  )
{
  TCanvas* c = mgr::NewCanvas();

  const double xmin = datahist->GetXaxis()->GetXmin();
  const double xmax = datahist->GetXaxis()->GetXmax();

  // Drawing top canvas
  TPad* pad1 = mgr::NewTopPad();
  pad1->Draw();
  pad1->cd();
  stack->Draw( PS_HIST );  // Defined plot style in Common/interface/PlotStyle.hpp
  if( compnamer.CheckInput( "drawdata" ) ){
    datahist->Draw( PS_SAME PS_DATA );
  }
  bkgerror->Draw( PS_SAME PS_ERROR );
  sighist->Draw( PS_SAME PS_HIST );
  c->cd();

  // Drawing bottom canvas
  TPad* pad2      = mgr::NewBottomPad();
  TLine* line     = new TLine( xmin, 1, xmax, 1 );
  TLine* line_top = new TLine( xmin, 1.5, xmax, 1.5 );
  TLine* line_bot = new TLine( xmin, 0.5, xmax, 0.5 );
  pad2->Draw();
  pad2->cd();
  datarel->Draw( PS_AXIS );
  bkgrel->Draw( PS_SAME PS_ERROR );
  line->Draw( PS_SAME );
  line_top->Draw( PS_SAME );
  line_bot->Draw( PS_SAME );
  if( compnamer.CheckInput( "drawdata" ) ){
    datarel->Draw( PS_SAME PS_DATA );
  }
  c->cd();

  // Plot styling ( see Common/interface/PlotStyle.hpp )
  tstar::SetErrorStyle( bkgerror );
  tstar::SetErrorStyle( bkgrel   );
  tstar::SetDataStyle( datahist );
  tstar::SetDataStyle( datarel  );
  tstar::SetSignalStyle( sighist  );

  // Font and title settings
  mgr::SetTopPlotAxis( stack );
  stack->GetYaxis()->SetTitle( datahist->GetYaxis()->GetTitle() );

  mgr::SetBottomPlotAxis( datarel );
  datarel->GetXaxis()->SetTitle( sighist->GetXaxis()->GetTitle() );
  datarel->GetYaxis()->SetTitle( "Data/MC" );
  datarel->SetMaximum( 1.6 );
  datarel->SetMinimum( 0.4 );
  line->SetLineColor( kRed );
  line->SetLineStyle( 1 );
  line_top->SetLineColor( kBlack );
  line_bot->SetLineColor( kBlack );
  line_top->SetLineStyle( 3 );
  line_bot->SetLineStyle( 3 );

  // Writing captions
  legend->Draw();
  mgr::DrawCMSLabel();
  mgr::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );
  TPaveText* tb = mgr::NewTextBox(
    PLOT_X_MIN + 0.05,
    PLOT_Y_MAX - ( TEXT_FONT_SIZE*2.0 )/( DEFAULT_CANVAS_HEIGHT ),
    PLOT_X_MIN + 0.40,
    PLOT_Y_MAX - 0.025
    );
  tb->AddText( compnamer.GetChannelEXT( "Root Name" ).c_str() );
  tb->Draw();

  // setting ranges and saving plots
  const double ymax       = mgr::GetYmax( {bkgerror, datahist, sighist} );
  const unsigned exponent = mgr::HistMgr::GetExponent( ymax * 1.2 );
  const string xunit      = mgr::HistMgr::GetXUnit( datahist );
  const double binsize    = ( xmax-xmin )/( datahist->GetNbinsX() );
  const string newytitle  = mgr::HistMgr::MakeYTitle( binsize, xunit, exponent );
  stack->GetYaxis()->SetTitle( newytitle.c_str() );  // fancy y title
  stack->SetMaximum( ymax * 1.2 );

  mgr::SaveToPDF( c, compnamer.PlotFileName( filenametag, taglist ) );
  mgr::SaveToROOT( c,
    compnamer.PlotRootFile(),
    compnamer.OptFileName( "", filenametag, taglist )
    );

  // Saving Logged version for comparison
  vector<string> newtaglist = taglist;
  newtaglist.push_back( "log" );
  pad1->SetLogy();
  stack->SetMaximum( ymax * 30 );
  stack->SetMinimum( 0.3 );
  stack->GetYaxis()->SetTitle( datahist->GetYaxis()->GetTitle() );  // resetting y axis title
  mgr::SaveToPDF( c, compnamer.PlotFileName( filenametag, newtaglist ) );

  delete pad1;
  delete pad2;
  delete c;
  delete line;
  delete line_top;
  delete line_bot;
  delete tb;
}

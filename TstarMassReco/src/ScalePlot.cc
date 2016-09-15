/*******************************************************************************
*
*  Filename    : ScalePlot.cc
*  Description : Drawing the comparison plots for effects of scaling
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/NameParse.hpp"
#include "TstarAnalysis/TstarMassReco/interface/Bin_Common.hpp"
#include "TstarAnalysis/TstarMassReco/interface/ScaleHistMgr.hpp"

#include "TCanvas.h"
#include <boost/format.hpp>
#include <string>

using namespace std;

// ------------------------------------------------------------------------------
//   Main control flow
// ------------------------------------------------------------------------------
void MakePlot(
   TH1D*              centralplot,
   TH1D*              upplot,
   TH1D*              downplot,
   const std::string& legendentry,
   const std::string& filetag
   );

void
ScalePlot( ScaleHistMgr* mgr )
{
   MakePlot(
      mgr->Hist( "TstarMass" ),
      mgr->Hist( "TstarMass_resup" ),
      mgr->Hist( "TstarMass_resdw" ),
      "Jet resolution",
      "jer"
      );

   MakePlot(
      mgr->Hist( "TstarMass" ),
      mgr->Hist( "TstarMass_jecup" ),
      mgr->Hist( "TstarMass_jecdw" ),
      "Jet corrections",
      "jec"
      );

   MakePlot(
      mgr->Hist( "TstarMass" ),
      mgr->Hist( "TstarMass_btagup" ),
      mgr->Hist( "TstarMass_btagdw" ),
      "Jet corrections",
      "btag"
      );
}


void
MakePlot(
   TH1D*         centralplot,
   TH1D*         upplot,
   TH1D*         downplot,
   const string& legendentry,
   const string& tag
   )
{

   // Styling
   centralplot->SetLineColor( kBlack );
   upplot->SetLineColor( kRed );
   downplot->SetLineColor( kBlue );
   // Legend;
   TLegend* tl = plt::NewLegend( 0.5, 0.7 );
   tl->AddEntry( centralplot, "Central Value",                            "l" );
   tl->AddEntry( upplot,      ( legendentry+" (1 #sigma up)" ).c_str(),   "l" );
   tl->AddEntry( downplot,    ( legendentry+" (1 #sigma down)" ).c_str(), "l" );
   tl->Draw();

   TH1D* uprel = (TH1D*)upplot->Clone();
   uprel->Divide( centralplot );
   TH1D* downrel = (TH1D*)downplot->Clone();
   downrel->Divide( centralplot );

   // plotting
   TCanvas* c = new TCanvas( "c", "c", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );

   TPad* pad1 = plt::NewTopPad();
   pad1->Draw();
   pad1->cd();
   centralplot->Draw( "axis" );
   upplot->Draw( "LE SAME" );
   downplot->Draw( "LE SAME" );
   centralplot->Draw( "SAMEHIST" );
   tl->Draw( "same" );
   c->cd();

   const double xmin = centralplot->GetXaxis()->GetXmin();
   const double xmax = centralplot->GetXaxis()->GetXmax();

   TPad* pad2      = plt::NewBottomPad();
   TLine* line     = new TLine( xmin, 1, xmax, 1 );
   TLine* line_top = new TLine( xmin, 1.5, xmax, 1.5 );
   TLine* line_bot = new TLine( xmin, 0.5, xmax, 0.5 );
   pad2->Draw();
   pad2->cd();

   uprel->Draw( "axis" );
   uprel->Draw( "LE SAME" );
   downrel->Draw( "LE SAME" );
   line->Draw( "same" );
   line_top->Draw( "SAME" );
   line_bot->Draw( "SAME" );
   c->cd();

   // accessory styling
   line->SetLineColor( kBlack );
   line->SetLineStyle( 1 );
   line->SetLineWidth( 2 );
   line_top->SetLineColor( kBlack );
   line_bot->SetLineColor( kBlack );
   line_top->SetLineStyle( 3 );
   line_bot->SetLineStyle( 3 );

   plt::SetAxis( centralplot );
   plt::DisableXAxis( centralplot );
   plt::SetAxis( uprel );

   centralplot->SetMaximum( centralplot->GetMaximum() * 1.2 );
   uprel->SetMaximum( 2.2 );
   uprel->SetMinimum( -0.2 );
   uprel->GetXaxis()->SetTitleOffset( 5.5 );
   uprel->GetYaxis()->SetTitle( "#frac{up,down}{Norm}" );


   TPaveText* tb = plt::NewTextBox( 0.12, 0.88, 0.30, 0.94 );
   tb->AddText( reconamer.GetChannelEXT( "Root Name" ).c_str() );
   tb->Draw();

   TLatex ltx;
   ltx.SetNDC( kTRUE );
   ltx.SetTextFont( FONT_TYPE );
   ltx.SetTextSize( AXIS_TITLE_FONT_SIZE );
   ltx.SetTextAlign( TOP_RIGHT );
   try {
      ltx.DrawLatex( PLOT_X_MAX-0.02, 0.75, str( boost::format( "t* %dGeV" )%GetInt( reconamer.InputStr( "mass" ) ) ).c_str() );
   } catch( std::exception ){
      ltx.DrawLatex( PLOT_X_MAX-0.02, 0.75, "Data" );
   }

   // cleaning up
   if( reconamer.InputStr( "mass" ).find( "Data" ) != string::npos ){
      plt::DrawCMSLabel( PRELIMINARY );
      const mgr::ConfigReader cfg( reconamer.MasterConfigFile() );
      plt::DrawLuminosity( cfg.GetStaticDouble( "Total Luminosity" ) );
   } else {
      plt::DrawCMSLabel( SIMULATION );
   }
   c->SaveAs( reconamer.PlotFileName( "scale", {tag} ).c_str() );
   delete tl;
   delete c;
   delete uprel;
   delete downrel;

}

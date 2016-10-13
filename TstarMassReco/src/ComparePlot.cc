/*******************************************************************************
*
*  Filename    : ComparePlot.cc
*  Description : Making The comparison plots
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "TstarAnalysis/Common/interface/NameParse.hpp"
#include "TstarAnalysis/TstarMassReco/interface/CompareHistMgr.hpp"

#include <boost/format.hpp>
#include <string>
#include <vector>

using namespace std;

// ------------------------------------------------------------------------------
//   Declaring global objects
// ------------------------------------------------------------------------------
TstarNamer reconamer( "TstarMassReco" );

Color_t Color_Sequence[5] = {
   kBlue,
   kRed,
   kGreen,
   kBlack,
   kCyan
};

void
ComparePlot( const string& comp_name, const vector<CompareHistMgr*> method_list  )
{

   for( const auto& histname : method_list.front()->AvailableHistList() ){

      TCanvas* c = plt::NewCanvas();
      TLegend* l = plt::NewLegend( 0.45, 0.70 );

      double max = 0;

      for( const auto& method : method_list ){
         if( method->Hist( histname )->GetMaximum() > max ){
            max = method->Hist( histname )->GetMaximum();
         }
      }

      for( const auto& method : method_list ){
         method->Hist( histname )->SetMaximum( floor( max*1.2 ) );
         method->Hist( histname )->SetTitle( "" );
      }

      unsigned int i = 0;

      for( const auto& method : method_list ){
         method->SetLineColor( Color_Sequence[i] );
         plt::SetAxis( method->Hist( histname ) );
         method->Hist( histname )->Draw( "HIST SAME" );
         l->AddEntry( method->Hist( histname ), method->LatexName().c_str(), "l" );
         ++i;
      }

      l->Draw();
      plt::DrawCMSLabel( SIMULATION );

      TLatex ltx;
      ltx.SetNDC( kTRUE );
      ltx.SetTextFont( FONT_TYPE );
      ltx.SetTextSize( AXIS_TITLE_FONT_SIZE );
      ltx.SetTextAlign( TOP_LEFT );
      ltx.DrawLatex( 0.45, 0.70, str( boost::format( "M_{t*} = %dGeV/c^{2}" )%GetInt( reconamer.InputStr( "mass" ) ) ).c_str() );

      const string rootfile = reconamer.PlotRootFile();
      const string filename = reconamer.PlotFileName(comp_name,{histname});
      plt::SaveToPDF( c, filename );
      plt::SaveToROOT( c, rootfile, Basename(filename) );
      delete c;
      delete l;
   }
}

// ------------------------------------------------------------------------------
//   Single channel comparison plot
// ------------------------------------------------------------------------------
void
MatchPlot( CompareHistMgr* mgr )
{
   TCanvas* c = new TCanvas( "c", "c", 700, 700 );
   TPad* pad  = new TPad( "p", "p", 0.05, 0.01, 0.95, 1.0 );

   TH2D* plot = mgr->MatchMap();

   pad->Draw();
   pad->cd();
   plot->Draw( "COLZ TEXT" );
   c->cd();

   plt::SetAxis( plot );

   plot->Scale( 1./mgr->EventCount() );
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

   pad->SetLogz( 1 );

   const string filename  = reconamer.PlotFileName( "jetmatchmap", {mgr->Name()} );
   plt::SaveToPDF( c , filename );
   plt::SaveToROOT( c, reconamer.PlotRootFile(), Basename(filename) );

   delete pad;
   delete c;
}

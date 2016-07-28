/*******************************************************************************
 *
 *  Filename    : ComparePlot.cc
 *  Description : Making The comparison plots
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/CompareHistMgr.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"

#include <string>
#include <vector>

using namespace std;

//------------------------------------------------------------------------------
//   Declaring global objects
//------------------------------------------------------------------------------
TstarNamer reconamer("TstarMassReco");

Color_t Color_Sequence[5] = {
   kBlue,
   kRed,
   kGreen,
   kBlack,
   kCyan
};

void ComparePlot( const string& comp_name , const vector<CompareHistMgr*> method_list  )
{

   for( const auto& hist_name : method_list.front()->AvailableHistList() ){
      TCanvas* c = new TCanvas( "c", "c", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );
      TLegend* l = plt::NewLegend( 0.45,0.65);

      double max = 0 ;
      for( const auto& method : method_list ){
         if( method->Hist(hist_name)->GetMaximum() > max ){
            max = method->Hist(hist_name)->GetMaximum();
         }
      }
      for( const auto& method : method_list ){
         method->Hist(hist_name)->SetMaximum( floor(max*1.2) );
         method->Hist(hist_name)->SetTitle("");
      }

      unsigned int i = 0 ;
      for( const auto& method : method_list ){
         method->SetColor( Color_Sequence[i] );
         plt::SetAxis( method->Hist(hist_name) );
         if( i ) { method->Hist( hist_name )->Draw("HIST SAME"); }
         else    { method->Hist( hist_name )->Draw("HIST"); }
         l->AddEntry( method->Hist(hist_name) , method->LatexName().c_str() , "l" );
         ++i;
      }
      l->Draw();
      plt::DrawCMSLabel( SIMULATION );

      c->SaveAs( reconamer.PlotFileName(comp_name,{hist_name}).c_str() );
      delete c;
      delete l;
   }
}

//------------------------------------------------------------------------------
//   Single channel comparison plot
//------------------------------------------------------------------------------
void MatchPlot( CompareHistMgr* mgr )
{
   TCanvas* c = new TCanvas("c","c",700,700);
   TPad*  pad = new TPad("p","p",0.05,0.01,0.95,1.0);

   TH2D* plot = mgr->MatchMap();

   pad->Draw();
   pad->cd();
   plot->Draw("COLZ");
   c->cd();

   plt::SetAxis( plot );

   plot->Scale(1./mgr->EventCount());
   plot->SetMaximum(1.);
   plot->SetMinimum(0.01);
   plot->SetStats(0);

   plot->SetTitle( mgr->LatexName().c_str() );

   plot->GetXaxis()->SetTitleOffset(1.5);
   plot->GetXaxis()->SetTitle("Type from Fit");
   plot->GetXaxis()->SetBinLabel(1,"Lep. b");
   plot->GetXaxis()->SetBinLabel(2,"Lep. g");
   plot->GetXaxis()->SetBinLabel(3,"Had. W jet");
   plot->GetXaxis()->SetBinLabel(4,"Had. b");
   plot->GetXaxis()->SetBinLabel(5,"Had. g");

   plot->GetYaxis()->SetTitleOffset(3.2);
   plot->GetYaxis()->SetTitle("Type from MC Truth");
   plot->GetYaxis()->SetBinLabel(1,"Lep b");
   plot->GetYaxis()->SetBinLabel(2,"Lep g");
   plot->GetYaxis()->SetBinLabel(3,"Had W jet");
   plot->GetYaxis()->SetBinLabel(4,"Had b");
   plot->GetYaxis()->SetBinLabel(5,"Had g");
   plot->GetYaxis()->SetBinLabel(6,"unknown");

   pad->SetLogz(1);
   c->SaveAs( reconamer.PlotFileName( "jetmatchmap", {mgr->Name()} ).c_str() );

   delete pad;
   delete c;
}

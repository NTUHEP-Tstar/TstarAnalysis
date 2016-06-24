/*******************************************************************************
 *
 *  Filename    : KinematicCompare
 *  Description : Plotting function for Implementation
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"

#include <boost/range/adaptor/reversed.hpp>

using namespace std;

extern int GetInt( const string& );

void MakeComparePlot(
   SampleHistMgr* all_data,
   vector<SampleHistMgr*>& background,
   SampleHistMgr* signal_mgr,
   const string label = "" )
{
   const double total_lumi = mgr::SampleMgr::TotalLuminosity();

   background[0]->SetColor( TColor::GetColor("#FFCC88") );
   background[1]->SetColor( TColor::GetColor("#996600") );
   background[2]->SetColor( TColor::GetColor("#FF3333") );
   background[3]->SetColor( TColor::GetColor("#33EEEE") );
   background[4]->SetColor( TColor::GetColor("#0066EE") );

   for( const auto& hist_name : background.front()->AvailableHistList() ){
      const unsigned bins = background.front()->Hist(hist_name)->GetXaxis()->GetNbins();
      const double   xmin = background.front()->Hist(hist_name)->GetXaxis()->GetXmin();
      const double   xmax = background.front()->Hist(hist_name)->GetXaxis()->GetXmax();
      double         ymax = 0;
      THStack* stack      = new THStack("stack","");
      TH1D*    error_hist = new TH1D("error","",bins,xmin,xmax);
      TH1D*    data_hist  = new TH1D("data" ,"",bins,xmin,xmax);
      Parameter error(0,0,0);

      // Making stack and total error hist
      for( auto& sample : boost::adaptors::reverse(background) ){
         stack->Add( sample->Hist( hist_name ), "HIST" );
         error_hist->Add( sample->Hist(hist_name) );
         error += sample->ExpectedYield();
      }

      // Making data hist
      data_hist->Add( all_data->Hist(hist_name) );

      // Making relative error plot
      TH1D*  data_rel_hist = (TH1D*)data_hist->Clone("data_rel");
      TH1D*  rel_error     = (TH1D*)error_hist->Clone("rel_error");
      data_rel_hist->Divide( error_hist );
      double avgError = error.RelAvgError();
      for( int i = 0 ; i < error_hist->GetSize() ; ++i ){
         double binContent   = error_hist->GetBinContent(i);
         double binError     = error_hist->GetBinError(i);
         double dataBinError = data_hist->GetBinError(i);
         binError += binContent*avgError;
         error_hist->SetBinError(i,binError);
         if( binError + binContent > ymax ){
            ymax = binError + binContent;
         }

         rel_error->SetBinContent(i,1.0);
         if( binContent != 0 ){
            rel_error->SetBinError(i, binError/binContent);
            data_rel_hist->SetBinError(i,dataBinError/binContent);
         } else {
            rel_error->SetBinError(i,0.0);
            data_rel_hist->SetBinError(i,0.0);
         }
      }
      // Making Signal plots
      TH1D*  signal_hist   = signal_mgr->Hist(hist_name);
      const  double sig_scale = 500./signal_hist->Integral() ; // Force 500 events in display
      signal_hist->Scale( sig_scale );

      // Legend settings
      TLegend* l = plt::NewLegend(0.62,0.3);
      char data_entry[128];
      char sig_entry[128];
      sprintf( data_entry , "Data (%.3lf fb^{-1})" , total_lumi/1000. );
      sprintf( sig_entry  , "Signal_{M=%d} (#times %.0lf)", GetInt(signal_mgr->Name()) , sig_scale );
      l->AddEntry( data_hist , data_entry , "lp" );
      for( const auto& entry : background ){
         l->AddEntry( entry->Hist(hist_name), entry->LatexName().c_str()  , "f" );
      }
      l->AddEntry( error_hist , "Bkg. error" , "fl" );
      l->AddEntry( signal_hist, sig_entry , "fl");

      // The Plotting commands
      TCanvas* c = new TCanvas("c","c",DEFAULT_CANVAS_WIDTH,DEFAULT_CANVAS_HEIGHT);

      // Drawing the top canvas
      TPad* pad1 = new TPad( "pad1" , "pad1" , 0 , 0.30 , 1., 1.0 );
      pad1->SetBottomMargin(0.050);
      pad1->Draw();
      pad1->cd();
      stack->Draw("HIST");
      data_hist->Draw("same LPE1");
      error_hist->Draw("same E2");
      signal_hist->Draw("same HIST");
      l->Draw("same");
      c->cd();

      // Drawing bottom canvas
      TPad*   pad2 = new TPad( "pad2" , "pad2" , 0 , 0.10, 1, 0.30 );
      TLine*  line = new TLine( xmin, 1, xmax, 1 );
      TLine*  line_top = new TLine( xmin, 0, xmax, 0 );
      TLine*  line_bot = new TLine( xmin, 0, xmax, 0 );

      pad2->SetTopMargin(0.025);
      pad2->SetBottomMargin(0.010);
      pad2->Draw();
      pad2->cd();
      data_rel_hist->Draw("axis");
      data_rel_hist->Draw("same LPE1");
      rel_error->Draw("same E2");
      line->Draw("same");
      line_top->Draw("same");
      line_bot->Draw("same");
      c->cd();


      // Plot Settings
      error_hist->SetFillColor(kBlack);
      error_hist->SetLineColor(kBlack);
      error_hist->SetFillStyle(3004);
      rel_error->SetFillColor(kBlack);
      rel_error->SetLineColor(kBlack);
      rel_error->SetFillStyle(3004);
      rel_error->SetStats(0);
      data_hist->SetMarkerStyle(21);
      data_hist->SetMarkerSize(0.4);
      data_hist->SetLineColor(kBlack);
      data_hist->SetStats(0);
      data_rel_hist->SetMarkerStyle(21);
      data_hist->SetMarkerSize(0.4);
      data_rel_hist->SetLineColor(kBlack);
      data_rel_hist->SetStats(0);
      signal_hist->SetLineColor(kRed);
      signal_hist->SetFillColor(kRed);
      signal_hist->SetFillStyle(3005);

      // Font and title settings
      plt::SetAxis( stack );
      plt::DisableXAxis( stack );
      stack->GetYaxis()->SetTitle( signal_hist->GetYaxis()->GetTitle() );
      stack->SetMaximum( ymax * 1.2 );

      plt::SetAxis( data_rel_hist );
      data_rel_hist->GetXaxis()->SetTitleOffset(5.5);
      data_rel_hist->GetXaxis()->SetTitle( signal_hist->GetXaxis()->GetTitle() );
      data_rel_hist->GetYaxis()->SetTitle( "#frac{Data}{MC}");
      data_rel_hist->SetMaximum(2.2);
      data_rel_hist->SetMinimum(-0.2);
      line->SetLineColor(kRed);
      line->SetLineStyle(1);
      line_top->SetLineColor(kBlack);
      line_bot->SetLineColor(kBlack);
      line_top->SetLineStyle(3);
      line_bot->SetLineStyle(3);

      // Additional captions
      plt::DrawCMSLabel();
      plt::DrawLuminosity( total_lumi );

      TPaveText* tb = plt::NewTextBox( 0.15,0.75, 0.45,0.85 );
      tb->AddText( GetChannelPlotLabel().c_str() );

      c->SaveAs( GetKinematicPlotFile(hist_name+"_"+label).c_str() );

      delete pad1;
      delete pad2;
      delete c;
      delete l;
      delete data_hist;
      delete error_hist;
      delete stack;
      delete line;
      delete tb;
   }

}

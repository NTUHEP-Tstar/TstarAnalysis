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
      TH1D*    signal_hist   = signal_mgr->Hist(hist_name);

      // Legend settings
      TLegend* l = plt::NewLegend(0.62,0.3);
      char data_entry[128];
      sprintf( data_entry , "Data (%lg fb^{-1})" , total_lumi/1000. );
      l->AddEntry( data_hist , data_entry , "lp" );
      l->AddEntry( background[0]->Hist(hist_name), "t#bar{t} + Jets"  , "f" );
      l->AddEntry( background[1]->Hist(hist_name), "Single top"       , "f" );
      l->AddEntry( background[2]->Hist(hist_name), "t#bar{t} + Boson" , "f");
      l->AddEntry( background[3]->Hist(hist_name), "Single Boson"     , "f" );
      l->AddEntry( background[4]->Hist(hist_name), "Di-Boson"         , "f" );
      l->AddEntry( error_hist , "Background error" , "fl" );
      l->AddEntry( signal_hist, "t*#bar{t}* {}_{M_{t*}=700GeV} (10 pb)" , "fl");

      // The Plotting commands
      TCanvas* c = new TCanvas("c","c",650,500);

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
      TLine*  line = new TLine( error_hist->GetXaxis()->GetXmin() , 1 , error_hist->GetXaxis()->GetXmax() , 1 );
      pad2->SetTopMargin(0.025);
      pad2->SetBottomMargin(0.010);
      pad2->Draw();
      pad2->cd();
      data_rel_hist->Draw("axis");
      data_rel_hist->Draw("same LPE1");
      rel_error->Draw("same E2");
      line->Draw("same");
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
      data_hist->SetLineColor(kBlack);
      data_hist->SetStats(0);
      data_rel_hist->SetMarkerStyle(21);
      data_rel_hist->SetLineColor(kBlack);
      data_rel_hist->SetStats(0);
      signal_hist->SetLineColor(kRed);
      signal_hist->SetFillColor(kRed);
      signal_hist->SetFillStyle(3005);

      // Font and title settings
      const size_t font_size = 14;
      plt::SetAxis( stack );
      plt::DisableXAxis( stack );
      stack->GetYaxis()->SetTitle( signal_hist->GetYaxis()->GetTitle() );

      plt::SetAxis( data_rel_hist );
      data_rel_hist->GetXaxis()->SetTitleOffset(5.5);
      data_rel_hist->GetXaxis()->SetTitle( signal_hist->GetXaxis()->GetTitle() );
      data_rel_hist->GetYaxis()->SetTitle( "#frac{Data}{MC}");
      data_rel_hist->SetMaximum(2.0);
      data_rel_hist->SetMinimum(0.0);

      // Additional captions
      TLatex tl;
      tl.SetTextFont(43);
      tl.SetTextSize( font_size + 4 );

      tl.SetTextAlign(11);
      tl.DrawLatex( 0.1, 0.95 , "CMS at #sqrt{s} = 13TeV");

      tl.SetTextAlign(31);
      string channel_msg = "("+GetChannelPlotLabel() + " " + label+")";
      tl.DrawLatex( 0.9, 0.95 , channel_msg.c_str() );

      c->SaveAs( GetKinematicPlotFile(hist_name+"_"+label).c_str() );

      delete pad1;
      delete pad2;
      delete c;
      delete l;
      delete data_hist;
      delete error_hist;
      delete stack;
      delete line;
   }

}

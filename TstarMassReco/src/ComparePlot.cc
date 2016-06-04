/*******************************************************************************
 *
 *  Filename    : ComparePlot.cc
 *  Description : Making The comparison plots
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/

#include "TstarAnalysis/TstarMassReco/interface/CompareHistMgr.hh"

#include "TCanvas.h"
#include "TLegend.h"
#include "TAxis.h"

#include <string>
#include <vector>

using namespace std;

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
      TCanvas* c = new TCanvas( "c", "c", 650,500 );
      TLegend* l = new TLegend( 0.65,0.65,0.9,0.9);

      unsigned int i = 0 ;
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

      for( const auto& method : method_list ){
         method->SetColor( Color_Sequence[i] );
         if( i ) method->Hist( hist_name )->Draw("HIST SAME");
         else method->Hist( hist_name )->Draw("HIST");
         l->AddEntry( method->Hist(hist_name) , method->LatexName().c_str() , "l" );
         ++i;
      }
      l->Draw();

      c->SaveAs( (comp_name + hist_name + ".png").c_str() );
      delete c;
      delete l;
   }
}

void MatchRatePlot( const string& comp_name, const vector<CompareHistMgr*> method_list)
{
   TCanvas* c = new TCanvas("c","c", 650,500);
   TH1F* LepMatchRate  = new TH1F( "LepMatch"  , "Lepton Fit-Truth Match Rate;;Match rate" , method_list.size() , 0 , method_list.size() );
   TH1F* LepBMatchRate = new TH1F( "LepBMatch" , "Leptonic b-quark Fit-Truth Match Rate;;Match rate" , method_list.size() , 0 , method_list.size() );
   TH1F* LepGMatchRate = new TH1F( "LepGMatch" , "Leptonic gluon Fit-Truth Match Rate;;Match rate" , method_list.size() , 0 , method_list.size() );
   TH1F* HadBMatchRate = new TH1F( "HadBMatch" , "Hadronic b-quark Fit-Truth Match Rate;;Match rate" , method_list.size() , 0 , method_list.size() );
   TH1F* HadGMatchRate = new TH1F( "HadGMatch" , "Hadronic gluon Fit-Truth Match Rate;;Match rate" , method_list.size() , 0 , method_list.size() );
   LepMatchRate ->SetStats(0);
   LepBMatchRate->SetStats(0);
   LepGMatchRate->SetStats(0);
   HadBMatchRate->SetStats(0);
   HadGMatchRate->SetStats(0);
   LepMatchRate ->SetMinimum(0.);
   LepBMatchRate->SetMinimum(0.);
   LepGMatchRate->SetMinimum(0.);
   HadBMatchRate->SetMinimum(0.);
   HadGMatchRate->SetMinimum(0.);
   LepMatchRate ->SetMaximum(1.);
   LepBMatchRate->SetMaximum(1.);
   LepGMatchRate->SetMaximum(1.);
   HadBMatchRate->SetMaximum(1.);
   HadGMatchRate->SetMaximum(1.);

   int i = 0;
   for( const auto& method : method_list ){
      LepMatchRate ->SetBinContent( i+1 , method->LepMatchRate()  );
      LepBMatchRate->SetBinContent( i+1 , method->LepBMatchRate() );
      LepGMatchRate->SetBinContent( i+1 , method->LepGMatchRate() );
      HadBMatchRate->SetBinContent( i+1 , method->HadBMatchRate() );
      HadGMatchRate->SetBinContent( i+1 , method->HadGMatchRate() );
      LepMatchRate ->SetBinError( i+1 , sqrt(method->LepMatchRate() *(1-method->LepMatchRate() ) / method->EventCount() ) );
      LepBMatchRate->SetBinError( i+1 , sqrt(method->LepBMatchRate()*(1-method->LepBMatchRate() )/ method->EventCount() ) );
      LepGMatchRate->SetBinError( i+1 , sqrt(method->LepGMatchRate()*(1-method->LepGMatchRate() )/ method->EventCount() ) );
      HadBMatchRate->SetBinError( i+1 , sqrt(method->HadBMatchRate()*(1-method->HadBMatchRate() )/ method->EventCount() ) );
      HadGMatchRate->SetBinError( i+1 , sqrt(method->HadGMatchRate()*(1-method->HadGMatchRate() )/ method->EventCount() ) );
      LepMatchRate ->GetXaxis()->SetBinLabel(i+1,method->LatexName().c_str() );
      LepBMatchRate->GetXaxis()->SetBinLabel(i+1,method->LatexName().c_str() );
      LepGMatchRate->GetXaxis()->SetBinLabel(i+1,method->LatexName().c_str() );
      HadBMatchRate->GetXaxis()->SetBinLabel(i+1,method->LatexName().c_str() );
      HadGMatchRate->GetXaxis()->SetBinLabel(i+1,method->LatexName().c_str() );
      ++i;
   }

   LepMatchRate->Draw("E1");
   c->SaveAs( (comp_name+"LepMatchRate.png").c_str() );
   delete LepMatchRate;

   LepBMatchRate->Draw("E1");
   c->SaveAs( (comp_name+"LepBMatchRate.png").c_str() );
   delete LepBMatchRate;

   LepGMatchRate->Draw("E1");
   c->SaveAs( (comp_name+"LepGMatchRate.png").c_str() );
   delete LepGMatchRate;

   HadBMatchRate->Draw("E1");
   c->SaveAs( (comp_name+"HadBMatchRate.png").c_str() );
   delete HadBMatchRate;

   HadGMatchRate->Draw("E1");
   c->SaveAs( (comp_name+"HadGMatchRate.png").c_str() );
   delete HadGMatchRate;

   delete c;
}

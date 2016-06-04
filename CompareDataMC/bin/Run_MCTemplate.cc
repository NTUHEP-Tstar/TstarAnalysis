/*******************************************************************************
 *
 *  Filename    : Run_MCTemplate.cc
 *  Description : Interfacing with command line to run Higgs Combine Package
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include "TstarAnalysis/CompareDataMC/interface/PlotConfig.hh"
#include "TstarAnalysis/Utils/interface/SystemUtils.hh"
#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraphAsymmErrors.h"

using namespace std;

//------------------------------------------------------------------------------
//   Setting constants
//------------------------------------------------------------------------------
const vector<string>  signal_list = {
   "tstar_M700",
   "tstar_M800",
   "tstar_M900",
   "tstar_M1000",
   "tstar_M1100",
   "tstar_M1200",
   //   "tstar_M1300",
   "tstar_M1400",
   "tstar_M1500",
   "tstar_M1600"
};

const string method = "Asymptotic";

//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
extern int GetInt( const string& );

int main(int argc, char* argv[]) {

   if( argc != 2 ){
      cerr << "Error! Expected exactly one arguement!" << endl;
      return 1;
   }
   cout << "Making Higgs Combine Plots!" << endl;
   SetChannelType( argv[1] );

   for( const auto& signal : signal_list ){
      char cmd[1024];
      sprintf( cmd , "( combine -M %s -m %d %s &> /dev/null && mv %s %s )&" ,
         method.c_str(),
         GetInt(signal),
         GetTemplate_CardFile(signal).c_str(),
         GetHiggCombineOutputFile(method,GetInt(signal)).c_str(),
         GetTemplate_HiggCombineStoreFile(method,GetInt(signal)).c_str()
      );
      printf( "%s" , cmd );
      fprintf( stdout , "Submitting command for %s\n", signal.c_str() );
      system( cmd );
   }

   WaitProcess("combine");

   // Plotting the results
   const size_t binCount = signal_list.size();
   size_t bin = 0;
   double temp1;
   double mass[binCount] = {0};
   double masserr[binCount] ={0};
   double obs_lim[binCount] = {0};
   double exp_lim[binCount] = {0};
   double one_sig_up[binCount] = {0};
   double one_sig_down[binCount] = {0};
   double two_sig_up[binCount] = {0};
   double two_sig_down[binCount] = {0};
   for( const auto& signal : signal_list ){
      const string file_name = GetTemplate_HiggCombineStoreFile(method,GetInt(signal));
      TFile* file = TFile::Open(file_name.c_str());
      if( !file ){
         fprintf(stderr,"Cannot open file (%s), skipping sample for %s\n" ,
            file_name.c_str(),
            signal.c_str() );
         continue;
      }
      TTree* tree = ((TTree*)file->Get("limit"));
      tree->SetBranchAddress( "limit"    , &temp1 );
      // tree->SetBranchAddress( "limitErr" , &temp2 );

      tree->GetEntry(0); two_sig_down[bin] = temp1;
      tree->GetEntry(1); one_sig_down[bin] = temp1;
      tree->GetEntry(2); exp_lim[bin]      = temp1;
      tree->GetEntry(3); one_sig_up[bin]   = temp1;
      tree->GetEntry(4); two_sig_up[bin]   = temp1;

      tree->GetEntry(5);
      obs_lim[bin] = temp1;  // obs_err[bin] = temp2;

      mass[bin]    = GetInt( signal );
      masserr[bin] = 50.;

      two_sig_up[bin] -= exp_lim[bin];
      one_sig_up[bin] -= exp_lim[bin];
      two_sig_down[bin] = exp_lim[bin] - two_sig_down[bin];
      one_sig_down[bin] = exp_lim[bin] - one_sig_down[bin];
      file->Close();
      ++bin;
   }

   TCanvas* c1                = new TCanvas("c1", "c1", CANVAS_WIDTH, CANVAS_HEIGHT );
   TMultiGraph* mg            = new TMultiGraph();
   TGraphAsymmErrors* one_sig = new TGraphAsymmErrors(binCount,mass,exp_lim,masserr,masserr,one_sig_down,one_sig_up);
   TGraphAsymmErrors* two_sig = new TGraphAsymmErrors(binCount,mass,exp_lim,masserr,masserr,two_sig_down,two_sig_up);
   TGraph* exp                = new TGraph(binCount,mass,exp_lim);
   TGraph* obs                = new TGraph(binCount,mass,obs_lim);
   TLegend* l                 = new TLegend(0.55,0.65,0.90,0.90);

   //----- Setting Styles  --------------------------------------------------------
   one_sig->SetFillColor( kGreen  );
   one_sig->SetLineColor( kGreen  );
   one_sig->SetLineWidth(0);
   one_sig->SetFillStyle(1001);
   two_sig->SetFillColor( kYellow );
   two_sig->SetLineColor( kYellow );
   two_sig->SetLineWidth(0);
   two_sig->SetFillStyle(1002);

   exp->SetLineColor(kRed);
   exp->SetLineWidth(2);
   exp->SetLineStyle(2);

   obs->SetLineColor(kBlack);
   obs->SetLineWidth(2);
   obs->SetLineStyle(1);

   l->AddEntry( obs     , "CL_{s} Observed (2.256 fb^{-1})" , "l" );
   l->AddEntry( exp     , "CL_{s} Expected"                 , "l" );
   l->AddEntry( one_sig , "CL_{s} Expected #pm 1 #sigma"    , "f" );
   l->AddEntry( two_sig , "CL_{s} Expected #pm 2 #sigma"    , "f" );

   //----- Plotting  --------------------------------------------------------------
   mg->Add(two_sig);
   mg->Add(one_sig);
   mg->Add(exp);
   mg->Add(obs);
   mg->Draw("AL3");
   l->Draw();
   mg->GetXaxis()->SetTitle( "t^{*} Mass (GeV/c^{2})" );    // MUST Be after draw!!
   mg->GetYaxis()->SetTitle( "#sigma_{ex}/#sigma_{pred}" ); // https://root.cern.ch/root/roottalk/roottalk09/0078.html
   mg->GetXaxis()->SetLabelFont(43);
   mg->GetXaxis()->SetLabelSize(FONT_SIZE);
   mg->GetXaxis()->SetTitleFont(43);
   mg->GetXaxis()->SetTitleSize(FONT_SIZE);
   mg->GetYaxis()->SetLabelFont(43);
   mg->GetYaxis()->SetLabelSize(FONT_SIZE);
   mg->GetYaxis()->SetTitleFont(43);
   mg->GetYaxis()->SetTitleSize(FONT_SIZE);

   TLatex tl;
   tl.SetNDC(kTRUE);
   tl.SetTextFont(43);
   tl.SetTextSize( FONT_SIZE + 4 );
   tl.SetTextAlign(11);
   tl.DrawLatex( 0.1, 0.95 , "CMS at #sqrt{s} = 13TeV");
   tl.SetTextAlign(31);
   string channel_msg = "";
   if( GetChannel().find("Muon") != string::npos ){
      channel_msg="(#mu channel)";
   } else if( GetChannel().find("Electron") != string::npos ){
      channel_msg="(e channel)";
   }
   tl.DrawLatex( 0.9, 0.95 , channel_msg.c_str() );

   //----- Saving and cleaning up  ------------------------------------------------
   c1->SaveAs( GetTemplate_PlotFile().c_str() );
   delete one_sig;
   delete two_sig;
   delete obs;
   delete exp;
   delete mg;
   delete l;
   delete c1;


   return 0;
}

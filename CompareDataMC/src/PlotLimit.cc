/*******************************************************************************
 *
 *  Filename    : PlotLimit.cc
 *  Description : Macros For plotting confidence level limits
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include "TstarAnalysis/CompareDataMC/interface/PlotConfig.hh"

#include "ManagerUtils/SysUtils/interface/SystemUtils.hh"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"

#include <map>
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
//   Helper static functions and variables
//------------------------------------------------------------------------------
static map<double,double>  pred_cross_section;
static void FillXSectionMap();

void MakeLimitPlot() {

   mgr::ConfigReader cfg( GetJsonFile() );
   const vector<string> signal_list = cfg.GetStaticStringList( "SignalList" );
   const size_t binCount = signal_list.size();
   double y_max = 0;
   double y_min = 10000000.;
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

   FillXSectionMap();
   const size_t fine_bincount = pred_cross_section.size();
   double mass_fine[fine_bincount];
   double xsec_fine[fine_bincount];
   for( const auto& point : pred_cross_section ){
      mass_fine[bin] = point.first;
      xsec_fine[bin] = point.second;
      bin++;
   }

   bin=0;
   for( const auto& signal : signal_list ){
      double mass_d = GetSignalMass(signal);
      double exp_xsec = pred_cross_section[mass_d];
      double mass_t = GetSignalMass(signal);
      const string file_name = HCStoreFile(mass_t);
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

      tree->GetEntry(0); two_sig_down[bin] = temp1 * exp_xsec;
      tree->GetEntry(1); one_sig_down[bin] = temp1 * exp_xsec;
      tree->GetEntry(2); exp_lim[bin]      = temp1 * exp_xsec;
      tree->GetEntry(3); one_sig_up[bin]   = temp1 * exp_xsec;
      tree->GetEntry(4); two_sig_up[bin]   = temp1 * exp_xsec;

      tree->GetEntry(5); obs_lim[bin] = temp1 * exp_xsec;  // obs_err[bin] = temp2;

      mass[bin]    = mass_d; //see src/FileNames.cc
      masserr[bin] = 50.;

      cout << mass[bin] << " " << exp_xsec << " " << exp_lim[bin] << " "
           << two_sig_up[bin] << " " << two_sig_down[bin] << endl;
      two_sig_up[bin]   -= exp_lim[bin];
      one_sig_up[bin]   -= exp_lim[bin];
      two_sig_down[bin]  = exp_lim[bin] - two_sig_down[bin];
      one_sig_down[bin]  = exp_lim[bin] - one_sig_down[bin];
      file->Close();

      // Getting plot range
      if( exp_lim[bin] + two_sig_up[bin] > y_max ){
         y_max = exp_lim[bin] + two_sig_up[bin]; }
      if( obs_lim[bin] > y_max ){
         y_max = obs_lim[bin]; }

      if( exp_lim[bin] - two_sig_down[bin] < y_min ){
         y_min = exp_lim[bin] - two_sig_down[bin]; }
      if( obs_lim[bin] < y_min ){
         y_min = obs_lim[bin]; }

      ++bin;
   }

   TCanvas* c1                = new TCanvas("c1", "c1", CANVAS_WIDTH*1.2, CANVAS_HEIGHT );
   TMultiGraph* mg            = new TMultiGraph();
   TGraphAsymmErrors* one_sig = new TGraphAsymmErrors(binCount,mass,exp_lim,masserr,masserr,one_sig_down,one_sig_up);
   TGraphAsymmErrors* two_sig = new TGraphAsymmErrors(binCount,mass,exp_lim,masserr,masserr,two_sig_down,two_sig_up);
   TGraph* exp                = new TGraph(binCount,mass,exp_lim);
   TGraph* obs                = new TGraph(binCount,mass,obs_lim);
   TGraph* theory             = new TGraph(fine_bincount,mass_fine,xsec_fine);
   TLegend* l                 = new TLegend(0.55,0.65,0.90,0.90);

   //----- Setting Styles  --------------------------------------------------------
   one_sig->SetFillColor( kYellow  );
   one_sig->SetLineColor( kYellow  );
   one_sig->SetLineWidth(0);
   one_sig->SetFillStyle(1001);
   two_sig->SetFillColor( kGreen );
   two_sig->SetLineColor( kGreen );
   two_sig->SetLineWidth(0);
   two_sig->SetFillStyle(1002);

   exp->SetLineColor(kRed);
   exp->SetLineWidth(2);
   exp->SetLineStyle(2);

   obs->SetLineColor(kBlack);
   obs->SetLineWidth(2);
   obs->SetLineStyle(1);

   theory->SetLineColor(kBlue);
   theory->SetLineWidth(2);
   theory->SetLineStyle(2);

   char data_entry[1024];
   sprintf( data_entry , "CL_{s} Observed (%.3lf fb^{-1})", cfg.GetStaticDouble( "Luminosity" )/1000. );
   l->AddEntry( obs     , data_entry , "l" );
   l->AddEntry( exp     , "CL_{s} Expected"                 , "l" );
   l->AddEntry( one_sig , "CL_{s} Expected #pm 1 #sigma"    , "f" );
   l->AddEntry( two_sig , "CL_{s} Expected #pm 2 #sigma"    , "f" );
   l->AddEntry( theory  , "Prediction From Theory"          , "l" );

   //----- Plotting  --------------------------------------------------------------
   mg->Add(two_sig);
   mg->Add(one_sig);
   mg->Add(exp);
   mg->Add(obs);
   mg->Add(theory);
   mg->Draw("AL3");
   l->Draw();
   mg->GetXaxis()->SetTitle( "t^{*} Mass (GeV/c^{2})" );    // MUST Be after draw!!
   mg->GetYaxis()->SetTitle( "#sigma(pp#rightarrow t^{*}#bar{t}^{*}) (pb)" ); // https://root.cern.ch/root/roottalk/roottalk09/0078.html
   mg->GetXaxis()->SetLabelFont(43);
   mg->GetXaxis()->SetLabelSize(FONT_SIZE);
   mg->GetXaxis()->SetTitleFont(43);
   mg->GetXaxis()->SetTitleSize(FONT_SIZE);
   mg->GetYaxis()->SetLabelFont(43);
   mg->GetYaxis()->SetLabelSize(FONT_SIZE);
   mg->GetYaxis()->SetTitleFont(43);
   mg->GetYaxis()->SetTitleSize(FONT_SIZE);
   mg->GetYaxis()->SetTitleOffset(1.2);
   mg->GetXaxis()->SetLimits(650,1650);

   cout << y_min << " " << y_max ;
   mg->SetMaximum(y_max*30.);
   mg->SetMinimum(y_min*0.1);

   char buffer[1024];
   sprintf( buffer , "CMS@#sqrt{s}=13TeV (%s with %s)", GetMethodLabel().c_str(), GetFitFuncTag().c_str() );
   TLatex tl;
   tl.SetNDC(kTRUE);
   tl.SetTextFont(43);
   tl.SetTextSize( FONT_SIZE + 4 );
   tl.SetTextAlign(11);
   tl.DrawLatex( 0.1, 0.95 , buffer );
   tl.SetTextAlign(31);
   tl.DrawLatex( 0.9, 0.95 , GetChannelPlotLabel().c_str() );

   //----- Saving and cleaning up  ------------------------------------------------
   c1->SetLogy(kTRUE);
   c1->SaveAs( LimitPlotFile().c_str() );
   delete one_sig;
   delete two_sig;
   delete obs;
   delete exp;
   delete theory;
   delete mg;
   delete l;
   delete c1;
}


//------------------------------------------------------------------------------
//   Helper function implementations
//------------------------------------------------------------------------------
void FillXSectionMap()
{
   FILE* xsec_file  = fopen("data/excitedtoppair13TeV.dat","r");
   char*  line_buf = NULL ;
   size_t line_len = 0 ;
   double energy, mass, xsec_value ;
   while( getline(&line_buf, &line_len, xsec_file ) != -1) {
        sscanf( line_buf , "%lf%lf%lf", &energy, &mass, &xsec_value );
        pred_cross_section[mass] = xsec_value * 1000.;
   }
   if( line_buf ){
      free(line_buf);
   }
   fclose( xsec_file );

}

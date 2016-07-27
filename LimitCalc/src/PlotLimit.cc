/*******************************************************************************
 *
 *  Filename    : PlotLimit.cc
 *  Description : Macros For plotting confidence level limits
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"

#include <map>
#include <vector>
#include <string>
#include <cstdlib>

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraphAsymmErrors.h"

using namespace std;

//------------------------------------------------------------------------------
//   Helper static functions and variables
//------------------------------------------------------------------------------
static map<double,double>  pred_cross_section;
static void FillXSectionMap();

void MakeLimitPlot()
{
   const mgr::ConfigReader cfg( limit_namer.MasterConfigFile() );
   const vector<string> signal_list = cfg.GetStaticStringList( "Signal List" );
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
      double mass_d = GetInt(signal);
      double exp_xsec = pred_cross_section[mass_d];
      const string file_name = limit_namer.RootFileName("combine",{signal});
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

      mass[bin]    = mass_d;
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

   TCanvas* c1                = new TCanvas("c1", "c1", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );
   TGraphAsymmErrors* one_sig = new TGraphAsymmErrors(binCount,mass,exp_lim,masserr,masserr,one_sig_down,one_sig_up);
   TGraphAsymmErrors* two_sig = new TGraphAsymmErrors(binCount,mass,exp_lim,masserr,masserr,two_sig_down,two_sig_up);
   TGraph* exp                = new TGraph(binCount,mass,exp_lim);
   TGraph* obs                = new TGraph(binCount,mass,obs_lim);
   TGraph* theory             = new TGraph(fine_bincount,mass_fine,xsec_fine);

   //----- Setting Styles  --------------------------------------------------------
   one_sig->SetFillColor( kGreen +1 );
   one_sig->SetLineColor( kGreen +1 );
   one_sig->SetLineWidth(0);
   one_sig->SetFillStyle(1001);
   two_sig->SetFillColor( kOrange );
   two_sig->SetLineColor( kOrange );
   two_sig->SetLineWidth(0);
   two_sig->SetFillStyle(3001);

   exp->SetLineColor(kBlack);
   exp->SetLineWidth(2);
   exp->SetLineStyle(2);

   obs->SetLineColor(kBlack);
   obs->SetLineWidth(2);
   obs->SetLineStyle(1);

   theory->SetLineColor(kBlue);
   theory->SetLineWidth(2);
   theory->SetLineStyle(2);

   TMultiGraph* mg  = new TMultiGraph();
   mg->Add(two_sig);
   mg->Add(one_sig);
   mg->Add(exp);
   mg->Add(obs);
   mg->Add(theory);


   //----- Plotting  --------------------------------------------------------------
   mg->Draw("AL3");
   plt::SetAxis( mg );
   mg->GetXaxis()->SetTitle( "t* Mass (GeV/c^{2})" );    // MUST Be after draw!!
   mg->GetYaxis()->SetTitle( "#sigma(pp#rightarrow t*#bar{t}*) (pb)" ); // https://root.cern.ch/root/roottalk/roottalk09/0078.html
   mg->GetXaxis()->SetLimits( mass[0]-50 , mass[binCount-1]+50 );
   mg->SetMaximum(y_max*100.);
   mg->SetMinimum(y_min*0.3);


   const double legend_x_min = 0.55;
   const double legend_y_min = 0.65;
   TLegend* l  = plt::NewLegend(legend_x_min,legend_y_min);
   l->AddEntry( obs     , "95%% CL Limit (Obs.)" , "l" );
   l->AddEntry( exp     , "Medium expected limit", "l" );
   l->AddEntry( one_sig , "68%% expected"        , "f" );
   l->AddEntry( two_sig , "95%% expected"        , "f" );
   l->AddEntry( theory  , "Theory"               , "l" );
   l->Draw();


   plt::DrawCMSLabel();
   plt::DrawLuminosity( cfg.GetStaticDouble("Total Luminosity") );

   TLatex tl;
   tl.SetNDC(kTRUE);
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_LEFT );
   tl.DrawLatex( PLOT_X_MIN+0.02, PLOT_Y_MAX-0.02, limit_namer.GetChannelEXT("Root Name").c_str() );
   tl.DrawLatex( PLOT_X_MIN+0.02, PLOT_Y_MAX-0.09, (limit_namer.GetExtName("fitmethod","Full Name")+", "+limit_namer.GetExtName("fitfunc","Full Name")).c_str() );

   //----- Saving and cleaning up  ------------------------------------------------
   c1->SetLogy(kTRUE);
   c1->SaveAs( limit_namer.PlotFileName("limit").c_str() );
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
   FILE* xsec_file  = fopen(
      (limit_namer.SubPackageDir()+"data/excitedtoppair13TeV.dat").c_str(),"r");
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

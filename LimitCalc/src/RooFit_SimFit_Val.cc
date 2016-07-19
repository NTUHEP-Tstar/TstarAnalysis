/*******************************************************************************
 *
 *  Filename    : RooFit_SimFit_Val.cc
 *  Description : Validation of simfit method via signal injection
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"

#include <vector>
#include <string>
#include <iostream>

#include "TGraphAsymmErrors.h"

using namespace std;


void Validate( SampleRooFitMgr* data, SampleRooFitMgr* sig )
{
   smft::FitPDFs( data, sig );
   const RooAddPdf* originalfit      = (RooAddPdf*)data->GetPdfFromAlias( sig->Name() );
   const double     fit_yield        = ((RooRealVar*)(originalfit->coefList().at(1)))->getVal();
   const vector<double> inject_list  = val::MakeSigStrengthList( sig, fit_yield );
   const vector<string> new_set_list = val::GenInjected(data,sig,inject_list);
   for( const auto& new_set : new_set_list ){
      dataset_alias = new_set;
      
   }
   val::MakePlot( data, sig, inject_list, new_set_list );
}


//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
vector<double> val::MakeSigStrengthList( SampleRooFitMgr* sig, const double fit_yield )
{
   vector<double> ans;
   const double exp_yield=sig->ExpectedYield().CentralValue();
   ans.push_back( 1. )
   ans.push_back( 3.  );
   ans.push_back( 10. );
   ans.push_back( exp_yield/10. );
   ans.push_back( exp_yield/3.  );
   ans.push_back( exp_yield     );
   ans.push_back( exp_yield*3.  );
   ans.push_back( exp_yield*10. );
   return ans;
}

vector<string> val::GenInjected(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   const vector<double>& gen_strength_list
)
{
   RooAddPdf*  originalfit = (RooAddPdf*)data->GetPdfFromAlias( sig->Name() );
   RooRealVar* bkg_var     = (RooRealVar*)(originalfit->coefList().at(0));
   RooRealVar* sig_var     = (RooRealVar*)(originalfit->coefList().at(1));
   vector<string> ans ;
   unsigned i = 0;

   sig_var->setConstant(kFALSE);

   for( const auto& gen_strength : gen_strength_list ){
      const string new_alias = "gen" + to_string(i);
      const string new_name  = data->MakeDataAlias( new_alias );
      *sig_var = gen_strength ;// reassigning new signal strength

      cout << "Generating new dataset for " << sig->Name()  << " with " << gen_strength << "Signal events" << endl;
      RooDataSet*  new_set = originalfit->generate(
         SampleRooFitMgr::x(),
         bkg_var->getVal() + sig_var->getVal(),
         RooFit::Name( new_name.c_str() )
      );
      data->AddDataSet( new_set );
      ans.push_back( new_alias );
      ++i;
   }
   return ans;
}

//------------------------------------------------------------------------------
//   Plotting functions
//------------------------------------------------------------------------------

void FillGraph( TGraphAsymmErrors*, const int, const double x,  const RooRealVar* );
TCanvas* DrawGraph( TGraphAsymmErrors* ,
   const string& title,
   const string& yaxis );
void DrawBox( const RooRealVar*, const double xmin, const double xmax );
void DrawLine( const double, const double);

void val::MakePlot(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   const std::vector<double>&      sig_strength_list,
   const std::vector<std::string>& psuedo_data_alias_list
){
   const unsigned bincount = sig_strength_list.size();
   TGraphAsymmErrors*  bkg_strength_plot = new TGraphAsymmErrors(bincount);
   TGraphAsymmErrors*  sig_strength_plot = new TGraphAsymmErrors(bincount);
   TGraphAsymmErrors*  param_a_plot      = new TGraphAsymmErrors(bincount);
   TGraphAsymmErrors*  param_b_plot      = new TGraphAsymmErrors(bincount);

   for( unsigned i = 0 ; i < sig_strength_list.size(); ++i ){
      const double setsigstrength = sig_strength_list[i];
      const string fit_alias      = sig->Name() + "gen" + to_string(i);
      const string bkgfullname    = data->MakePdfAlias("bg"+fit_alias);
      const RooAddPdf*  model     = (RooAddPdf*)(data->GetPdfFromAlias( fit_alias ));
      const RooRealVar* bkgvar    = (RooRealVar*)(model->coefList().at(0));
      const RooRealVar* sigvar    = (RooRealVar*)(model->coefList().at(1));
      const RooRealVar* param_a   = var_mgr.FindByName("a"+bkgfullname);
      const RooRealVar* param_b   = var_mgr.FindByName("b"+bkgfullname);

      FillGraph(bkg_strength_plot,i,setsigstrength,bkgvar);
      FillGraph(sig_strength_plot,i,setsigstrength,sigvar);
      FillGraph(param_a_plot     ,i,setsigstrength,param_a);
      FillGraph(param_b_plot     ,i,setsigstrength,param_b);
   }
   const string bkgfullname  = data->MakePdfAlias("bg"+sig->Name());
   const RooRealVar*  bkgvar = var_mgr.FindByName("nb"+sig->Name());
   const RooRealVar*  param_a= var_mgr.FindByName("a"+bkgfullname);
   const RooRealVar*  param_b= var_mgr.FindByName("b"+bkgfullname);

   char title[1024];
   sprintf( title, "Comparison plot for mass point %d GeV", GetInt(sig->Name()));

   TCanvas* c_bkg = DrawGraph( bkg_strength_plot, title, "fitted bkg. strength" );
   DrawBox( bkgvar , sig_strength_list.front(), sig_strength_list.back() );
   c_bkg->SaveAs( limit_namer.PlotFileName("valplot",sig->Name()+"_bkg").c_str() );

   TCanvas* c_sig = DrawGraph( sig_strength_plot, title, "fitted signal strength");
   DrawLine( sig_strength_list.front(), sig_strength_list.back() );
   c_sig->SetLogy();
   c_sig->SaveAs( limit_namer.PlotFileName("valplot",sig->Name()+"_sig").c_str() );

   TCanvas* c_a = DrawGraph( param_a_plot, title, "fitted parameter [1]" );
   DrawBox( param_a , sig_strength_list.front() , sig_strength_list.back() );
   c_a->SaveAs( limit_namer.PlotFileName("valplot",sig->Name()+"_param_a").c_str() );

   TCanvas* c_b = DrawGraph( param_b_plot, title, "fitted parameter [2]" );
   DrawBox( param_b , sig_strength_list.front() , sig_strength_list.back() );
   c_b->SaveAs( limit_namer.PlotFileName("valplot",sig->Name()+"_param_b").c_str() );

   delete bkg_strength_plot;
   delete sig_strength_plot;
   delete param_a_plot;
   delete param_b_plot;
   delete c_bkg;
   delete c_sig;
}


// Helper plotting functions
void FillGraph( TGraphAsymmErrors* graph , const int bin, const double x , const RooRealVar* var )
{
   graph->SetPoint(bin,x,var->getVal() );
   graph->SetPointError(bin,0,0,-var->getErrorLo(),var->getErrorHi() );
}

TCanvas* DrawGraph( TGraphAsymmErrors* graph, const string& title , const string& y_title )
{
   TCanvas* c = new TCanvas(
      y_title.c_str(),y_title.c_str(),
      DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT
   );

   graph->Draw("ALP");
   graph->SetTitle(title.c_str());
   graph->GetXaxis()->SetTitle("injected signal strength");
   graph->GetYaxis()->SetTitle(y_title.c_str() );
   graph->SetMarkerStyle( 21);
   c->SetLogx();
   return c;
}

void DrawBox( const RooRealVar* var, const double xmin, const double xmax )
{
   double val = var->getVal();
   double low = var->getVal() + var->getErrorLo();
   double high= var->getVal() + var->getErrorHi();
   TLine* center = new TLine( xmin, val, xmax, val) ;
   TLine* lower  = new TLine( xmin, low, xmax, low );
   TLine* upper  = new TLine( xmin, high,xmax, high);

   center->SetLineColor(kBlue);
   lower->SetLineColor(kCyan);
   upper->SetLineColor(kCyan);
   center->Draw();
   lower->Draw();
   upper->Draw();
}

void DrawLine( const double xmin, const double xmax )
{
   TLine* diag = new TLine( xmin, xmin, xmax, xmax );
   diag->SetLineColor(kRed);
   diag->Draw();
}

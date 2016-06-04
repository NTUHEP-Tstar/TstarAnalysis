/*******************************************************************************
 *
 *  Filename    : RooFit_SideBand.cc
 *  Description : Implementations for making/saving/plotting results for side band method
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleRooFitMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include "TstarAnalysis/CompareDataMC/interface/PlotConfig.hh"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hh"

#include <stdio.h>
#include <gsl/gsl_integration.h> // Including stnadard gsl library

#include "RooGenericPdf.h"
#include "RooKeysPdf.h"
#include "RooPlot.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TBox.h"
#include "TLine.h"
#include "TMinuit.h"

using namespace std;
//------------------------------------------------------------------------------
//   Helper variables
//------------------------------------------------------------------------------
static double lower = 0.;
static double upper = 0.;
static RooDataSet*  data_reduced;

//------------------------------------------------------------------------------
//   Helper Functions
//------------------------------------------------------------------------------
int    GetInt( const string& );
double FitFunction(double,void*);
double GetIntegral(double*,double,double);
void   LogLikihoodFCN(int&,double*,double&,double*,int);

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------

void MakeSideBand( SampleRooFitMgr* data_mgr, SampleRooFitMgr* signal )
{
   printf("Making sideband objects for (%s)\n" , signal->Name().c_str() );

   const string name  = signal->Name();
   const double center = GetInt( name );
   const double minfitmass = SampleRooFitMgr::MinFitMass();
   const double maxmass    = SampleRooFitMgr::MaxMass();
   const double rms        = signal->OriginalDataSet()->sigma( SampleRooFitMgr::x() );
   lower = center - rms;
   upper = center + rms;
   char cut[1024];
   sprintf( cut , "((x>%lf&&x<%lf)||(x>%lf&&x<%lf))",
      minfitmass, lower,
      upper, maxmass );
   data_reduced = data_mgr->MakeReduceDataSet( name, RooFit::Cut(cut) );

   // Calling TMinuit for self defined log likelihood fit
   double m_val, m_err, b_val, b_err;
   TMinuit* my_minuit = new TMinuit(2);
   my_minuit->SetPrintLevel(-1); // Suppressing output
   my_minuit->SetFCN( &LogLikihoodFCN );
   my_minuit->DefineParameter(0, "m", 150., 100., 1., 1000. );
   my_minuit->DefineParameter(1, "b", 150., 100., 1., 1000. );
   my_minuit->Command("MINI");
   my_minuit->Command("MINOS");
   my_minuit->GetParameter(0,m_val,m_err);
   my_minuit->GetParameter(1,b_val,b_err);
   delete my_minuit;

   printf("[%lf] %lf %lf %lf %lf\n",
      center,m_val,m_err,b_val,b_err);

   // Making background pdf
   RooRealVar m("m","m",100,1000);
   RooRealVar b("b","b",100,1000);
   const string pdf_name = data_mgr->MakePdfAlias( name );
   RooGenericPdf* bg_pdf = new RooGenericPdf(
      pdf_name.c_str() , pdf_name.c_str() ,
      "1/(1+exp((x-m)/b))",
      RooArgSet(SampleRooFitMgr::x(),m,b) );
   data_mgr->AddPdf(bg_pdf);
   m = m_val ; m.setError(m_err);
   b = b_val ; m.setError(b_err);
   m.setConstant(kTRUE);
   b.setConstant(kTRUE);

   // Making signal pdf
   char signal_cut[1024];
   sprintf( signal_cut , "(x>%lf&&x<%lf)" , lower, upper );
   RooDataSet* signal_reduced = signal->MakeReduceDataSet( "selc" , RooFit::Cut(signal_cut) );
   const string signal_name = signal->MakePdfAlias("selc");
   RooKeysPdf* signal_pdf = new RooKeysPdf(
      signal_name.c_str(), signal_name.c_str(),
      SampleRooFitMgr::x(), *signal_reduced );
   signal->AddPdf( signal_pdf );


   // Get Normalization
   double parm[2] = { m_val , b_val };
   const double data_yield = data_mgr->GetReduceDataSet( GetDataSetName() )->numEntries();
   const double norm_over_whole = GetIntegral( parm , minfitmass, maxmass );
   const double norm_over_selc  = GetIntegral( parm , minfitmass, lower ) + GetIntegral( parm, upper, maxmass );
   const double exp_yield = data_reduced->numEntries() * norm_over_whole / norm_over_selc ;

   // Plotting objects
   RooPlot* frame = SampleRooFitMgr::x().frame();

   TGraph* data_plot   = PlotOn( frame, data_mgr->OriginalDataSet() );
   bg_pdf->plotOn( frame , RooFit::Range("FitRange") , RooFit::Normalization(exp_yield,RooAbsReal::NumEvent));
   TGraph* bg_plot = (TGraph*)frame->getObject( frame->numItems() -1 );
   TGraph* sig_set_plot =  PlotOn( frame , signal_reduced );
   TGraph* signal_plot = PlotOn( frame, signal_pdf ,
      RooFit::Range(lower-0.5*rms,upper+0.5*rms),
      RooFit::DrawOption("B") );

   // TCanvas and other plotted objects  object
   TCanvas* c = new TCanvas("c","c", 650,500);
   frame->Draw();
   frame->SetMinimum(0.3);
   frame->SetTitle("");
   frame->GetXaxis()->SetLabelFont(43);
   frame->GetXaxis()->SetLabelSize( FONT_SIZE );
   frame->GetXaxis()->SetTitleFont(43);
   frame->GetXaxis()->SetTitleSize( FONT_SIZE );
   frame->GetXaxis()->SetTitleOffset( 1.2 );
   frame->GetYaxis()->SetLabelFont(43);
   frame->GetYaxis()->SetLabelSize( FONT_SIZE);
   frame->GetYaxis()->SetTitleFont(43);
   frame->GetYaxis()->SetTitleSize( FONT_SIZE );
   frame->GetYaxis()->SetTitleOffset( 1.2 );
   frame->SetTitle("");

   TBox* box = new TBox( lower, 0,  upper, 0.8*(frame->GetMaximum())  );
   box->Draw("SAME");
   box->SetFillStyle(3444);
   box->SetFillColorAlpha(kGray,0.5);
   box->SetLineStyle(3);
   box->SetLineWidth(4);
   box->SetLineColor(kGray);

   bg_plot->Draw("SAME");
   sig_set_plot->Draw("SAME");
   signal_plot->Draw("SAME");

   TLegend* l = new TLegend(0.6, 0.6, 0.9, 0.9);
   char leg_entry[1024];
   sprintf( leg_entry , "Data (%.3lf fb^{-1})" , signal->Sample()->TotalLuminosity()/1000. );
   l->AddEntry( data_plot   , leg_entry , "lp" );
   l->AddEntry( bg_plot     , "Fitted background" , "l" );
   sprintf( leg_entry , "Signal (%dGeV @ %.1lf pb)" , GetInt(signal->Name()), signal->Sample()->CrossSection().CentralValue() );
   l->AddEntry( signal_plot , leg_entry , "fl" );
   l->Draw("SAME");
   //l->SetTextSizePixels( FONT_SIZE );

   TLatex* lt = new TLatex();
   lt->SetNDC(kTRUE);
   lt->SetTextFont(43);
   lt->SetTextSize(FONT_SIZE+4);
   lt->DrawLatex( 0.1, 0.93, "CMS at #sqrt{s} = 13TeV");
   lt->SetTextAlign(31);
   lt->DrawLatex( 0.9, 0.93 , "(#mu channel)");
   lt->SetTextAlign(33);
   lt->DrawLatex(0.6,0.85 , "f(m) = N #left(1 + exp#left( #frac{m-a}{b} #right)#right)^{-1}" );

   char exp_mesg[1024];
   sprintf(exp_mesg,"Expected: %.0lf", data_yield );
   lt->SetTextAlign(11);
   lt->DrawLatex(0.4,0.75, exp_mesg);
   sprintf(exp_mesg,"Estimated: %.2lf", exp_yield );
   lt->DrawLatex(0.4,0.70, exp_mesg);
   sprintf(exp_mesg,"#Delta_{Total}: %.2lf", 100*fabs( (exp_yield-data_yield)/data_yield ) );
   lt->DrawLatex(0.4,0.65, exp_mesg);

   data_plot->SetMarkerSize(1);
   data_plot->SetMarkerStyle(21);
   signal_plot->SetFillStyle(3004);
   signal_plot->SetLineColor(kRed);
   signal_plot->SetFillColor(kRed);
   sig_set_plot->SetLineColor(kRed);
   sig_set_plot->SetMarkerColor(kRed);

   c->SaveAs( ("temp"+name+".png").c_str() );
   c->SetLogy(1);
   c->SaveAs( ("temp"+name+"_log.png").c_str() );

   delete c;
   return ;
}

void MakeCheckPlot(SampleRooFitMgr* data)
{
   data_reduced = data->GetReduceDataSet("FitRange");

   // Calling TMinuit for self defined log likelihood fit
   double m_val, m_err, b_val, b_err;
   TMinuit* my_minuit = new TMinuit(2);
   my_minuit->SetPrintLevel(-1); // Suppressing output
   my_minuit->SetFCN( &LogLikihoodFCN );
   my_minuit->DefineParameter(0, "m", 150., 100., 1., 1000. );
   my_minuit->DefineParameter(1, "b", 150., 100., 1., 1000. );
   my_minuit->Command("MINI");
   my_minuit->Command("MINOS");
   my_minuit->GetParameter(0,m_val,m_err);
   my_minuit->GetParameter(1,b_val,b_err);
   delete my_minuit;


   // Making background pdf
   RooRealVar m("m","m",100,1000);
   RooRealVar b("b","b",100,1000);
   const string pdf_name = data->MakePdfAlias( "none" );
   RooGenericPdf* bg_pdf = new RooGenericPdf(
      pdf_name.c_str() , pdf_name.c_str() ,
      "1/(1+exp((x-m)/b))",
      RooArgSet(SampleRooFitMgr::x(),m,b) );
   data->AddPdf(bg_pdf);
   m = m_val ; m.setError(m_err);
   b = b_val ; m.setError(b_err);
   m.setConstant(kTRUE);
   b.setConstant(kTRUE);

   TCanvas* c = new TCanvas("c","c",CANVAS_WIDTH,CANVAS_HEIGHT);
   TLegend* l = new TLegend(0.55,0.5,0.9,0.9);
   RooPlot* frame = SampleRooFitMgr::x().frame();
   TGraph* data_set_plot = PlotOn( frame , data->OriginalDataSet() );
   TGraph* pdf_plot    = PlotOn( frame , data->GetPdfFromAlias("none"), RooFit::Range("FitRange") );
   frame->Draw();
   frame->SetMinimum(0.03);
   l->AddEntry(data_set_plot,"Data","lp");
   l->AddEntry(pdf_plot ,"MC Template fit (Norm)","l");
   l->Draw();
   c->SaveAs("check_fitdata_fit_vs_data.png");
   c->SetLogy();
   c->SaveAs("check_fitdata_fit_vs_data_log.png");
}


//------------------------------------------------------------------------------
//   Helper function implementations
//------------------------------------------------------------------------------
int GetInt( const string& x )
{
   string ans_string;
   for( auto y : x ){
      if( y >= '0' && y <= '9'){
         ans_string.push_back(y);
      }
   }
   return stoi(ans_string);
}


//------------------------------------------------------------------------------
//   Fit Function
//------------------------------------------------------------------------------
struct f_parms {
   double m;
   double b;
};

double FitFunction(double x, void* p_raw )
{
   f_parms* parms = (f_parms*)p_raw;
   const double m = parms->m;
   const double b = parms->b;
   return 1/(1+exp((x-m)/b));
}

double GetIntegral( double* parm , double min, double max)
{
   gsl_integration_workspace* w = gsl_integration_workspace_alloc(10000);
   gsl_function F;
   double norm, error;
   f_parms*  parms = new f_parms;

   F.function = &FitFunction;
   parms->m = parm[0];
   parms->b = parm[1];
   F.params   = parms;
   gsl_integration_qag( &F,
      min,
      max,
      1e-10,
      0.,
      1000,
      GSL_INTEG_GAUSS21,
      w,
      &norm,
      &error
   );
   gsl_integration_workspace_free(w);
   delete parms;

   return norm;
}


void LogLikihoodFCN(int&, double*, double& f, double* parm, int)
{
   const double norm = GetIntegral(
      parm,
      SampleRooFitMgr::MinFitMass(),
      SampleRooFitMgr::MaxMass() );
   f_parms*  parms = new f_parms;
   parms->m = parm[0];
   parms->b = parm[1];

   f = 0.;
   for( int i = 0 ; i < data_reduced->numEntries() ; ++i ){
      double x = data_reduced->get(i)->getRealValue("x");
      f -= 2. * log( FitFunction(x,parms)/norm );
   }
   delete parms;
}

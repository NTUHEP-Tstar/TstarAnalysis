/*******************************************************************************
 *
 *  Filename    : RooFit_MCTemplate.cc
 *  Description : Plotting to file for template methods.cc
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"

#include "TFile.h"
#include "RooFit.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGenericPdf.h"
#include "RooKeysPdf.h"
#include "RooPlot.h"
#include "RooWorkspace.h"
#include "RooFitResult.h"

#include <string>
#include <fstream>

using namespace std;

//------------------------------------------------------------------------------
//   Helper variables
//------------------------------------------------------------------------------
namespace tmplt {
   RooFitResult* MakeBGFromMC( SampleRooFitMgr* );
   void MakeTemplatePlot( SampleRooFitMgr*, SampleRooFitMgr*, SampleRooFitMgr*, RooFitResult*, const bool );
   void MakeCardFile(SampleRooFitMgr*, SampleRooFitMgr*, SampleRooFitMgr*);
};

//------------------------------------------------------------------------------
//   Function implemetations
//------------------------------------------------------------------------------
void MakeTemplate( SampleRooFitMgr* data, SampleRooFitMgr* bg, vector<SampleRooFitMgr*>& signal_list )
{
   using namespace tmplt;
   vector<RooAbsPdf*> sig_pdf_list;

   for( auto& sig : signal_list ){ sig_pdf_list.push_back( MakeKeysPdf(sig) ); }
   RooFitResult* err = MakeBGFromMC(bg);
   MakeTemplatePlot( data, bg, signal_list.front(), err , true );
   MakeTemplatePlot( data, bg, signal_list.front(), err , false );


   SaveRooWorkSpace(
      data->GetDataFromAlias( limit_namer.GetTag("fitset") ),
      {bg->GetPdfFromAlias("fit")},
      sig_pdf_list,
      {err}
   ); // See src/RooFit_Common.cc

   for( auto& signal : signal_list ){ MakeCardFile(data,bg,signal); }
}



//------------------------------------------------------------------------------
//  Helper function implementations
//------------------------------------------------------------------------------
RooFitResult* tmplt::MakeBGFromMC( SampleRooFitMgr* bg )
{
   RooGenericPdf* bg_pdf;

   if( limit_namer.GetFitFunc() == "Exo" ){
      bg_pdf = MakeExo( bg, "fit" );
   } else if( limit_namer.GetFitFunc() == "Fermi" ){
      bg_pdf = MakeFermi( bg, "fit" );
   } else {
      bg_pdf = MakeFermi( bg, "fit" );
   }

   RooFitResult* results =  bg_pdf->fitTo( *(bg->OriginalDataSet()) ,
      RooFit::Save() ,            // Suppressing output
      RooFit::SumW2Error(kTRUE),  // For Weighted dataset
      RooFit::Range("FitRange"),  // Fitting range
      RooFit::Minos(kTRUE),
      RooFit::Verbose(kFALSE),
      RooFit::PrintLevel(-1)
   );
   var_mgr.SetConstant();
   return results;
}

void tmplt::MakeCardFile( SampleRooFitMgr* data, SampleRooFitMgr* bg, SampleRooFitMgr* sig )
{
   RooDataSet* data_obs = data->GetReduceDataSet(limit_namer.GetTag("fitset"));
   RooAbsPdf*  bg_pdf   = bg->GetPdfFromAlias("fit");
   RooDataSet* sig_set  = sig->OriginalDataSet();
   RooAbsPdf*  sig_pdf  = sig->GetPdfFromAlias("key");

   FILE* card_file = MakeCardCommon( data_obs, bg_pdf, sig_pdf, sig->Name() );

   fprintf(
      card_file , "%12s %15lf %15lf\n",
      "rate",
      sig_set->sumEntries(),
      data_obs->sumEntries()
   );

   fprintf( card_file , "----------------------------------------\n" );
   const Parameter lumi(1,0.05,0.05);
   const Parameter null(0,0,0);
   const Parameter sig_unc = sig->Sample()->SelectionEfficiency();
   const Parameter bkg_unc = lumi;
   PrintNuisanceFloats( card_file, "Lumi",    "lnN", lumi   , lumi    );
   PrintNuisanceFloats( card_file, "sig_unc", "lnN", sig_unc, null    );
   PrintNuisanceFloats( card_file, "bg_unc" , "lnN", null   , bkg_unc );

   fclose( card_file );
}


//------------------------------------------------------------------------------
//   Plotting fitting results
//------------------------------------------------------------------------------
void tmplt::MakeTemplatePlot(
   SampleRooFitMgr* data,
   SampleRooFitMgr* mc,
   SampleRooFitMgr* signal,
   RooFitResult*    err,
   const bool       use_data )
{
   // First plot against MC
   const double TotalLuminosity = limit_namer.MasterConfig().GetStaticDouble( "Total Luminosity" );
   TCanvas* c = new TCanvas("c","c",DEFAULT_CANVAS_WIDTH,DEFAULT_CANVAS_HEIGHT);
   RooPlot* frame = SampleRooFitMgr::x().frame();
   TGraph* pdf_plot_err;
   TGraph* pdf_plot;
   TGraph* sig_plot;
   TGraph* set_plot;


   if( use_data ){
      set_plot = PlotOn( frame, data->OriginalDataSet() );
      pdf_plot_err= PlotOn(
         frame, mc->GetPdfFromAlias("fit"),
         RooFit::VisualizeError(*err,1),
         RooFit::Range("FitRange"),
         RooFit::Normalization( data->OriginalDataSet()->sumEntries() , RooAbsReal::NumEvent )
      );
      pdf_plot = PlotOn(
         frame, mc->GetPdfFromAlias("fit"),
         RooFit::Range("FitRange") ,
         RooFit::Normalization( data->OriginalDataSet()->sumEntries() , RooAbsReal::NumEvent )
      );
      set_plot = PlotOn( frame, data->OriginalDataSet() );
   } else {
      set_plot = PlotOn( frame, mc->OriginalDataSet() );
      pdf_plot_err= PlotOn(
         frame, mc->GetPdfFromAlias("fit"),
         RooFit::VisualizeError(*err,1),
         RooFit::Range("FitRange"),
         RooFit::Normalization( mc->OriginalDataSet()->sumEntries() , RooAbsReal::NumEvent )
      );
      pdf_plot = PlotOn(
         frame, mc->GetPdfFromAlias("fit"),
         RooFit::Range("FitRange") ,
         RooFit::Normalization( mc->OriginalDataSet()->sumEntries() , RooAbsReal::NumEvent )
      );
      set_plot = PlotOn( frame, data->OriginalDataSet() );
   }
   sig_plot = PlotOn(
      frame, signal->GetPdfFromAlias("key"),
      RooFit::DrawOption("LB"),
      RooFit::Normalization( signal->Sample()->ExpectedYield().CentralValue(), RooAbsReal::NumEvent )
   );

   frame->Draw();
   frame->SetMinimum(0.3);
   SetFrame(frame,AXIS_TITLE_FONT_SIZE);
   frame->GetYaxis()->SetTitle( set_plot->GetYaxis()->GetTitle() );
   pdf_plot_err->SetFillStyle(1);
   pdf_plot_err->SetFillColor(kCyan);
   sig_plot->SetFillStyle(3004);
   sig_plot->SetLineColor(kRed);
   sig_plot->SetFillColor(kRed);

   char norm[1024];
   char sig_entry[1024];
   if( use_data ){
      sprintf( norm, "Template fit (from MC)");
   } else {
      sprintf( norm, "Template fit (Normalized to Data)");
   }

   sprintf( sig_entry , "Signal (%.2lf pb)" , signal->Sample()->CrossSection().CentralValue() );

   const double legend_x_min = 0.55;
   const double legend_y_min = 0.70;
   TLegend* l = plt::NewLegend(legend_x_min,legend_y_min);
   if( use_data ){
      l->AddEntry( set_plot, "Data", "lp");
      l->AddEntry( pdf_plot, "Template fit (Normalized to Data)",  "l");
   } else {
      l->AddEntry( set_plot, "MC Background", "lp");
      l->AddEntry( pdf_plot, "Template fit (Normalized to Lumi)",  "l");
   }
   l->AddEntry( pdf_plot_err, "Fit Error (1#sigma)" , "fl");
   l->AddEntry( sig_plot, sig_entry , "fl");
   l->Draw();

   plt::DrawCMSLabel();
   plt::DrawLuminosity( TotalLuminosity );
   TLatex tl;
   tl.SetNDC(kTRUE);
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.05, limit_namer.GetChannelRoot().c_str() );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.12, limit_namer.GetFitFuncRoot().c_str() );

   if( use_data ){
      c->SaveAs( limit_namer.PlotFileName( "fitplot", signal->Name()+"_fitmc-vs-data").c_str() );
      c->SetLogy();
      c->SaveAs( limit_namer.PlotFileName( "fitplot", signal->Name()+"_fitmc-vs-data_log").c_str() );
   } else {
      c->SaveAs( limit_namer.PlotFileName( "fitplot", signal->Name()+"_fitmc-vs-mc").c_str() );
      c->SetLogy();
      c->SaveAs( limit_namer.PlotFileName( "fitplot", signal->Name()+"_fitmc-vs-mc_log").c_str() );
   }
   delete c;
   delete l;
}

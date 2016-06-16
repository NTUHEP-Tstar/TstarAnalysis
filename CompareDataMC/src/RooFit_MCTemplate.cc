/*******************************************************************************
 *
 *  Filename    : RooFit_MCTemplate.cc
 *  Description : Plotting to file for template methods.cc
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleRooFitMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/VarMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/MakePDF.hh"
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include "TstarAnalysis/CompareDataMC/interface/PlotConfig.hh"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hh"

#include "RooFit.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGenericPdf.h"
#include "RooKeysPdf.h"
#include "RooPlot.h"
#include "RooWorkspace.h"
#include "RooFitResult.h"

#include "TFile.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include <string>
#include <fstream>

using namespace std;

//------------------------------------------------------------------------------
//   Static variables
//------------------------------------------------------------------------------
static const string ws_name = "wspace";

//------------------------------------------------------------------------------
//   Helper variables
//------------------------------------------------------------------------------
namespace tmplt {
   RooFitResult* MakeBGFromMC( SampleRooFitMgr* );
   void MakeTemplatePlot( SampleRooFitMgr* , SampleRooFitMgr*, SampleRooFitMgr*, RooFitResult*, const bool );
   void SaveRooWorkSpace( SampleRooFitMgr*, SampleRooFitMgr*, vector<SampleRooFitMgr*>&, RooFitResult* );
   void MakeCardFile(SampleRooFitMgr*, SampleRooFitMgr*, SampleRooFitMgr*);
};

//------------------------------------------------------------------------------
//   Function implemetations
//------------------------------------------------------------------------------
void MakeTemplate( SampleRooFitMgr* data, SampleRooFitMgr* bg, vector<SampleRooFitMgr*>& signal_list )
{
   using namespace tmplt;
   RooFitResult* err = MakeBGFromMC(bg);
   for( auto& signal : signal_list ){
      MakeKeysPdf( signal , "fit" );
   }
   MakeTemplatePlot( data, bg, signal_list.front(), err , true );
   MakeTemplatePlot( data, bg, signal_list.front(), err , false );

   //------------------------------------------------------------------------------
   //   Saving all relavent obejcts by RooWorkSpace
   //------------------------------------------------------------------------------
   SaveRooWorkSpace( data, bg, signal_list, err  );

   //------------------------------------------------------------------------------
   //   Making Higgs Combine Data Cards
   //------------------------------------------------------------------------------
   for( auto& signal : signal_list ){
      MakeCardFile(data,bg,signal);
   }

}



//------------------------------------------------------------------------------
//  Helper function implementations
//------------------------------------------------------------------------------
RooFitResult* tmplt::MakeBGFromMC( SampleRooFitMgr* bg )
{
   RooGenericPdf* bg_pdf;

   if( GetFitFunc() == "Exo" ){
      bg_pdf = MakeExo( bg, "fit" );
   } else if( GetFitFunc() == "Fermi" ){
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

void tmplt::SaveRooWorkSpace( SampleRooFitMgr* data, SampleRooFitMgr* bg, vector<SampleRooFitMgr*>& sig_list, RooFitResult* err )
{
   const string roofit_file = GetRooObjFile();
   cout << "Saving RooFit objects to " << roofit_file << endl;
   RooWorkspace ws( ws_name.c_str() , ws_name.c_str() );
   ws.import( *(data->GetReduceDataSet(GetDataSetName())) );
   ws.import( *(bg->GetPdfFromAlias("fit")) );
   for( auto& signal : sig_list ){
      ws.import( *(signal->GetPdfFromAlias("fit")) );
   }
   ws.writeToFile( roofit_file.c_str() );

   TFile* fit_file = TFile::Open( GetFitResults().c_str() , "RECREATE" );
   err->Write("results");
   fit_file->Close();
   delete fit_file;
}


void tmplt::MakeCardFile( SampleRooFitMgr* data, SampleRooFitMgr* bg, SampleRooFitMgr* signal )
{
   const string cardfile_name = GetCardFile( signal->Name() );
   RooDataSet* data_obs       = data->GetReduceDataSet(GetDataSetName());
   RooAbsPdf*  bg_pdf         = bg->GetPdfFromAlias("fit");
   RooDataSet* signal_dataset = signal->OriginalDataSet();
   RooAbsPdf*  signal_pdf     = signal->GetPdfFromAlias("fit");

   FILE* cardfile = fopen( cardfile_name.c_str() , "w" );

   // Priting header
   fprintf( cardfile , "imax 1\n" );
   fprintf( cardfile , "jmax *\n" );
   fprintf( cardfile , "kmax *\n" );
   fprintf( cardfile , "----------------------------------------\n" );

   // Printing objects
   fprintf( cardfile , "shapes %10s %15s %30s %s:%s\n" ,
      "bg",
      GetChannel().c_str() ,
      GetRooObjFile().c_str() ,
      ws_name.c_str(),
      bg_pdf->GetName()
   );
   fprintf( cardfile , "shapes %10s %15s %30s %s:%s\n" ,
      "sig",
      GetChannel().c_str() ,
      GetRooObjFile().c_str() ,
      ws_name.c_str(),
      signal_pdf->GetName()
   );
   fprintf( cardfile , "shapes %10s %15s %30s %s:%s\n" ,
      "data_obs",
      GetChannel().c_str() ,
      GetRooObjFile().c_str() ,
      ws_name.c_str(),
      data_obs->GetName()
   );
   fprintf( cardfile , "----------------------------------------\n" );

   // Printing data correspondence
   fprintf( cardfile , "%12s %s\n" , "bin" , GetChannel().c_str() );
   fprintf( cardfile , "%12s %lg\n" , "observation" , data_obs->sumEntries() );
   fprintf( cardfile , "----------------------------------------\n" );

   // Printing expected
   fprintf( cardfile , "%12s %15s %15s\n" , "bin"     , GetChannel().c_str(), GetChannel().c_str() );
   fprintf( cardfile , "%12s %15s %15s\n" , "process" , "sig", "bg" );
   fprintf( cardfile , "%12s %15s %15s\n" , "process" , "-1" , "1" );
   fprintf( cardfile , "%12s %15lf %15lf\n" , "rate",
   signal_dataset->sumEntries(),
   data_obs->sumEntries() );

   // Listing Nuisance parameters
   fprintf( cardfile , "----------------------------------------\n" );
   const string sig_unc = "1.05";
   fprintf( cardfile, "%8s lnN %15s %15s\n" , "Lumi"   , "1.05"          , "1.05" );
   fprintf( cardfile, "%8s lnN %15s %15s\n" , "sig_unc", sig_unc.c_str() , "--"   );
   fprintf( cardfile, "%8s lnN %15s %15s\n" , "bg_unc" , "--"            , "1.05" );

   cout << "Writing to " << cardfile_name << "..." << endl;
   fclose( cardfile );
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
   const double TotalLuminosity = StaticCfg().GetStaticDouble( "Total Luminosity" );
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
      frame, signal->GetPdfFromAlias("fit"),
      RooFit::DrawOption("LB"),
      RooFit::Normalization( signal->Sample()->ExpectedYield().CentralValue(), RooAbsReal::NumEvent )
   );

   frame->Draw();
   frame->SetMinimum(0.3);
   SetFrame(frame,FONT_SIZE);
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
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.05, GetChannelPlotLabel().c_str() );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.12, GetFitFuncFormula().c_str() );

   if( use_data ){
      c->SaveAs( GetRooObjPlot(signal->Name() + "_fitmc_vs_data").c_str() );
      c->SetLogy();
      c->SaveAs( GetRooObjPlot(signal->Name() + "_fitmc_vs_data_log").c_str() );
   } else {
      c->SaveAs( GetRooObjPlot( signal->Name() + "_fitmc_vs_mc").c_str() );
      c->SetLogy();
      c->SaveAs( GetRooObjPlot( signal->Name() + "_fitmc_vs_mc_log").c_str() );
   }
   delete c;
   delete l;
}

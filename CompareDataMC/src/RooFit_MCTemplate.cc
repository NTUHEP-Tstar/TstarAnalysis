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
   void MakeTemplatePlot( SampleRooFitMgr* , SampleRooFitMgr*, SampleRooFitMgr*, RooFitResult* );
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
   MakeTemplatePlot( data, bg, signal_list.front(), err );

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
void tmplt::MakeTemplatePlot( SampleRooFitMgr* data, SampleRooFitMgr* mc, SampleRooFitMgr* signal, RooFitResult* err )
{
   // First plot against MC
   TCanvas* c1 = new TCanvas("c1","c1",CANVAS_WIDTH,CANVAS_HEIGHT);
   RooPlot* frame_1 = SampleRooFitMgr::x().frame();
   mgr::ConfigReader cfg( GetJsonFile() );
   const double TotalLuminosity = cfg.GetStaticDouble( "Luminocity" );

   TGraph* pdf_plot_err= PlotOn(
      frame_1 , mc->GetPdfFromAlias("fit"),
      RooFit::VisualizeError(*err,1),
      RooFit::Range("FitRange"),
      RooFit::Normalization( mc->OriginalDataSet()->sumEntries() , RooAbsReal::NumEvent )
   );
   TGraph* pdf_plot    = PlotOn(
      frame_1 , mc->GetPdfFromAlias("fit"),
      RooFit::Range("FitRange") ,
      RooFit::Normalization( mc->OriginalDataSet()->sumEntries() , RooAbsReal::NumEvent )
   );
   TGraph* sig_plot_1  = PlotOn(
      frame_1 , signal->GetPdfFromAlias("fit"),
      RooFit::DrawOption("LB"),
      RooFit::Normalization( signal->Sample()->ExpectedYield().CentralValue(), RooAbsReal::NumEvent )
   );
   TGraph* mc_set_plot = PlotOn( frame_1 , mc->OriginalDataSet() );

   frame_1->Draw();
   frame_1->SetMinimum(0.3);
   SetFrame(frame_1,FONT_SIZE);
   frame_1->GetYaxis()->SetTitle( mc_set_plot->GetYaxis()->GetTitle() );
   pdf_plot_err->SetFillStyle(1);
   pdf_plot_err->SetFillColor(kCyan);
   sig_plot_1->SetFillStyle(3004);
   sig_plot_1->SetLineColor(kRed);
   sig_plot_1->SetFillColor(kRed);

   char data_entry[1024];
   char sig_entry[1024];
   sprintf( data_entry, "MC Background (%.3lf fb^{-1})", TotalLuminosity/1000. );
   sprintf( sig_entry , "Signal (%.2lf pb)" , signal->Sample()->CrossSection().CentralValue() );

   TLegend* l1 = new TLegend(0.55,0.7,0.9,0.9);
   l1->AddEntry(mc_set_plot, data_entry, "lp");
   l1->AddEntry(pdf_plot   , "Template fit","l");
   l1->AddEntry(pdf_plot_err, "Fit Error (1#sigma)" , "fl");
   l1->AddEntry(sig_plot_1 , sig_entry , "fl");
   l1->Draw();

   TLatex* tl = new TLatex();
   tl->SetNDC(kTRUE);
   tl->SetTextFont(43);
   tl->SetTextSize( FONT_SIZE + 4 );
   tl->SetTextAlign(11);
   tl->DrawLatex( 0.1, 0.93, "CMS at #sqrt{s} = 13TeV");
   tl->SetTextAlign(31);
   tl->DrawLatex( 0.9, 0.93 , GetChannelPlotLabel().c_str() );
   tl->SetTextAlign(33);
   tl->DrawLatex(0.88,0.65 , GetFitFuncFormula().c_str() );

   c1->SaveAs( GetRooObjPlot( signal->Name() + "_fitmc_vs_mc").c_str() );
   c1->SetLogy();
   c1->SaveAs( GetRooObjPlot( signal->Name() + "_fitmc_vs_mc_log").c_str() );

   TCanvas* c2 = new TCanvas("c2","c2",CANVAS_WIDTH,CANVAS_HEIGHT);
   RooPlot* frame_2 = SampleRooFitMgr::x().frame();

   TGraph* pdf_plot_err_2 = PlotOn(
      frame_2, mc->GetPdfFromAlias("fit"),
      RooFit::VisualizeError(*err,1),
      RooFit::Range("FitRange"),
      RooFit::Normalization( data->OriginalDataSet()->sumEntries(), RooAbsReal::NumEvent )
   );
   TGraph* pdf_plot_2 = PlotOn(
      frame_2, mc->GetPdfFromAlias("fit"),
      RooFit::Range("FitRange"),
      RooFit::Normalization( data->OriginalDataSet()->sumEntries(), RooAbsReal::NumEvent )
   );
   TGraph* sig_plot_2 = PlotOn(
      frame_2 , signal->GetPdfFromAlias("fit"),
      RooFit::DrawOption("LB"),
      RooFit::Normalization( signal->Sample()->ExpectedYield().CentralValue(), RooAbsReal::NumEvent )
   );
   TGraph* data_set_plot  = PlotOn( frame_2 , data->OriginalDataSet() );

   frame_2->Draw();
   frame_2->SetMinimum(0.3);
   SetFrame(frame_2,FONT_SIZE);
   frame_2->GetYaxis()->SetTitle( data_set_plot->GetYaxis()->GetTitle() );

   pdf_plot_err_2->SetFillStyle(1);
   pdf_plot_err_2->SetFillColor(kCyan);
   sig_plot_2->SetFillStyle(3004);
   sig_plot_2->SetLineColor(kRed);
   sig_plot_2->SetFillColor(kRed);

   sprintf( data_entry, "Data (%.3lf fb^{-1})", TotalLuminosity/1000. );

   TLegend* l2 = new TLegend(0.55,0.7,0.9,0.9);
   l2->AddEntry(data_set_plot, data_entry ,"lp");
   l2->AddEntry(pdf_plot_2 ,"Template fit (Normalized to Data)","l");
   l2->AddEntry(pdf_plot_err_2, "Fit error (1 #sigma)" , "f");
   l2->AddEntry(sig_plot_2 , sig_entry , "fl");
   l2->Draw();

   tl->SetNDC(kTRUE);
   tl->SetTextFont(43);
   tl->SetTextSize( FONT_SIZE + 4 );
   tl->SetTextAlign(11);
   tl->DrawLatex( 0.1, 0.93, "CMS at #sqrt{s} = 13TeV");
   tl->SetTextAlign(31);
   tl->DrawLatex( 0.9, 0.93 , GetChannelPlotLabel().c_str() );
   tl->SetTextAlign(33);
   tl->DrawLatex(0.88,0.65, GetFitFuncFormula().c_str() );

   c2->SaveAs( GetRooObjPlot(signal->Name() + "_fitmc_vs_data").c_str() );
   c2->SetLogy();
   c2->SaveAs( GetRooObjPlot(signal->Name() + "_fitmc_vs_data_log").c_str() );

   delete c1;
   delete c2;
   delete l1;
   delete l2;
   delete tl;
   }

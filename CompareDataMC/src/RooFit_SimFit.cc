/*******************************************************************************
 *
 *  Filename    : RooFit_SimFit.cc
 *  Description : Making simultaneous fit objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleRooFitMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/VarMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/MakePDF.hh"
#include "TstarAnalysis/CompareDataMC/interface/PlotConfig.hh"
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"

#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hh"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"


#include <stdio.h>
#include <fstream>

#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooAddPdf.h"
#include "RooKeysPdf.h"
#include "RooGenericPdf.h"
#include "RooFitResult.h"

#include "TAxis.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"

using namespace std;

//------------------------------------------------------------------------------
//   Static helper variables
//------------------------------------------------------------------------------
static const string ws_name = "wspace";

//------------------------------------------------------------------------------
//   Helper static functions
//------------------------------------------------------------------------------
namespace smft {
   RooFitResult* FitPDFs( SampleRooFitMgr*, SampleRooFitMgr* );
   void MakeValidationPlot( SampleRooFitMgr*, SampleRooFitMgr*, RooFitResult* );
   void MakeCardFile( SampleRooFitMgr*, SampleRooFitMgr* );
   void SaveRooWorkSpace(SampleRooFitMgr*,vector<SampleRooFitMgr*>&, vector<RooFitResult*>& );
};

void MakeSimFit( SampleRooFitMgr* data, vector<SampleRooFitMgr*>& signal_list )
{
   using namespace smft;
   vector<RooFitResult*> results_list;
   for( auto& signal : signal_list ){
      RooFitResult* err = FitPDFs( data, signal );
      MakeValidationPlot( data, signal, err );
      MakeCardFile( data,signal);
      results_list.push_back( err );
   }
   SaveRooWorkSpace(data,signal_list,results_list);
}

//------------------------------------------------------------------------------
//   RooFit control flow
//------------------------------------------------------------------------------
RooFitResult* smft::FitPDFs( SampleRooFitMgr* data , SampleRooFitMgr* sig )
{
   const string model_name = data->MakePdfAlias( sig->Name() );
   RooGenericPdf* bg_pdf   = NULL;
   RooKeysPdf*    sig_pdf  = MakeKeysPdf( sig, "key" );
   RooRealVar*    nb       = var_mgr.NewVar("nb"+sig->Name(), 4500, 0,    10000);
   RooRealVar*    ns       = var_mgr.NewVar("ns"+sig->Name(), 10,   0,    10000);

   if( GetFitFunc() == "Exo" ){
      bg_pdf = MakeExo( data, "bg" +sig->Name() );
   } else if( GetFitFunc() == "Fermi" ){
      bg_pdf = MakeFermi( data, "bg" + sig->Name() );
   } else {
      bg_pdf = MakeFermi( data, "bg" + sig->Name() );
      fprintf( stderr, "Warning! Unrecognized Fitting function [%s], using fermi...\n" , GetFitFunc().c_str() );
      fflush( stderr );
   }

   RooAddPdf* model = new RooAddPdf(
      model_name.c_str(), model_name.c_str(),
      RooArgList(*bg_pdf,*sig_pdf),
      RooArgList(*nb,*ns)
   );

   RooDataSet* data_to_fit = data->GetDataFromAlias( GetDataSetName() );

   RooFitResult* err = model->fitTo(
      *(data_to_fit),
      RooFit::Range("FitRange"),
      RooFit::Minos(kTRUE),
      RooFit::Save(),
      RooFit::Verbose(kFALSE),
      RooFit::PrintLevel(-1)
   );
   err->SetName( ("results_"+sig->Name()).c_str() );
   data->AddPdf( model );
   var_mgr.SetConstant();
   return err;
}

//------------------------------------------------------------------------------
//   Making data card
//------------------------------------------------------------------------------
void smft::MakeCardFile( SampleRooFitMgr* data, SampleRooFitMgr* sig )
{
   RooAddPdf* model  = (RooAddPdf*)(data->GetPdfFromAlias( sig->Name() )) ;
   const Parameter bg_strength = Parameter(
      ((RooRealVar*)(model->coefList().at(0)))->getVal(),
      ((RooRealVar*)(model->coefList().at(0)))->getErrorHi(),
      ((RooRealVar*)(model->coefList().at(0)))->getErrorLo()
   );
   const Parameter sig_strength = sig->Sample()->ExpectedYield();

   RooDataSet* data_obs    = data->GetReduceDataSet( GetDataSetName() );
   RooAbsPdf*  bg_pdf      = data->GetPdfFromAlias("bg"+sig->Name());
   RooAbsPdf*  signal_pdf  = sig->GetPdfFromAlias("key");

   FILE* cardfile = fopen( GetCardFile(sig->Name()).c_str() , "w" );

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
      sig_strength.CentralValue() ,
      bg_strength.CentralValue()
   );

   // Listing Nuisance parameters
   char sig_unc[1024];
   char bg_unc[1024];
   sprintf( sig_unc , "%lf/%lf" , 1+sig_strength.RelUpperError(),1-sig_strength.RelLowerError() );
   sprintf( bg_unc  , "%lf/%lf" , 1+bg_strength.RelUpperError(),1-bg_strength.RelLowerError() );

   fprintf( cardfile , "----------------------------------------\n" );
   fprintf( cardfile, "%8s lnN %15s %15s\n" , "Lumi"   , "1.05" , "1.05" );
   fprintf( cardfile, "%8s lnN %15s %15s\n" , "sig_unc", sig_unc, "--"   );
   fprintf( cardfile, "%8s lnN %15s %15s\n" , "bg_unc" , "--"   , bg_unc );

   fclose( cardfile );
}

//------------------------------------------------------------------------------
//   Making validation plots
//------------------------------------------------------------------------------
void smft::MakeValidationPlot( SampleRooFitMgr* data, SampleRooFitMgr* sig, RooFitResult* err )
{
   TCanvas* c = new TCanvas("c","c", CANVAS_WIDTH, CANVAS_HEIGHT );
   RooPlot* frame = SampleRooFitMgr::x().frame();

   RooAddPdf*  model   = (RooAddPdf*)data->GetPdfFromAlias( sig->Name() );
   RooAbsPdf*  bg_pdf  = data->GetPdfFromAlias("bg"+sig->Name() );
   RooAbsPdf*  sig_pdf = sig->GetPdfFromAlias("key");
   RooRealVar* bg_var  = (RooRealVar*)(model->coefList().at(0));
   const double bg_strength  = bg_var->getVal();
   const double bg_err       = bg_var->getError();
   const double sig_strength = sig->Sample()->ExpectedYield();

   TGraph* data_plot = PlotOn( frame, data->GetDataFromAlias( GetDataSetName() ) );
   TGraph* bg_err_plot = PlotOn( frame, bg_pdf,
      RooFit::VisualizeError(*err, 1 ),
      RooFit::Normalization(bg_strength,RooAbsReal::NumEvent)
   );
   TGraph* bg_plot = PlotOn( frame, bg_pdf,
      RooFit::Normalization(bg_strength,RooAbsReal::NumEvent)
   );
   TGraph* sig_plot  = PlotOn( frame, sig_pdf,
      RooFit::Normalization(sig_strength,RooAbsReal::NumEvent),
      RooFit::DrawOption("LB")
   );
   PlotOn( frame, data->GetDataFromAlias( GetDataSetName() ) );

   frame->Draw();
   frame->SetMinimum(0.3);
   SetFrame(frame,FONT_SIZE); // see Utils/src/RooFitUtils.cc
   // frame->GetYaxis()->SetTitle( data_plot->GetYaxis()->GetTitle() );

   bg_err_plot->SetFillColor(kCyan);
   bg_plot->SetFillColor(kCyan);
   sig_plot->SetLineColor(kRed);
   sig_plot->SetFillStyle(3004);
   sig_plot->SetFillColor(kRed);

   const double legend_x_min = 0.6;
   const double legend_y_min = 0.7;

   TLegend* leg = plt::NewLegend( legend_x_min, legend_y_min );
   char sig_entry[1024];
   sprintf( sig_entry,  "t* {}_{M_{t*}=%dGeV} (%.1lf pb)", GetSignalMass(sig->Name()), sig->Sample()->CrossSection().CentralValue() );
   leg->AddEntry( data_plot, "Data", "lp" );
   leg->AddEntry( bg_plot  , "Fitted Background" , "l" );
   leg->AddEntry( bg_err_plot, "Fit Error(1#sigma)" , "f");
   leg->AddEntry( sig_plot , sig_entry , "lf" );
   leg->Draw();

   plt::DrawCMSLabel();
   plt::DrawLuminosity( sig->Sample()->TotalLuminosity() );
   TLatex tl;
   tl.SetNDC(kTRUE);
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.02, GetChannelPlotLabel().c_str() );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.08, GetFitFuncFormula().c_str() );

   unsigned obs = data->GetDataFromAlias(GetDataSetName())->sumEntries();
   char obs_yield[1024];
   char exp_yield[1024];
   char delta[1024];
   sprintf( obs_yield, "Observed Yield: %d", obs );
   sprintf( exp_yield, "Expected Yield: %.2lf #pm %.2lf" , bg_strength, bg_err );
   sprintf( delta    , "#Delta: %.3lf%%", 100.* (bg_strength-obs)/obs );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.02 , obs_yield );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.07, exp_yield );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.12, delta );


   // Saving and cleaning up
   c->SaveAs( GetRooObjPlot( sig->Name() ).c_str() );
   c->SetLogy(kTRUE);
   c->SaveAs( GetRooObjPlot(sig->Name() + "_log").c_str() );
   delete leg;
   delete c;
}

//------------------------------------------------------------------------------
//   Save everything to RooWorkspace
//------------------------------------------------------------------------------
void smft::SaveRooWorkSpace( SampleRooFitMgr* data, vector<SampleRooFitMgr*>& signal_list, vector<RooFitResult*>& results_list )
{
   const string roofit_file = GetRooObjFile();
   cout << "Saving RooFit objects to " << roofit_file << endl;
   RooWorkspace ws( ws_name.c_str() , ws_name.c_str() );
   ws.import( *(data->GetReduceDataSet(GetDataSetName())) );
   for( auto& signal : signal_list ){
      ws.import( *(data->GetPdfFromAlias("bg"+signal->Name())) );
      ws.import( *(signal->GetPdfFromAlias("key")) );
   }
   ws.writeToFile( roofit_file.c_str() );

   TFile* fit_file = TFile::Open( GetFitResults().c_str() , "RECREATE" );
   for( auto& result : results_list ){
      cout << "Adding results " << result->GetName() << " " << result << endl;
      result->Write( result->GetName() );
   }
   fit_file->Close();
   delete fit_file;
}

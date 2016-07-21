/*******************************************************************************
 *
 *  Filename    : RooFit_SimFit.cc
 *  Description : Making simultaneous fit objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"

#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"


#include <stdio.h>
#include <fstream>

#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooAddPdf.h"
#include "RooKeysPdf.h"
#include "RooGenericPdf.h"
#include "RooFitResult.h"

#include "TFile.h"

using namespace std;

void MakeSimFit( SampleRooFitMgr* data, vector<SampleRooFitMgr*>& signal_list )
{
   using namespace smft;
   vector<RooAbsPdf*>    bg_pdf_list;
   vector<RooAbsPdf*>    sig_pdf_list;
   vector<RooFitResult*> results_list;
   for( auto& sig : signal_list ){
      RooFitResult* err = FitPDFs( data, sig );
      MakeValidationPlot( data, sig, err );
      MakeCardFile( data,sig );

      bg_pdf_list .push_back( data->GetPdfFromAlias("bg"+sig->Name()) );
      sig_pdf_list.push_back( sig ->GetPdfFromAlias("key") );
      results_list.push_back( err );
   }

   SaveRooWorkSpace(
      data->GetDataFromAlias( dataset_alias ),
      bg_pdf_list,
      sig_pdf_list,
      results_list
   );
}

//------------------------------------------------------------------------------
//   RooFit control flow
//------------------------------------------------------------------------------
RooFitResult* smft::FitPDFs( SampleRooFitMgr* data , SampleRooFitMgr* sig, const string& tag )
{
   const string full_tag   = sig->Name() + tag;
   const string model_name = data->MakePdfAlias( full_tag );
   RooDataSet* data_to_fit = data->GetDataFromAlias( dataset_alias ) ;
   const double num_data   = data_to_fit->sumEntries();
   RooAbsPdf*   bg_pdf     = NULL;
   RooKeysPdf*  sig_pdf    = MakeKeysPdf( sig );
   RooRealVar*  nb         = var_mgr.NewVar("nb"+full_tag, num_data, -100000, 100000);
   RooRealVar*  ns         = var_mgr.NewVar("ns"+full_tag, 0       , -100000, 100000);

   const string fitfunc = limit_namer.GetInput("fitfunc");
   if( fitfunc == "Exo" ){
      bg_pdf = MakeExo( data, "bg"+full_tag );
   } else if( fitfunc == "Fermi" ){
      bg_pdf = MakeFermi( data, "bg"+full_tag );
   } else if( fitfunc == "Lognorm") {
      bg_pdf = MakeLognorm( data, "bg"+full_tag );
   } else if( fitfunc == "Landau" ){
      bg_pdf = MakeLandau( data, "bg"+full_tag );
   } else if( fitfunc == "Trial" ){
      bg_pdf = MakeTrial( data, "bg"+full_tag );
   } else {
      bg_pdf = MakeFermi( data, "bg" + full_tag );
      fprintf(
         stderr, "Warning! Unrecognized Fitting function [%s], using fermi...\n",
         fitfunc.c_str() );
      fflush( stderr );
   }

   RooAddPdf* model = new RooAddPdf(
      model_name.c_str(), model_name.c_str(),
      RooArgList(*bg_pdf,*sig_pdf),
      RooArgList(*nb,*ns)
   );


   RooFitResult* err = model->fitTo(
      *(data_to_fit),
      RooFit::Range("FitRange"),
      RooFit::Minos(kTRUE),
      RooFit::Save(),
      RooFit::Verbose(kFALSE),
      RooFit::PrintLevel(-1),
      RooFit::PrintEvalErrors(-1),
      RooFit::Warnings(kFALSE)
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
   RooAddPdf* model     = (RooAddPdf*)(data->GetPdfFromAlias( sig->Name() )) ;
   RooDataSet* data_set = data->GetReduceDataSet( dataset_alias );
   RooAbsPdf*  bg_pdf   = data->GetPdfFromAlias("bg"+sig->Name());
   RooDataSet* sig_set  = sig->OriginalDataSet();
   RooAbsPdf*  sig_pdf  = sig->GetPdfFromAlias("key");
   const Parameter bg_unc = Parameter(
      ((RooRealVar*)(model->coefList().at(0)))->getVal(),
      ((RooRealVar*)(model->coefList().at(0)))->getErrorHi(),
      ((RooRealVar*)(model->coefList().at(0)))->getErrorLo()
   );

   FILE* card_file = MakeCardCommon( data_set, bg_pdf, sig_pdf, sig->Name() );
   fprintf(
      card_file , "%12s %15lf %15lf\n" , "rate",
      sig_set->sumEntries(),
      bg_unc.CentralValue()
   );

   const Parameter lumi(1,0.05,0.05);
   const Parameter sig_unc = sig->Sample()->SelectionEfficiency();
   const Parameter null(0,0,0);
   fprintf( card_file , "----------------------------------------\n" );

   PrintNuisanceFloats( card_file, "Lumi"   , "lnN", lumi   , lumi   );
   PrintNuisanceFloats( card_file, "sig_unc", "lnN", sig_unc, null   );
   PrintNuisanceFloats( card_file, "bg_unc" , "lnN", null   , bg_unc );

   fclose( card_file );
}

//------------------------------------------------------------------------------
//   Making validation plots
//------------------------------------------------------------------------------
void smft::MakeValidationPlot( SampleRooFitMgr* data, SampleRooFitMgr* sig, RooFitResult* err, const string& tag )
{
   TCanvas* c = new TCanvas("c","c", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );
   RooPlot* frame = SampleRooFitMgr::x().frame();

   const string full_tag = sig->Name()+tag;
   RooDataSet* data_set = data->GetDataFromAlias( dataset_alias );
   RooAddPdf*  model    = (RooAddPdf*)data->GetPdfFromAlias( full_tag );
   RooAbsPdf*  bg_pdf   = data->GetPdfFromAlias("bg"+full_tag );
   RooAbsPdf*  sig_pdf  = sig->GetPdfFromAlias("key");
   RooRealVar* bg_var   = (RooRealVar*)(model->coefList().at(0));
   RooRealVar* sig_var  = (RooRealVar*)(model->coefList().at(1));
   const double sig_fit_stgth = sig_var->getVal();
   const double bg_strength  = bg_var->getVal();
   const double bg_err       = bg_var->getError();
   const double sig_strength = sig->Sample()->ExpectedYield();

   TGraph* data_plot    = PlotOn( frame, data_set );
   TGraph* model_plot   = PlotOn( frame, model    );
   TGraph* sig_fit_plot = PlotOn( frame, sig_pdf,
      RooFit::Normalization(sig_fit_stgth,RooAbsReal::NumEvent),
      RooFit::DrawOption("L")
   );
   TGraph* bg_plot = PlotOn( frame, bg_pdf,
      RooFit::Normalization(bg_strength,RooAbsReal::NumEvent)
   );
   TGraph* sig_plot  = PlotOn( frame, sig_pdf,
      RooFit::Normalization(sig_strength,RooAbsReal::NumEvent),
      RooFit::DrawOption("LB")
   );
   PlotOn( frame, data_set );

   frame->Draw();
   frame->SetMinimum(0.3);
   SetFrame(frame,AXIS_TITLE_FONT_SIZE); // see Utils/src/RooFitUtils.cc
   // frame->GetYaxis()->SetTitle( data_plot->GetYaxis()->GetTitle() );

   model_plot->SetLineColor(kGreen);
   sig_fit_plot->SetLineColor(kGreen);
   bg_plot->SetFillColor(kCyan);
   sig_plot->SetLineColor(kRed);
   sig_plot->SetFillStyle(3004);
   sig_plot->SetFillColor(kRed);

   const double legend_x_min = 0.6;
   const double legend_y_min = 0.7;

   TLegend* leg = plt::NewLegend( legend_x_min, legend_y_min );
   char sig_entry[1024];
   sprintf(
      sig_entry,  "t* {}_{M_{t*}=%dGeV} (%.1lf pb)",
      GetInt(sig->Name()),
      sig->Sample()->CrossSection().CentralValue()
   );
   if( tag != "" ) { leg->AddEntry( data_plot,  "Pseudo data", "lp" ); }
   else            { leg->AddEntry( data_plot,  "Data", "lp" ); }
   leg->AddEntry( bg_plot  ,  "Fitted Background",      "l" );
   leg->AddEntry( model_plot, "Fitted Signal/Combine" , "l" );
   leg->AddEntry( sig_plot , sig_entry , "lf" );
   leg->Draw();

   if( tag != "" ) { plt::DrawCMSLabel(SIMULATION) ; }
   else            { plt::DrawCMSLabel(); }
   plt::DrawLuminosity( sig->Sample()->TotalLuminosity() );
   TLatex tl;
   tl.SetNDC(kTRUE);
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.02, limit_namer.GetChannelEXT("Root Name").c_str() );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.08, limit_namer.GetExtName("fitfunc","Root Name").c_str() );

   unsigned obs = data_set->sumEntries();
   char obs_yield[1024];
   char exp_yield[1024];
   char delta[1024];
   sprintf( obs_yield, "Observed Yield: %d", obs );
   sprintf( exp_yield, "Fitted bkg: %.2lf #pm %.2lf" , bg_strength, bg_err );
   sprintf( delta    , "#Delta: %.3lf%%", 100.* (bg_strength-obs)/obs );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.02, limit_namer.query_tree("fitmethod","SimFit","Full Name").c_str() );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.07, obs_yield );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.12, exp_yield );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.17, delta );


   // Saving and cleaning up
   string prefix = "fitplot";
   if( tag != "" ){ prefix="valplot"+tag; }
   c->SaveAs( limit_namer.PlotFileName( prefix, sig->Name() ).c_str() );
   c->SetLogy(kTRUE);
   c->SaveAs( limit_namer.PlotFileName( prefix,sig->Name()+"_log").c_str() );

   c->SetLogy(kFALSE);
   const double peak_value = model_plot->Eval( GetInt(sig->Name()) );
   frame->SetMaximum(peak_value*1.5);
   c->SaveAs( limit_namer.PlotFileName( prefix, sig->Name()+"_zoom" ).c_str() );
   c->SetLogy(kTRUE);
   frame->SetMaximum(peak_value*10.);
   frame->SetMinimum(peak_value*0.1);
   c->SaveAs( limit_namer.PlotFileName( prefix,sig->Name()+"_zoom_log").c_str() );

   delete leg;
   delete c;
}

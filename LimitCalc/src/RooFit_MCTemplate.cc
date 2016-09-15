/*******************************************************************************
*
*  Filename    : RooFit_MCTemplate.cc
*  Description : Plotting to file for template methods.cc
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/MakeKeysPdf.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/Template.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"

#include <fstream>
#include <string>

using namespace std;

// ------------------------------------------------------------------------------
//   Object naming conventions
// ------------------------------------------------------------------------------
string
TemplatePdfName( const std::string& datasetname )
{
   return datasetname + "template";
}

extern const string StitchTemplatePdfName = "templatemaster";

// ------------------------------------------------------------------------------
//   Main control flow, to be called by main function
// ------------------------------------------------------------------------------
void
MakeTemplate(
   SampleRooFitMgr*          data,
   SampleRooFitMgr*          bg,
   vector<SampleRooFitMgr*>& signal_list
   )
{
   vector<RooAbsPdf*> sig_pdf_list;

   for( auto& sig : signal_list ){
      sig_pdf_list.push_back( MakeFullKeysPdf( sig ) );
   }

   MakeFullTemplate( bg );
   MakeTemplatePlot( data, bg, signal_list.front(), true );
   MakeTemplatePlot( data, bg, signal_list.front(), false );

   SaveRooWorkSpace(
      data->DataSet( "" ),
      {bg->Pdf( StitchTemplatePdfName )},
      sig_pdf_list
      );

   for( auto& signal : signal_list ){
      MakeTemplateCardFile( data, bg, signal );
   }
}


// ------------------------------------------------------------------------------
//  Fitting function implementations
// ------------------------------------------------------------------------------
RooFitResult*
FitBackgroundTemplate( SampleRooFitMgr* bg, const string& datatag )
{
   const string bgpdfname = TemplatePdfName( datatag );

   RooAbsPdf* bg_pdf = bg->NewPdf( bgpdfname, limit_namer.GetInput( "fitfunc" ) );

   RooFitResult* ans = bg_pdf->fitTo(
      *( bg->DataSet( datatag ) ),
      RooFit::Save(),
      RooFit::SumW2Error( kTRUE ),// For Weighted dataset
      RooFit::Range( "FitRange" ),// Fitting range
      RooFit::Minos( kTRUE ),
      RooFit::Verbose( kFALSE ),
      RooFit::PrintLevel( -1 ),
      RooFit::PrintEvalErrors( -1 ),
      RooFit::Warnings( kFALSE )
      );

   bg->SetConstant( kTRUE );// Freezing all constants

   return ans;
}

RooAddPdf*
MakeFullTemplate( SampleRooFitMgr* bg )
{
   vector<string> bgpdflist;

   for( const auto& datasetname : bg->SetNameList() ){
      FitBackgroundTemplate( bg, datasetname );
      bgpdflist.push_back( TemplatePdfName( datasetname ) );
   }

   return MakeStichPdf( bg, StitchTemplatePdfName, bgpdflist );
}

void
MakeTemplateCardFile( SampleRooFitMgr* data, SampleRooFitMgr* bg, SampleRooFitMgr* sig )
{
   RooDataSet* data_obs = data->DataSet( "" );
   RooAbsPdf* bg_pdf    = bg->Pdf( StitchTemplatePdfName );
   RooAbsPdf* sig_pdf   = sig->Pdf( StitchKeyPdfName );

   FILE* card_file = MakeCardCommon( data_obs, bg_pdf, sig_pdf, sig->Name() );

   fprintf(
      card_file, "%12s %15lf %15lf\n",
      "rate",
      sig->DataSet()->sumEntries(),
      data_obs->sumEntries()
      );

   fprintf( card_file, "----------------------------------------\n" );
   const Parameter lumi( 1, 0.05, 0.05 );
   const Parameter null( 0, 0, 0 );
   const Parameter sig_unc = sig->Sample()->SelectionEfficiency();
   const Parameter bkg_unc = lumi;
   PrintNuisanceFloats( card_file, "Lumi",    "lnN", lumi,    lumi    );
   PrintNuisanceFloats( card_file, "sig_unc", "lnN", sig_unc, null    );
   PrintNuisanceFloats( card_file, "bg_unc",  "lnN", null,    bkg_unc );

   fclose( card_file );
}


// ------------------------------------------------------------------------------
//   Plotting fitting results
// ------------------------------------------------------------------------------
void
MakeTemplatePlot(
   SampleRooFitMgr* data,
   SampleRooFitMgr* mc,
   SampleRooFitMgr* signal,
   const bool       use_data )
{
   // First plot against MC
   const double TotalLuminosity = mgr::SampleMgr::TotalLuminosity();

   TCanvas* c     = new TCanvas( "c", "c", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );
   RooPlot* frame = SampleRooFitMgr::x().frame();
   TGraph* pdf_plot;
   TGraph* sig_plot;
   TGraph* set_plot;

   if( use_data ){
      set_plot = PlotOn( frame, data->DataSet() );
      pdf_plot = PlotOn(
         frame,
         mc->Pdf( StitchTemplatePdfName ),
         RooFit::Range( "FitRange" ),
         RooFit::Normalization( data->DataSet()->sumEntries(), RooAbsReal::NumEvent )
         );
      set_plot = PlotOn( frame, data->DataSet() );
   } else {
      set_plot = PlotOn( frame, mc->DataSet() );
      pdf_plot = PlotOn(
         frame, mc->Pdf( StitchTemplatePdfName ),
         RooFit::Range( "FitRange" ),
         RooFit::Normalization( mc->DataSet()->sumEntries(), RooAbsReal::NumEvent )
         );
      set_plot = PlotOn( frame, data->DataSet() );
   }
   sig_plot = PlotOn(
      frame, signal->Pdf( StitchKeyPdfName ),
      RooFit::DrawOption( "LB" ),
      RooFit::Normalization( signal->Sample()->ExpectedYield().CentralValue(), RooAbsReal::NumEvent )
      );

   frame->Draw();
   frame->SetMinimum( 0.3 );
   SetFrame( frame );
   frame->GetYaxis()->SetTitle( set_plot->GetYaxis()->GetTitle() );
   sig_plot->SetFillStyle( 3004 );
   sig_plot->SetLineColor( kRed );
   sig_plot->SetFillColor( kRed );

   char norm[1024];
   char sig_entry[1024];
   if( use_data ){
      sprintf( norm, "Template fit (from MC)" );
   } else {
      sprintf( norm, "Template fit (Normalized to Data)" );
   }

   sprintf( sig_entry, "Signal (%.2lf pb)", signal->Sample()->CrossSection().CentralValue() );

   const double legend_x_min = 0.55;
   const double legend_y_min = 0.70;

   TLegend* l = plt::NewLegend( legend_x_min, legend_y_min );
   if( use_data ){
      l->AddEntry( set_plot, "Data",                              "lp" );
      l->AddEntry( pdf_plot, "Template fit (Normalized to Data)", "l" );
   } else {
      l->AddEntry( set_plot, "MC Background",                     "lp" );
      l->AddEntry( pdf_plot, "Template fit (Normalized to Lumi)", "l" );
   }
   l->AddEntry( sig_plot, sig_entry, "fl" );
   l->Draw();

   plt::DrawCMSLabel();
   plt::DrawLuminosity( TotalLuminosity );

   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.05, limit_namer.GetChannelEXT( "Root Name" ).c_str() );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.12, limit_namer.GetExtName( "fitfunc", "Full Name" ).c_str() );

   if( use_data ){
      c->SaveAs( limit_namer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-data"} ).c_str() );
      c->SetLogy();
      c->SaveAs( limit_namer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-data_log"} ).c_str() );
   } else {
      c->SaveAs( limit_namer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-mc"} ).c_str() );
      c->SetLogy();
      c->SaveAs( limit_namer.PlotFileName( "fitplot", {signal->Name(), ",fitmc-vs-mc_log"} ).c_str() );
   }
   delete c;
   delete l;
}

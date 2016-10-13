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
#include "TstarAnalysis/LimitCalc/interface/SimFit.hpp"
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
   vector<RooAbsPdf*> sigpdf_list;

   for( auto& sig : signal_list ){
      sigpdf_list.push_back( MakeFullKeysPdf( sig ) );
   }

   MakeFullTemplate( bg );
   MakeTemplatePlot( data, bg, signal_list.front(), true );
   MakeTemplatePlot( data, bg, signal_list.front(), false );

   SaveRooWorkSpace(
      data->DataSet( "" ),
      {bg->Pdf( StitchTemplatePdfName )},
      sigpdf_list
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

   RooAbsPdf* bgpdf = bg->NewPdf( bgpdfname, limit_namer.GetInput( "fitfunc" ) );

   RooFitResult* ans = bgpdf->fitTo(
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
   RooDataSet* dataobs = data->DataSet( "" );
   RooAbsPdf* bgpdf    = bg->Pdf( StitchTemplatePdfName );
   RooAbsPdf* sigpdf   = sig->Pdf( StitchKeyPdfName );

   FILE* cardfile = MakeCardCommon( dataobs, bgpdf, sigpdf, sig->Name() );

   fprintf(
      cardfile, "%12s %15lf %15lf\n",
      "rate",
      sig->DataSet()->sumEntries(),
      bg->DataSet()->sumEntries()
      );

   fprintf( cardfile, "----------------------------------------\n" );
   const Parameter null( 0, 0, 0 );
   const Parameter lumi( 1, 0.062, 0.062 );
   const Parameter lepunc( 1, 0.03, 0.03 );
   const Parameter sigstatunc = sig->Sample()->SelectionEfficiency();
   const Parameter bkgstatunc = bg->ExpectedYield();// includes uncertaintly from cross section and selection effiency

   const Parameter sigjecunc   = GetMCNormError( sig, "jecup",    "jecdown"    );
   const Parameter sigjerunc   = GetMCNormError( sig, "jetresup", "jetresdown" );
   const Parameter siglepunc   = GetMCNormError( sig, "lepup",    "lepdown"    );
   const Parameter sigbtagunc  = GetMCNormError( sig, "btagup",   "btagdown"   );
   const Parameter sigpuunc    = GetMCNormError( sig, "puup",     "pudown"     );
   const Parameter sigpdfunc   = GetMCNormError( sig, "pdfup",    "pdfdown"    );
   const Parameter sigscaleunc = GetMCNormError( sig, "scaleup",  "scaledown"  );

   const Parameter bkgjecunc   = GetMCNormError( bg, "jecup",    "jecdown"    );
   const Parameter bkgjerunc   = GetMCNormError( bg, "jetresup", "jetresdown" );
   const Parameter bkglepunc   = GetMCNormError( bg, "lepup",    "lepdown"    );
   const Parameter bkgbtagunc  = GetMCNormError( bg, "btagup",   "btagdown"   );
   const Parameter bkgpuunc    = GetMCNormError( bg, "puup",     "pudown"     );
   const Parameter bkgpdfunc   = GetMCNormError( bg, "pdfup",    "pdfdown"    );
   const Parameter bkgscaleunc = GetMCNormError( bg, "scaleup",  "scaledown"  );

   PrintNuisanceFloats( cardfile, "Lumi",    "lnN", lumi,        lumi        );
   PrintNuisanceFloats( cardfile, "lepsys",  "lnN", lepunc,      lepunc      );
   PrintNuisanceFloats( cardfile, "sigstat", "lnN", sigstatunc,  null        );
   PrintNuisanceFloats( cardfile, "bkgstat", "lnN", null,        bkgstatunc  );
   PrintNuisanceFloats( cardfile, "jec",     "lnN", sigjecunc,   bkgjecunc   );
   PrintNuisanceFloats( cardfile, "jer",     "lnN", sigjerunc,   bkgjerunc   );
   PrintNuisanceFloats( cardfile, "lep",     "lnN", siglepunc,   bkglepunc   );
   PrintNuisanceFloats( cardfile, "btag",    "lnN", sigbtagunc,  bkgbtagunc  );
   PrintNuisanceFloats( cardfile, "pileup",  "lnN", sigpuunc,    bkgpuunc    );
   PrintNuisanceFloats( cardfile, "pdf",     "lnN", sigpdfunc,   bkgpdfunc   );
   PrintNuisanceFloats( cardfile, "scale",   "lnN", sigscaleunc, bkgscaleunc );

   // Getting fitting parameters
   for( const auto& var : bg->VarContains( "template" ) ){
      const string varname = var->GetName();
      if( varname.find( "coeff" ) == string::npos ){
         PrintFloatParam( cardfile, var );
      }
   }

   // Getting stitching co-efficiencts
   for( const auto& var : bg->VarContains( "coeff" ) ){
      PrintFlatParam( cardfile, var );
   }

   for( const auto& var : sig->VarContains( "coeff" ) ){
      PrintFlatParam( cardfile, var );
   }

   fclose( cardfile );
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

   TCanvas* c     = plt::NewCanvas();
   RooPlot* frame = SampleRooFitMgr::x().frame();
   TGraph* pdf_plot;
   TGraph* sig_plot;
   TGraph* set_plot;

   if( use_data ){
      pdf_plot = PlotOn(
         frame,
         mc->Pdf( StitchTemplatePdfName ),
         RooFit::Range( "FitRange" ),
         RooFit::Normalization( data->DataSet()->sumEntries(), RooAbsReal::NumEvent )
         );
      set_plot = PlotOn( frame, data->DataSet() );
   } else {
      pdf_plot = PlotOn(
         frame, mc->Pdf( StitchTemplatePdfName ),
         RooFit::Range( "FitRange" ),
         RooFit::Normalization( mc->DataSet()->sumEntries(), RooAbsReal::NumEvent )
         );
      set_plot = PlotOn( frame, mc->DataSet() );
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

   char sig_entry[1024];
   sprintf( sig_entry, "%s (%.2lf pb)", signal->RootName().c_str(), signal->Sample()->CrossSection().CentralValue() );

   const double legend_x_min = 0.45;
   const double legend_y_min = 0.70;
   TLegend* l                = plt::NewLegend( legend_x_min, legend_y_min );
   if( use_data ){
      l->AddEntry( set_plot, "Data",                                     "lp" );
      l->AddEntry( pdf_plot, "Background fit to MC(Normalized to Data)", "l" );
   } else {
      l->AddEntry( set_plot, "MC Background",        "lp" );
      l->AddEntry( pdf_plot, "Background fit to MC", "l"  );
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
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.12, limit_namer.GetExtName( "fitfunc", "Root Name" ).c_str() );


   const string rootfile = limit_namer.PlotRootFile();
   if( use_data ){
      plt::SaveToPDF( c, limit_namer.MakeFileName( "", "fitplot", {signal->Name(), "fitmc-vs-data"} ) );
      plt::SaveToROOT( c, rootfile, limit_namer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-data"} ) );
      c->SetLogy();
      plt::SaveToPDF( c, limit_namer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-data_log"} ) );
   } else {
      plt::SaveToPDF( c, limit_namer.MakeFileName( "", "fitplot", {signal->Name(), "fitmc-vs-mc"} ) );
      plt::SaveToROOT( c, rootfile, limit_namer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-mc"} ) );
      c->SetLogy();
      plt::SaveToPDF( c, limit_namer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-mc_log"} ) );
   }
   delete frame;
   delete c;
   delete l;
}

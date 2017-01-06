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
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"

#include <fstream>
#include <string>

using namespace std;

/*******************************************************************************
*   Object naming convention
*******************************************************************************/
string
TemplatePdfName( const std::string& datasetname )
{
   return datasetname + "template";
}

extern const string StitchTemplatePdfName = "templatemaster";

/*******************************************************************************
*   Main control flow to be caleed by main function
*******************************************************************************/
void
MakeTemplate(
   SampleRooFitMgr*          data,
   SampleRooFitMgr*          bg,
   vector<SampleRooFitMgr*>& signal_list
   )
{
   vector<RooAbsPdf*> pdflist;
   vector<RooAbsReal*> funclist;

   for( auto& sig : signal_list ){
      pdflist.push_back( MakeFullKeysPdf( sig ) );
      // funclist.push_back( sig->Func(StitchKeyNormName));
   }

   MakeFullTemplate( bg );
   MakeTemplatePlot( data, bg, signal_list.front(), true );
   MakeTemplatePlot( data, bg, signal_list.front(), false );
   pdflist.push_back( bg->Pdf( StitchTemplatePdfName) );

   SaveRooWorkSpace(
      data->DataSet( "" ),
      pdflist,
      funclist
      );

   for( auto& signal : signal_list ){
      MakeTemplateCardFile( data, bg, signal );
   }
}


/*******************************************************************************
*   Fitting function implementations
*******************************************************************************/
RooFitResult*
FitBackgroundTemplate( SampleRooFitMgr* bg, const string& datatag )
{
   const string bgpdfname = TemplatePdfName( datatag );

   RooAbsPdf* bgpdf = bg->NewPdf( bgpdfname, limnamer.GetInput( "fitfunc" ) );

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

/******************************************************************************/

RooAbsPdf*
MakeFullTemplate( SampleRooFitMgr* bg )
{
   vector<string> bgpdflist;

   for( const auto& datasetname : bg->SetNameList() ){
      FitBackgroundTemplate( bg, datasetname );
      bgpdflist.push_back( TemplatePdfName( datasetname ) );
   }

   return MakeSimpleStitchPdf( bg, StitchTemplatePdfName, bgpdflist );
}

/******************************************************************************/

void
MakeTemplateCardFile( SampleRooFitMgr* data, SampleRooFitMgr* bg, SampleRooFitMgr* sig )
{
   RooAbsData* dataobs = data->DataSet( "" );
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
   const Parameter sigstatunc = sig->Sample().SelectionEfficiency();
   const Parameter bkgstatunc(1,0.03,0.03);// includes uncertaintly from cross section and selection effiency

   const Parameter sigjecunc   = GetMCNormError( sig, "jecUp",    "jecDown"    );
   const Parameter sigjerunc   = GetMCNormError( sig, "jetresUp", "jetresDown" );
   const Parameter siglepunc   = GetMCNormError( sig, "lepUp",    "lepDown"    );
   const Parameter sigbtagunc  = GetMCNormError( sig, "btagUp",   "btagDown"   );
   const Parameter sigpuunc    = GetMCNormError( sig, "puUp",     "puDown"     );
   const Parameter sigpdfunc   = GetMCNormError( sig, "pdfUp",    "pdfDown"    );
   const Parameter sigscaleunc = GetMCNormError( sig, "scaleUp",  "scaleDown"  );

   const Parameter bkgjecunc   = GetMCNormError( bg, "jecUp",    "jecDown"    );
   const Parameter bkgjerunc   = GetMCNormError( bg, "jetresUp", "jetresDown" );
   const Parameter bkglepunc   = GetMCNormError( bg, "lepUp",    "lepDown"    );
   const Parameter bkgbtagunc  = GetMCNormError( bg, "btagUp",   "btagDown"   );
   const Parameter bkgpuunc    = GetMCNormError( bg, "puUp",     "puDown"     );
   const Parameter bkgpdfunc   = GetMCNormError( bg, "pdfUp",    "pdfDown"    );
   const Parameter bkgscaleunc = GetMCNormError( bg, "scaleUp",  "scaleDown"  );

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


/*******************************************************************************
*   Plotting fit results
*******************************************************************************/
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
   TGraph* pdfplot;
   TGraph* sigplot;
   TGraph* setplot;

   if( use_data ){
      setplot = plt::PlotOn(
         frame,
         data->DataSet(),
         RooFit::DrawOption( PGS_DATA )
      );
      pdfplot = plt::PlotOn(
         frame,
         mc->Pdf( StitchTemplatePdfName ),
         RooFit::Range( "FitRange" ),
         RooFit::Normalization( data->DataSet()->sumEntries(), RooAbsReal::NumEvent )
         );
   } else {
      setplot = plt::PlotOn(
         frame,
         mc->DataSet(),
         RooFit::DrawOption( PGS_DATA )
      );
      pdfplot = plt::PlotOn(
         frame, mc->Pdf( StitchTemplatePdfName ),
         RooFit::Range( "FitRange" ),
         RooFit::Normalization( mc->DataSet()->sumEntries(), RooAbsReal::NumEvent )
         );
   }

   sigplot = plt::PlotOn(
      frame, signal->Pdf( StitchKeyPdfName ),
      RooFit::DrawOption( PGS_SIGNAL ),
      RooFit::Normalization( signal->ExpectedYield(), RooAbsReal::NumEvent )
      );

   frame->Draw();
   frame->SetMinimum( 0.3 );
   plt::SetFrame( frame );
   frame->GetYaxis()->SetTitle( setplot->GetYaxis()->GetTitle() );

   tstar::SetSignalStyle( sigplot );

   char sig_entry[1024];
   sprintf( sig_entry, "%s (%.2lf pb)", signal->RootName().c_str(), signal->Sample().CrossSection().CentralValue() );

   const double legend_x_min = 0.45;
   const double legend_y_min = 0.70;
   TLegend* l                = plt::NewLegend( legend_x_min, legend_y_min );
   if( use_data ){
      l->AddEntry( setplot, "Data",                                     "lp" );
      l->AddEntry( pdfplot, "Background fit to MC(Normalized to Data)", "l" );
   } else {
      l->AddEntry( setplot, "MC Background",        "lp" );
      l->AddEntry( pdfplot, "Background fit to MC", "l"  );
   }
   l->AddEntry( sigplot, sig_entry, "fl" );
   l->Draw();

   plt::DrawCMSLabel();
   plt::DrawLuminosity( TotalLuminosity );

   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.05, limnamer.GetChannelEXT( "Root Name" ).c_str() );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.12, limnamer.GetExtName( "fitfunc", "Root Name" ).c_str() );

   const vector<const TGraph*> graphlist = {pdfplot,setplot,sigplot};
   const double ymax = plt::GetYmax( graphlist );
   frame->SetMaximum( ymax * 1.5 );

   const string rootfile = limnamer.PlotRootFile();
   if( use_data ){
      plt::SaveToPDF( c, limnamer.MakeFileName( "", "fitplot", {signal->Name(), "fitmc-vs-data"} ) );
      plt::SaveToROOT( c, rootfile, limnamer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-data"} ) );
      frame->SetMaximum( ymax * 300 );
      c->SetLogy();
      plt::SaveToPDF( c, limnamer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-data_log"} ) );
   } else {
      plt::SaveToPDF( c, limnamer.MakeFileName( "", "fitplot", {signal->Name(), "fitmc-vs-mc"} ) );
      plt::SaveToROOT( c, rootfile, limnamer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-mc"} ) );
      frame->SetMaximum( ymax * 300 );
      c->SetLogy();
      plt::SaveToPDF( c, limnamer.PlotFileName( "fitplot", {signal->Name(), "fitmc-vs-mc_log"} ) );
   }
   delete frame;
   delete c;
   delete l;
}

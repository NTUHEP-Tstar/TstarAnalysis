/*******************************************************************************
*
*  Filename    : RooFit_SimFit.cc
*  Description : Making simultaneous fit objects
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
#include <stdio.h>

#include "RooAddPdf.h"
#include "RooConstVar.h"
#include "RooFitResult.h"
#include "RooGaussian.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

#include "TFile.h"

using namespace std;

/*******************************************************************************
*   Object naming convetions functions
*******************************************************************************/
string
SimFitBGPdfName( const string& datasetname, const string& signalname )
{
   return datasetname + signalname + "simfitbg";
}

string
SimFitCombinePdfName( const string& datasetname, const string& signalname )
{
   return datasetname + signalname  + "simfitcomb";
}

string
StitchSimFitBgPdfname( const std::string& signalname )
{
   return signalname + "simfitbfmaster";
}

/*******************************************************************************
*   Main control function
*******************************************************************************/
void
MakeSimFit(
   SampleRooFitMgr*          data,
   SampleRooFitMgr*          mc,
   vector<SampleRooFitMgr*>& signal_list
   )
{
   vector<RooAbsPdf*> bg_pdf_list;
   vector<RooAbsPdf*> sig_pdf_list;

   // Getting fit result from default MC dataset ;
   RooFitResult* bgconstrain = FitBackgroundTemplate( mc, "" );

   for( auto& sig : signal_list ){
      MakeFullKeysPdf( sig );
      MakeFullSimFit( data, sig, bgconstrain );
      MakeSimFitCardFile( data, sig );

      for( const auto& name : data->SetNameList() ){
         MakeSimFitPlot( data, sig, name );
      }

      bg_pdf_list.push_back( data->Pdf( StitchSimFitBgPdfname( sig->Name() ) ) );
      sig_pdf_list.push_back( sig->Pdf( StitchKeyPdfName ) );
   }

   SaveRooWorkSpace(
      data->DataSet( "" ),
      bg_pdf_list,
      sig_pdf_list
      );
}

// ------------------------------------------------------------------------------
//   Fitting control flow
// ------------------------------------------------------------------------------
RooFitResult*
SimFitSingle(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   const string&    datasetname,
   RooFitResult*    bgconstrain
   )
{
   const string bgfuncname   = SimFitBGPdfName( datasetname, sig->Name() );
   const string combfuncname = SimFitCombinePdfName( datasetname, sig->Name() );
   const string fitfunc      = limit_namer.GetInput( "fitfunc" );

   RooDataSet* fitdataset = data->DataSet( datasetname );
   RooAbsPdf* bgpdf       = data->NewPdf( bgfuncname, fitfunc );
   RooAbsPdf* sigpdf      = MakeFullKeysPdf( sig );          // attempting simfit will default signal regardless

   vector<RooRealVar*> bgfitvarlist = data->VarContains( bgfuncname );

   const double numdata = fitdataset->sumEntries();

   RooRealVar* nb =
      data->NewVar( combfuncname+"nb", numdata, -1000000, 1000000 );
   RooRealVar* ns =
      data->NewVar( combfuncname+"ns", 0, -1000000, 1000000 );

   RooArgSet fitconstraints;

   for( unsigned i = 0; i < bgfitvarlist.size(); ++i ){
      RooRealVar* bgvar    = bgfitvarlist[i];
      RooRealVar* bgconvar =
         (RooRealVar*)( bgconstrain->floatParsFinal().at( i ) );

      string pdfname = bgvar->GetName();
      pdfname += "constrain";

      RooGaussian bgvarconpdf(
         pdfname.c_str(), pdfname.c_str(),
         *bgvar,
         RooFit::RooConst( bgconvar->getVal() ),
         RooFit::RooConst( 5*bgconvar->getError() )// five time sigma constrain for the time being
         );
      fitconstraints.addClone( bgvarconpdf );
   }

   RooAddPdf* model = new RooAddPdf(
      combfuncname.c_str(),
      combfuncname.c_str(),
      RooArgList( *bgpdf, *sigpdf ),
      RooArgList( *nb,    *ns )
      );

   RooFitResult* err = model->fitTo(
      *( fitdataset ),
      RooFit::Range( "FitRange" ),
      RooFit::Minos( kTRUE ),
      RooFit::ExternalConstraints( fitconstraints ),
      RooFit::Save(),
      RooFit::Verbose( kFALSE ),
      RooFit::PrintLevel( -1 ),
      RooFit::PrintEvalErrors( -1 ),
      RooFit::Warnings( kFALSE )
      );

   data->AddPdf( model );
   return err;
}

RooAddPdf*
MakeFullSimFit(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   RooFitResult*    bgconstrain
   )
{
   vector<string> bgpdfnamelist;

   for( const auto& datasetname : data->SetNameList() ){
      SimFitSingle( data, sig, datasetname, bgconstrain );
      bgpdfnamelist.push_back( SimFitBGPdfName( datasetname, sig->Name() ) );
   }

   return MakeStichPdf(
      data,
      StitchSimFitBgPdfname( sig->Name() ),
      bgpdfnamelist
      );
}


/*******************************************************************************
*   Making Data cards
*******************************************************************************/
void
MakeSimFitCardFile(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig
   )
{
   const string centralpdfname  = SimFitCombinePdfName( "", sig->Name() );
   const string stitchbgpdfname = StitchSimFitBgPdfname( sig->Name() );
   RooDataSet* dataset          = data->DataSet();
   RooAbsPdf* bgpdf             = data->Pdf( stitchbgpdfname );
   RooAbsPdf* sigpdf            = sig->Pdf( StitchKeyPdfName );
   const Parameter bgunc        = GetBgNormError( data, "", "", sig->Name() );

   // Writting the common parts
   FILE* card_file = MakeCardCommon( dataset, bgpdf, sigpdf, sig->Name() );
   fprintf(
      card_file, "%12s %15lf %15lf\n", "rate",
      sig->DataSet()->sumEntries(),
      bgunc.CentralValue()
      );

   // Printing list of normalization errors
   const Parameter lumi( 1, 0.05, 0.05 );
   const Parameter sigunc       = sig->Sample()->SelectionEfficiency();
   const Parameter bgjecunc     = GetBgNormError( data, "jecup", "jecdown", sig->Name() );
   const Parameter sigjecunc    = GetSigNormError( sig, "jecup", "jecdown" );
   const Parameter sigjetresunc = GetSigNormError( sig, "jetresup", "jetresdown" );
   const Parameter sigbjetunc   = GetSigNormError( sig, "btagup", "btagdown" );
   const Parameter null( 0, 0, 0 );
   fprintf( card_file, "----------------------------------------\n" );
   PrintNuisanceFloats( card_file, "Lumi",   "lnN", lumi,         lumi   );
   PrintNuisanceFloats( card_file, "sigunc", "lnN", sigunc,       null   );
   PrintNuisanceFloats( card_file, "bgunc",  "lnN", null,         bgunc  );
   PrintNuisanceFloats( card_file, "jec",    "lnN", sigjecunc,    bgjecunc );
   PrintNuisanceFloats( card_file, "jer",    "lnN", sigjetresunc, null );
   PrintNuisanceFloats( card_file, "btag",   "lnN", sigbjetunc,   null );

   // Getting a list of fitting parameters to permutate.
   vector<RooRealVar*> bgvarlist;

   for( const auto& datasetname : data->SetNameList() ){
      const string bgfuncname = SimFitBGPdfName( datasetname, sig->Name() );

      for( auto var : data->VarContains( bgfuncname ) ){
         bgvarlist.push_back( var );
      }
   }

   for( const auto& var : bgvarlist ){
      PrintFloatParam( card_file, var );
   }

   // Getting stitching variables
   vector<RooRealVar*> stitchvarlist;

   for( auto var : data->VarContains( StitchSimFitBgPdfname( sig->Name() ) + "coeff" ) ){
      stitchvarlist.push_back( var );
   }

   for( auto var : sig->VarContains( "coeff" ) ){
      stitchvarlist.push_back( var );
   }

   for( const auto& var : stitchvarlist ){
      var->setConstant( kFALSE );
      PrintFlatParam( card_file, var );
   }

   // Closing file
   fclose( card_file );
}

/*******************************************************************************
*   Error getting.
*******************************************************************************/
Parameter
GetBgNormError(
   SampleRooFitMgr* data,
   const string&    uperror,
   const string&    downerror,
   const string&    signalname
   )
{
   const string centralpdfname   = SimFitCombinePdfName( "", signalname );
   const string uperrorpdfname   = SimFitCombinePdfName( uperror, signalname );
   const string downerrorpdfname = SimFitCombinePdfName( downerror, signalname );

   RooAddPdf* centralpdf   = (RooAddPdf*)( data->Pdf( centralpdfname ) );
   RooAddPdf* uperrorpdf   = (RooAddPdf*)( data->Pdf( uperrorpdfname ) );
   RooAddPdf* downerrorpdf = (RooAddPdf*)( data->Pdf( downerrorpdfname ) );

   if( uperror == "" || downerror == "" ){// fitting error for main pdf
      return Parameter(
         ( (RooRealVar*)( centralpdf->coefList().at( 0 ) ) )->getVal(),
         ( (RooRealVar*)( centralpdf->coefList().at( 0 ) ) )->getErrorHi(),
         ( (RooRealVar*)( centralpdf->coefList().at( 0 ) ) )->getErrorLo()
         );
   } else {
      const double centralnorm   = ( (RooRealVar*)( centralpdf->coefList().at( 0 ) ) )->getVal();
      const double uperrornorm   = ( (RooRealVar*)( uperrorpdf->coefList().at( 0 ) ) )->getVal();
      const double downerrornorm = ( (RooRealVar*)( downerrorpdf->coefList().at( 0 ) ) )->getVal();
      return Parameter(
         centralnorm,
         uperrornorm - centralnorm,
         centralnorm - downerrornorm
         );
   }
}

/******************************************************************************/

Parameter
GetSigNormError(
   SampleRooFitMgr* sig,
   const string&    uperrorset,
   const string&    downerrorset
   )
{
   const double centralsetyield = sig->DataSet( "" )->sumEntries();
   const double uperroryield    = sig->DataSet( uperrorset )->sumEntries();
   const double downerroryield  = sig->DataSet( downerrorset )->sumEntries();

   return Parameter(
      centralsetyield,
      uperroryield - centralsetyield,
      centralsetyield - downerroryield
      );
}

/*******************************************************************************
*   Plotting fnuctions
*******************************************************************************/
void
MakeSimFitPlot(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   const string&    datasetname
   )
{
   TCanvas* c     = new TCanvas( "c", "c", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );
   RooPlot* frame = SampleRooFitMgr::x().frame();

   const string combpdfname = SimFitCombinePdfName( datasetname, sig->Name() );
   const string bgpdfname   = SimFitBGPdfName( datasetname, sig->Name() );

   RooDataSet* data_set = data->DataSet( datasetname );
   RooAddPdf* model     = (RooAddPdf*)data->Pdf( combpdfname );
   RooAbsPdf* bg_pdf    = data->Pdf( bgpdfname );
   RooAbsPdf* sig_pdf   = sig->Pdf( StitchKeyPdfName );

   const RooRealVar* bg_var  = data->Var( combpdfname + "nb" );
   const double bg_strength  = bg_var->getVal();
   const double bg_err       = bg_var->getError();
   const double sig_strength = sig->Sample()->ExpectedYield();

   TGraph* data_plot  = PlotOn( frame, data_set );
   TGraph* model_plot = PlotOn( frame, model    );

   TGraph* bg_plot = PlotOn(
      frame, bg_pdf,
      RooFit::Normalization( bg_strength, RooAbsReal::NumEvent )
      );

   TGraph* sig_plot = PlotOn(
      frame, sig_pdf,
      RooFit::Normalization( sig_strength, RooAbsReal::NumEvent ),
      RooFit::DrawOption( "LB" )
      );
   PlotOn( frame, data_set );

   frame->Draw();
   frame->SetMinimum( 0.3 );
   SetFrame( frame );
   // frame->GetYaxis()->SetTitle( data_plot->GetYaxis()->GetTitle() );

   model_plot->SetLineColor( kGreen );
   bg_plot->SetFillColor( kCyan );
   sig_plot->SetLineColor( kRed );
   sig_plot->SetFillStyle( 3004 );
   sig_plot->SetFillColor( kRed );

   const double legend_x_min = 0.57;
   const double legend_y_min = 0.7;

   TLegend* leg = plt::NewLegend( legend_x_min, legend_y_min );
   char sig_entry[1024];
   sprintf(
      sig_entry,  "t*%dGeV (%.2lf pb)",
      GetInt( sig->Name() ),
      sig->Sample()->CrossSection().CentralValue()
      );

   if( data->Name().find( "Data" ) != string::npos ){
      leg->AddEntry( data_plot, "Pseudo data", "lp" );
   } else {
      leg->AddEntry( data_plot, "Data", "lp" );
   }
   leg->AddEntry( bg_plot,    "Fitted Background", "l"  );
   leg->AddEntry( model_plot, "Fitted Combine",    "l"  );
   leg->AddEntry( sig_plot,   sig_entry,           "lf" );
   leg->Draw();

   plt::DrawCMSLabel();
   plt::DrawLuminosity( sig->Sample()->TotalLuminosity() );
   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.02,
      limit_namer.GetChannelEXT( "Root Name" ).c_str() );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.08,
      limit_namer.GetExtName( "fitfunc", "Root Name" ).c_str() );

   unsigned obs = data_set->sumEntries();
   char obs_yield[1024];
   char exp_yield[1024];
   char delta[1024];

   sprintf( obs_yield, "Observed Yield: %d",          obs );
   sprintf( exp_yield, "Fitted bkg: %.2lf #pm %.2lf", bg_strength, bg_err );
   sprintf( delta,     "#Delta: %.3lf%%",
      100.* ( bg_strength-obs )/obs );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.02,
      limit_namer.query_tree(
         "fitmethod", "SimFit", "Full Name" ).c_str() );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.07, obs_yield );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.12, exp_yield );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.17, delta );


   // Saving and cleaning up
   const string prefix = ( data->Name().find( "Data" ) != string::npos ) ?
                         "fitplot" : "valsimfitplot";
   const string mainname = limit_namer.PlotFileName( prefix, {datasetname, sig->Name()} );
   const string logname  = limit_namer.PlotFileName( prefix, {datasetname, sig->Name(), "log"} );
   const string zoomname = limit_namer.PlotFileName( prefix, {datasetname, sig->Name(), "zoom"} );
   const string zoomlog  = limit_namer.PlotFileName( prefix, {datasetname, sig->Name(), "zoom", "log"} );

   // Unzoomed plot
   c->SaveAs( mainname.c_str() );
   c->SetLogy( kTRUE );
   c->SaveAs( logname.c_str() );
   c->SetLogy( kFALSE );

   // Zooming into designated position
   const double peak_value = model_plot->Eval( GetInt( sig->Name() ) );
   frame->SetMaximum( peak_value*1.5 );
   c->SaveAs( zoomname.c_str() );
   c->SetLogy( kTRUE );
   frame->SetMaximum( peak_value*10. );
   frame->SetMinimum( peak_value*0.1 );
   c->SaveAs( zoomlog.c_str() );

   delete leg;
   delete c;
}

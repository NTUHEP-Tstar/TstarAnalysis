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
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"

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
   vector<RooAbsPdf*> bgpdf_list;
   vector<RooAbsPdf*> sigpdf_list;

   // Getting fit result from default MC dataset ;
   RooFitResult* bgconstrain = FitBackgroundTemplate( mc, "" );

   for( auto& sig : signal_list ){
      MakeFullKeysPdf( sig );
      MakeFullSimFit( data, sig, bgconstrain );
      MakeSimFitCardFile( data, sig );

      for( const auto& name : data->SetNameList() ){
         MakeSimFitPlot( data, sig, name );
      }

      bgpdf_list.push_back( data->Pdf( StitchSimFitBgPdfname( sig->Name() ) ) );
      sigpdf_list.push_back( sig->Pdf( StitchKeyPdfName ) );
   }

   SaveRooWorkSpace(
      data->DataSet( "" ),
      bgpdf_list,
      sigpdf_list
      );
}

/*******************************************************************************
*   Fitting control flow
*******************************************************************************/
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
   const string fitfunc      = limnamer.GetInput( "fitfunc" );

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

/******************************************************************************/

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
   FILE* cardfile = MakeCardCommon( dataset, bgpdf, sigpdf, sig->Name() );
   fprintf(
      cardfile, "%12s %15lf %15lf\n", "rate",
      sig->DataSet()->sumEntries(),
      bgunc.CentralValue()
      );

   // Printing list of normalization errors
   const Parameter lumi( 1, 0.062, 0.062 );
   const Parameter lepunc = data->Name().find( "Muon" ) == string::npos ?
                            Parameter( 1, 0.03, 0.03 ) : Parameter( 1, 0.03, 0.03 );
   const Parameter sigunc        = sig->Sample().SelectionEfficiency();
   const Parameter sigjecunc     = GetMCNormError( sig, "jecup",    "jecdown" );
   const Parameter sigjetresunc  = GetMCNormError( sig, "jetresup", "jetresdown" );
   const Parameter sigbjetunc    = GetMCNormError( sig, "btagup",   "btagdown" );
   const Parameter sigpuunc      = GetMCNormError( sig, "puup",     "pudown" );
   const Parameter siglepstatunc = GetMCNormError( sig, "lepup",    "lepdown" );
   const Parameter sigpdfunc     = GetMCNormError( sig, "pdfup",    "pdfdown" );
   const Parameter sigscaleunc   = GetMCNormError( sig, "scaleup", "scaledown" );
   const Parameter null( 0, 0, 0 );
   fprintf( cardfile, "----------------------------------------\n" );
   PrintNuisanceFloats( cardfile, "Lumi",     "lnN", lumi,          lumi  );
   PrintNuisanceFloats( cardfile, "sigunc",   "lnN", sigunc,        null  );
   PrintNuisanceFloats( cardfile, "bgunc",    "lnN", null,          bgunc );
   PrintNuisanceFloats( cardfile, "jec",      "lnN", sigjecunc,     null  );
   PrintNuisanceFloats( cardfile, "jer",      "lnN", sigjetresunc,  null  );
   PrintNuisanceFloats( cardfile, "btag",     "lnN", sigbjetunc,    null  );
   PrintNuisanceFloats( cardfile, "pileup",   "lnN", sigpuunc,      null  );
   PrintNuisanceFloats( cardfile, "lepstat",  "lnN", siglepstatunc, null  );
   PrintNuisanceFloats( cardfile, "lepsys",   "lnN", lepunc,        null  );
   PrintNuisanceFloats( cardfile, "pdf",      "lnN", sigpdfunc,     null  );
   PrintNuisanceFloats( cardfile, "qcdscale", "lnN", sigpdfunc,     null  );

   // Getting a list of fitting parameters to permutate.
   for( const auto& datasetname : data->SetNameList() ){
      const string bgfuncname = SimFitBGPdfName( datasetname, sig->Name() );

      for( auto var : data->VarContains( bgfuncname ) ){
         const string varname = var->GetName();
         if( varname.find( "coeff" ) == string::npos ){// We shouldn't push the joint coefficient inside
            PrintFloatParam( cardfile, var );
         }
      }
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
      PrintFlatParam( cardfile, var );
   }

   // Closing file
   fclose( cardfile );
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
GetMCNormError(
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
   const string&    datasetname,
   const string&    exttag
   )
{
   TCanvas* c     = plt::NewCanvas();
   RooPlot* frame = SampleRooFitMgr::x().frame();

   const string combpdfname = SimFitCombinePdfName( datasetname, sig->Name() );
   const string bgpdfname   = SimFitBGPdfName( datasetname, sig->Name() );

   RooDataSet* dataset = data->DataSet( datasetname );
   RooAddPdf* model    = (RooAddPdf*)data->Pdf( combpdfname );
   RooAbsPdf* bgpdf    = data->Pdf( bgpdfname );
   RooAbsPdf* sigpdf   = sig->Pdf( StitchKeyPdfName );

   const RooRealVar* bgvar     = data->Var( combpdfname + "nb" );
   const RooRealVar* sigvar    = data->Var( combpdfname + "ns" );
   const double bgstrength     = bgvar->getVal();
   const double bgerr          = bgvar->getError();
   const double sigfitstrength = sigvar->getVal();
   const double sigexpstrength = sig->ExpectedYield();


   TGraph* dataplot = plt::PlotOn(
      frame, dataset,
      RooFit::DrawOption( PGS_DATA )
   ); //Must plot data first to set y axis title

   TGraph* modelplot = plt::PlotOn(
      frame, model,
      RooFit::Normalization( bgstrength + sigfitstrength, RooAbsReal::NumEvent )
      );

   TGraph* bgplot = plt::PlotOn(
      frame, bgpdf,
      RooFit::Normalization( bgstrength, RooAbsReal::NumEvent )
      );

   TGraph* sigplot = plt::PlotOn(
      frame, sigpdf,
      RooFit::Normalization( sigexpstrength, RooAbsReal::NumEvent ),
      RooFit::DrawOption( PGS_SIGNAL )
      );

   frame->Draw();
   frame->SetMinimum( 0.3 );
   plt::SetFrame( frame );
   // frame->GetYaxis()->SetTitle( dataplot->GetYaxis()->GetTitle() );

   modelplot->SetLineColor( kGreen );
   bgplot->SetFillColor( kCyan );
   tstar::SetSignalStyle( sigplot );

   const double legend_x_min = 0.57;
   const double legend_y_min = 0.7;

   TLegend* leg = plt::NewLegend( legend_x_min, legend_y_min );
   char sig_entry[1024];
   sprintf(
      sig_entry,  "%s (%.2lf pb)",
      sig->RootName().c_str(),
      sig->Sample().CrossSection().CentralValue()
      );

   if( datasetname == "pseudo" ){
      leg->AddEntry( dataplot, "Pseudo data", "lp" );
   } else {
      leg->AddEntry( dataplot, "Data", "lp" );
   }
   leg->AddEntry( bgplot,    "Fitted Background", "l"  );
   leg->AddEntry( modelplot, "Fitted Combine",    "l"  );
   leg->AddEntry( sigplot,   sig_entry,           "lf" );
   leg->Draw();

   plt::DrawCMSLabel();
   plt::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );
   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.02, limnamer.GetChannelEXT( "Root Name" ).c_str() );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, legend_y_min-0.08,
      limnamer.GetExtName( "fitfunc", "Root Name" ).c_str() );

   unsigned obs = dataset->sumEntries();
   char obs_yield[1024];
   char exp_yield[1024];
   char delta[1024];

   sprintf( obs_yield, "Observed Yield: %d",          obs );
   sprintf( exp_yield, "Fitted bkg: %.2lf #pm %.2lf", bgstrength, bgerr );
   sprintf( delta,     "#Delta: %.3lf%%",             100.* ( bgstrength-obs )/obs );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.02, limnamer.query_tree( "fitmethod", "SimFit", "Full Name" ).c_str() );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.07, obs_yield );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.12, exp_yield );
   tl.DrawLatex( legend_x_min-0.02, PLOT_Y_MAX-0.17, delta );


   // Saving and cleaning up
   const string prefix   = ( datasetname != "pseudo" ) ? "fitplot" : "valsimfitplot";
   const string mainname = limnamer.PlotFileName( prefix, {datasetname, sig->Name(), exttag} );
   const string logname  = limnamer.PlotFileName( prefix, {datasetname, sig->Name(), exttag, "log"} );
   const string zoomname = limnamer.PlotFileName( prefix, {datasetname, sig->Name(), exttag, "zoom"} );
   const string zoomlog  = limnamer.PlotFileName( prefix, {datasetname, sig->Name(), exttag, "zoom", "log"} );

   // Saving to ROOTFILE
   plt::SaveToROOT(
      c,
      limnamer.PlotRootFile(),
      limnamer.MakeFileName( "", prefix, {datasetname, sig->Name(), exttag} )
      );

   // Unzoomed plot
   const vector<const TGraph*> graphlist = {dataplot,modelplot,sigplot, bgplot};
   const double ymax = plt::GetYmax( graphlist );
   frame->SetMaximum( ymax * 1.5 );
   plt::SaveToPDF( c, mainname );
   c->SetLogy( kTRUE );
   frame->SetMaximum( ymax * 300 );
   plt::SaveToPDF( c, logname.c_str() );
   c->SetLogy( kFALSE );

   // Zooming into designated position
   const double peak_value = modelplot->Eval( GetInt( sig->Name() ) );
   frame->SetMaximum( peak_value*1.5 );
   plt::SaveToPDF( c, zoomname.c_str() );
   c->SetLogy( kTRUE );
   frame->SetMaximum( peak_value*10. );
   frame->SetMinimum( peak_value*0.1 );
   plt::SaveToPDF( c, zoomlog.c_str() );

   delete frame;
   delete leg;
   delete c;
}

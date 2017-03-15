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
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"
#include "TstarAnalysis/LimitCalc/interface/Template.hpp"

#include "ManagerUtils/Maths/interface/RooFitExt.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"

#include <boost/format.hpp>
#include <fstream>
#include <stdio.h>

#include "RooAddPdf.h"
#include "RooFitResult.h"
#include "RooGenericPdf.h"
#include "RooProdPdf.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"
#include "TFile.h"

using namespace std;
using namespace mgr;

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
  vector<RooAbsPdf*> pdflist;
  vector<RooAbsReal*> funclist;

  // Getting fit result from default MC dataset ;
  RooFitResult* bgconstrain = FitBackgroundTemplate( mc, "" );

  for( auto& sig : signal_list ){
    MakeFullKeysPdf( sig );

    auto fiterr = SimFitSingle( data, sig, "", bgconstrain );

    for( const auto& name : data->SetNameList() ){
      MakeSimFitPlot( data, sig, fiterr, name );
    }

    MakeSimFitCardFile( data, sig );

    // pdflist.push_back( data->Pdf( SimFitBGPdfName( "", sig->Name() ) ) );
    pdflist.push_back( data->Pdf( StitchSimFitBgPdfname( sig->Name() ) ) );
    pdflist.push_back( sig->Pdf( StitchKeyPdfName ) );
    funclist.push_back( sig->Func( StitchKeyNormName ) );

  }

  SaveRooWorkSpace(
    data->DataSet( "" ),
    pdflist,
    {}
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
  const string fitfunc      = limnamer.GetInput<string>( "fitfunc" );

  RooAbsData* fitdataset = data->DataSet( datasetname );
  RooAbsPdf* bgpdf       = data->NewPdf( bgfuncname, fitfunc );
  RooAbsPdf* sigpdf      = sig->Pdf( KeyPdfName( "" ) );   // attempting simfit will default signal regardless

  const double numdata = fitdataset->sumEntries();

  RooRealVar* nb = data->NewVar( combfuncname+"nb", numdata, -1000000, 1000000 );
  RooRealVar* ns = data->NewVar( combfuncname+"ns", 0, -1000000, 1000000 );

  RooAddPdf* model = new RooAddPdf(
    combfuncname.c_str(),
    combfuncname.c_str(),
    RooArgList( *bgpdf, *sigpdf ),
    RooArgList( *nb,    *ns     )
    );

  RooFitResult* err = model->fitTo(
    *( fitdataset ),
    RooFit::Minimizer( "Minuit", "Migrad" ),
    RooFit::Extended( kTRUE ),
    RooFit::Minos( kTRUE ),
    // RooFit::ExternalConstraints( fitconstraints ),
    RooFit::Save(),
    RooFit::Verbose( kFALSE ),
    RooFit::PrintLevel( -1 ),
    RooFit::PrintEvalErrors( -1 ),
    RooFit::Warnings( kFALSE )
    );

  cout << "Finish running simultaneous fit for " << sig->Name() << endl;

  // Adding external term
  RooRealVar* c = data->NewVar( bgfuncname + "sys", 0, -5, 5 );// Item for adding background shape systematic
  RooRealVar* a = data->Var( bgfuncname + "a" );

  char formula[1024];
  sprintf(
    formula,
    "TMath::Exp( -(%s)*TMath::Power(TMath::Log(x/(%s)),3) )",
    c->GetName(),
    a->GetName()
    );
  c->setError( 0.01 );// A magic number for now
  c->setConstant( kTRUE );

  RooGenericPdf* bkgsys = new RooGenericPdf(
    ( bgfuncname + "sys" ).c_str(),
    ( bgfuncname + "sys" ).c_str(),
    formula,
    RooArgSet( SampleRooFitMgr::x(), *a, *c )
    );

  RooProdPdf* bkgtot = new RooProdPdf(
    StitchSimFitBgPdfname( sig->Name() ).c_str(),
    StitchSimFitBgPdfname( sig->Name() ).c_str(),
    RooArgSet( *bgpdf, *bkgsys )
    );

  data->AddPdf( bkgsys );
  data->AddPdf( bkgtot );
  data->AddPdf( model );
  return err;
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
  const string sigpdfname = StitchKeyPdfName;
  const string bgpdfname  = StitchSimFitBgPdfname( sig->Name() );
  RooAbsData* dataset     = data->DataSet();
  RooAbsPdf* bgpdf        = data->Pdf( bgpdfname );
  RooAbsPdf* sigpdf       = sig->Pdf( sigpdfname );
  const Parameter bgunc   = GetBgNormError( data, "", "", sig->Name() );

  // Writting the common parts
  ofstream cardfile( limnamer.TextFileName( "card", sig->Name() ) );

  MakeCardCommon( cardfile, dataset, bgpdf, sigpdf );
  cardfile << boost::format( "%12s %15lf %15lf" )
    % "rate"
    % sig->DataSet()->sumEntries()
    % bgunc.CentralValue()
           << endl;

  // Printing list of normalization errors
  const Parameter lumi   = limnamer.MasterConfig().GetStaticParameter( "Lumi Error" );
  const Parameter lepunc = data->Name().find( "Muon" ) == string::npos ?
                           limnamer.MasterConfig().GetStaticParameter( "Electron Systematic" ) :
                           limnamer.MasterConfig().GetStaticParameter( "Muon Systematic" );
  const Parameter sigunc = sig->Sample().SelectionEfficiency();
  const Parameter null( 0, 0, 0 );
  cardfile << "----------------------------------------" << endl;

  // Normalization nuisance parameter
  for( const auto& source : uncsource ){
    if( source == "pdf" || source == "scale" ){ continue; }
    const Parameter unc = GetMCNormError( sig, source + "Up", source + "Down" );
    PrintNuisanceFloats( cardfile, source, "lnN", unc, null  );
  }

  PrintNuisanceFloats( cardfile, "lumi",    "lnN", lumi,   lumi  );
  PrintNuisanceFloats( cardfile, "statunc", "lnN", sigunc, null );
  PrintNuisanceFloats( cardfile, "lepsys",  "lnN", lepunc, null  );
  PrintNuisanceFloats( cardfile, "bgunc",   "lnN", null,   bgunc );

  // Getting a list of fitting parameters to permutate.
  for( const auto& datasetname : data->SetNameList() ){
    const string bgfuncname = SimFitBGPdfName( datasetname, sig->Name() );

    for( auto var : data->VarContains( bgfuncname ) ){
      const string varname = var->GetName();
      if( varname.find( "coeff" ) == string::npos ){
        // We shouldn't push the joint coefficient inside
        PrintFloatParam( cardfile, var );
      }
    }

    // Disabling for now parametric shapes
    // for( auto var : sig->VarContains("") ){
    //   const string varname = var->GetName();
    //   if( varname.find("coeff") == string::npos ){
    //     PrintFloatParam( cardfile, var );
    //   }
    // }


    // Parameter for background systematic error
    for( auto var : data->VarContains( StitchSimFitBgPdfname( sig->Name() ) ) ){
      PrintFloatParam( cardfile, var );
    }
  }

  // Getting stitching variables
  vector<RooRealVar*> stitchvarlist;

  for( auto var : sig->VarContains( "coeff" ) ){
    stitchvarlist.push_back( var );
  }

  for( const auto& var : stitchvarlist ){
    var->setConstant( kFALSE );
    PrintFlatParam( cardfile, var );
  }

  // Closing file
  cardfile.close();
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

  cout << sig->Name() << " | " << uperrorset << " | ";
  cout << centralsetyield << " " << uperroryield << " " << downerroryield << endl;

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
  RooFitResult*    fitres,
  const string&    datasetname,
  const string&    exttag
  )
{
  TCanvas* c     = mgr::NewCanvas();
  TPad* toppad   = mgr::NewTopPad();
  TPad* botpad   = mgr::NewBottomPad();
  RooPlot* frame = SampleRooFitMgr::x().frame();

  const string combpdfname = SimFitCombinePdfName( datasetname, sig->Name() );
  const string bgpdfname   = SimFitBGPdfName( datasetname, sig->Name() );

  RooAbsData* dataset = data->DataSet( datasetname );
  RooAddPdf* model    = (RooAddPdf*)data->Pdf( combpdfname );
  RooAbsPdf* bgpdf    = data->Pdf( bgpdfname );
  RooAbsPdf* sigpdf   = sig->Pdf( KeyPdfName( "" ) );

  cout << bgpdfname << endl;

  for( const auto& pdfname : data->PdfNameList() ){
    cout << pdfname << endl;
  }

  const RooRealVar* bgvar     = data->Var( combpdfname + "nb" );
  const RooRealVar* sigvar    = data->Var( combpdfname + "ns" );
  const double obs            = dataset->sumEntries();
  const double bgstrength     = bgvar->getVal();
  const double bgerr          = bgvar->getError();
  const double sigfitstrength = sigvar->getVal();
  const double sigexpstrength = sig->ExpectedYield();
  const double ksbkg          = KSTest( *dataset, *bgpdf, SampleRooFitMgr::x() );
  const double ksall          = KSTest( *dataset, *model, SampleRooFitMgr::x() );
  const double multipler      = ( obs > sigexpstrength*4 ) ? ( obs/( sigexpstrength*4 ) ) : 1;

  /*******************************************************************************
  *   Drawing top pad
  *******************************************************************************/
  toppad->Draw();
  toppad->cd();

  TGraph* dataplot = mgr::PlotOn(
    frame, dataset,
    RooFit::DrawOption( PGS_DATA )
    );// Must plot data first to set y axis title

  TGraph* sigplot = mgr::PlotOn(
    frame, sigpdf,
    RooFit::Normalization( sigexpstrength * multipler, RooAbsReal::NumEvent ),
    RooFit::DrawOption( PGS_SIGNAL )
    );

  TGraph* bgerrplot = mgr::PlotOn(
    frame, model,
    RooFit::Components( RooArgList( *bgpdf ) ),
    RooFit::VisualizeError( *fitres, 1, kFALSE )
    );

  TGraph* bgplot = mgr::PlotOn(
    frame, bgpdf,
    RooFit::Normalization( bgstrength, RooAbsReal::NumEvent )
    );

  TGraph* modelplot = mgr::PlotOn(
    frame, model,
    RooFit::Normalization( bgstrength + sigfitstrength, RooAbsReal::NumEvent )
    );

  frame->Draw();
  frame->SetMinimum( 0.3 );
  frame->SetTitle( "" );
  mgr::SetTopPlotAxis( frame );
  c->cd();

  /*******************************************************************************
  *   Drawing bottom pad
  *******************************************************************************/
  botpad->Draw();
  botpad->cd();

  TGraphAsymmErrors* bgrelplot = mgr::DividedGraph(
    (TGraphAsymmErrors*)bgerrplot,
    bgplot
    );
  TGraphAsymmErrors* datarelplot = mgr::DividedGraph(
    (TGraphAsymmErrors*)dataplot,
    bgplot
    );
  TLine cen( SampleRooFitMgr::x().getMin(), 1, SampleRooFitMgr::x().getMax(), 1 );
  TLine lineup( SampleRooFitMgr::x().getMin(), 1.5, SampleRooFitMgr::x().getMax(), 1.5 );
  TLine linedown( SampleRooFitMgr::x().getMin(), 0.5, SampleRooFitMgr::x().getMax(), 0.5 );

  bgrelplot->Draw( "AF" );
  datarelplot->Draw( PGS_DATA );
  cen.Draw();
  cen.SetLineColor( KBLUE );
  cen.SetLineWidth( 2 );
  lineup.Draw();
  lineup.SetLineColor( kBlack );
  lineup.SetLineStyle( 3 );
  linedown.Draw();
  linedown.SetLineColor( kBlack );
  linedown.SetLineStyle( 3 );

  // Title setting
  bgrelplot->GetXaxis()->SetTitle( frame->GetXaxis()->GetTitle() );
  bgrelplot->GetXaxis()->SetRangeUser( SampleRooFitMgr::x().getMin(), SampleRooFitMgr::x().getMax() );
  bgrelplot->GetYaxis()->SetTitle( "Data/Bkg.fit" );
  bgrelplot->SetMaximum( 1.6 );
  bgrelplot->SetMinimum( 0.4 );
  mgr::SetBottomPlotAxis( bgrelplot );

  c->cd();

  /*******************************************************************************
  *   Styling options
  *******************************************************************************/
  tstar::SetFitCombStyle( modelplot );
  tstar::SetFitBGStyle( bgplot );
  tstar::SetFitBGStyle( bgerrplot );
  tstar::SetSignalStyle( sigplot );

  tstar::SetFitBGErrStyle( bgrelplot );
  tstar::SetDataStyle( datarelplot );
  tstar::SetDataStyle( dataplot );

  // Creating legend
  const double legend_x_min = 0.53;
  const double legend_y_min = 0.7;
  TLegend* leg              = mgr::NewLegend( legend_x_min, legend_y_min );

  boost::format sigfmt( "%s(#times%.1lf)" );
  const string sigentry  = str( sigfmt % sig->RootName() % multipler );
  const string dataentry = datasetname == "pseudo" ? "Pseudo data" : "Data";
  const string bgentry   = "Fitted bkg. (#pm 1 s.d.)";
  const string mdlentry  = "Fitted bkg. + sig.";

  leg->AddEntry( dataplot,  dataentry.c_str(), "lp" );
  leg->AddEntry( bgerrplot, bgentry.c_str(),   "lf" );
  leg->AddEntry( modelplot, mdlentry.c_str(),  "l"  );
  leg->AddEntry( sigplot,   sigentry.c_str(),  "l"  );
  leg->Draw();

  // Additional information
  mgr::DrawCMSLabel();
  mgr::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );
  LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
  .WriteLine( limnamer.GetChannelEXT( "Root Name" ) )
  .WriteLine( limnamer.ExtQuery<string>( "fitmethod", "SimFit", "Full Name" ) )
  .WriteLine( limnamer.GetExt<string>( "fitfunc", "Full Name" ) );

  boost::format obsfmt( "Observed Yield: %d" );
  boost::format fitfmt( "Fitted bkg.: %.0lf #pm %.0lf" );
  boost::format detfmt( "K_{bkg} = %.3lf,  K_{all} = %.3lf" );

  const string obsentry = str( obsfmt % obs );
  const string expentry = str( fitfmt % bgstrength % bgerr );
  const string detentry = str( detfmt % ksbkg % ksall );

  latex.SetOrigin( PLOT_X_TEXT_MAX, legend_y_min - TEXT_MARGIN, TOP_RIGHT )
  .WriteLine( obsentry )
  .WriteLine( expentry )
  .WriteLine( detentry );


  // Saving and cleaning up
  vector<string> taglist = ( datasetname != "pseudo" )
                           ? vector<string>( {datasetname, sig->Name(), exttag} )
                           : vector<string>( {datasetname, sig->Name(), exttag, SigStrengthTag() } );

  const string prefix   = ( datasetname != "pseudo" ) ? "fitplot" : "valsimfitplot";
  const string mainname = limnamer.PlotFileName( prefix, taglist );
  taglist.push_back( "log" );
  const string logname = limnamer.PlotFileName( prefix, taglist );
  taglist.back() = "zoom";
  const string zoomname = limnamer.PlotFileName( prefix, taglist );
  taglist.push_back( "log" );
  const string zoomlog = limnamer.PlotFileName( prefix, taglist );

  // Saving to ROOTFILE
  mgr::SaveToROOT(
    c,
    limnamer.PlotRootFile(),
    limnamer.OptFileName( "", prefix, taglist )
    );

  // Unzoomed plot
  const vector<TGraph*> graphlist = {dataplot, modelplot, sigplot, bgplot};
  const double ymax               = mgr::GetYmax( graphlist );
  frame->SetMaximum( ymax * 1.5 );
  mgr::SaveToPDF( c, mainname );
  toppad->SetLogy( kTRUE );
  frame->SetMaximum( ymax * 3000 );
  mgr::SaveToPDF( c, logname.c_str() );
  toppad->SetLogy( kFALSE );

  // Zooming into designated position
  const double peak_value = modelplot->Eval( GetInt( sig->Name() ) );
  frame->SetMaximum( peak_value*1.5 );
  mgr::SaveToPDF( c, zoomname.c_str() );
  toppad->SetLogy( kTRUE );
  frame->SetMaximum( peak_value*10. );
  frame->SetMinimum( peak_value*0.1 );
  mgr::SaveToPDF( c, zoomlog.c_str() );

  delete frame;
  delete leg;
  delete c;
}

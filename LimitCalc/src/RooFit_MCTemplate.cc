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

#include "ManagerUtils/Maths/interface/RooFitExt.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"

#include <boost/format.hpp>
#include <fstream>
#include <string>

#include "RooDataSet.h"

using namespace std;
using namespace mgr;

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

  auto fitresultlist = MakeFullTemplate( bg );
  MakeTemplatePlot( data, bg, signal_list.front(), fitresultlist.front(), true );
  MakeTemplatePlot( data, bg, signal_list.front(), fitresultlist.front(), false );
  pdflist.push_back( bg->Pdf( StitchTemplatePdfName ) );

  SaveRooWorkSpace(
    data->DataSet( "" ),
    pdflist,
    {}
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

  RooAbsPdf* bgpdf = bg->NewPdf( bgpdfname, limnamer.GetInput<string>( "fitfunc" ) );

  RooFitResult* ans = bgpdf->fitTo(
    *( bg->DataSet( datatag ) ),
    RooFit::Save(),
    RooFit::SumW2Error( kTRUE ),// For Weighted dataset
    RooFit::Minimizer( "Minuit2", "Migrad" ),
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

vector<RooFitResult*>
MakeFullTemplate( SampleRooFitMgr* bg )
{
  vector<string> bgpdflist;
  vector<RooFitResult*> ans;

  for( const auto& datasetname : bg->SetNameList() ){
    ans.push_back( FitBackgroundTemplate( bg, datasetname ) );
    bgpdflist.push_back( TemplatePdfName( datasetname ) );
  }

  MakeSimpleStitchPdf( bg, StitchTemplatePdfName, bgpdflist );
  return ans;
}

/******************************************************************************/

void
MakeTemplateCardFile( SampleRooFitMgr* data, SampleRooFitMgr* bg, SampleRooFitMgr* sig )
{
  RooAbsData* dataobs = data->DataSet( "" );
  RooAbsPdf* bgpdf    = bg->Pdf( StitchTemplatePdfName );
  RooAbsPdf* sigpdf   = sig->Pdf( StitchKeyPdfName );

  ofstream cardfile( limnamer.TextFileName( "card", {sig->Name()} ) );

  MakeCardCommon( cardfile, dataobs, bgpdf, sigpdf );

  cardfile << boost::format( "%12s %15lf %15lf" )
    % "rate"
    % sig->DataSet()->sumEntries()
    % bg->DataSet()->sumEntries()
           << endl;

  cardfile << "----------------------------------------" << endl;

  const Parameter null( 0, 0, 0 );
  const Parameter lumi( 1, 0.062, 0.062 );
  const Parameter lepunc( 1, 0.03, 0.03 );
  const Parameter sigstatunc = sig->Sample().SelectionEfficiency();
  const Parameter bkgstatunc( 1, 0.03, 0.03 );// includes uncertaintly from cross section and selection effiency

  const Parameter sigjecunc  = GetMCNormError( sig, "jecUp",    "jecDown"    );
  const Parameter sigjerunc  = GetMCNormError( sig, "jetresUp", "jetresDown" );
  const Parameter siglepunc  = GetMCNormError( sig, "lepUp",    "lepDown"    );
  const Parameter sigbtagunc = GetMCNormError( sig, "btagUp",   "btagDown"   );
  const Parameter sigpuunc   = GetMCNormError( sig, "puUp",     "puDown"     );
  const Parameter sigpdfunc  = GetMCNormError( sig, "pdfUp",    "pdfDown"    );

  const Parameter bkgjecunc  = GetMCNormError( bg, "jecUp",    "jecDown"    );
  const Parameter bkgjerunc  = GetMCNormError( bg, "jetresUp", "jetresDown" );
  const Parameter bkglepunc  = GetMCNormError( bg, "lepUp",    "lepDown"    );
  const Parameter bkgbtagunc = GetMCNormError( bg, "btagUp",   "btagDown"   );
  const Parameter bkgpuunc   = GetMCNormError( bg, "puUp",     "puDown"     );
  const Parameter bkgpdfunc  = GetMCNormError( bg, "pdfUp",    "pdfDown"    );

  PrintNuisanceFloats( cardfile, "Lumi",    "lnN", lumi,       lumi        );
  PrintNuisanceFloats( cardfile, "lepsys",  "lnN", lepunc,     lepunc      );
  PrintNuisanceFloats( cardfile, "sigstat", "lnN", sigstatunc, null        );
  PrintNuisanceFloats( cardfile, "bkgstat", "lnN", null,       bkgstatunc  );
  PrintNuisanceFloats( cardfile, "jec",     "lnN", sigjecunc,  bkgjecunc   );
  PrintNuisanceFloats( cardfile, "jer",     "lnN", sigjerunc,  bkgjerunc   );
  PrintNuisanceFloats( cardfile, "lep",     "lnN", siglepunc,  bkglepunc   );
  PrintNuisanceFloats( cardfile, "btag",    "lnN", sigbtagunc, bkgbtagunc  );
  PrintNuisanceFloats( cardfile, "pileup",  "lnN", sigpuunc,   bkgpuunc    );
  PrintNuisanceFloats( cardfile, "pdf",     "lnN", sigpdfunc,  bkgpdfunc   );

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

  cardfile.close();
}


/*******************************************************************************
*   Plotting fit results
*******************************************************************************/
void
MakeTemplatePlot(
  SampleRooFitMgr* data,
  SampleRooFitMgr* mc,
  SampleRooFitMgr* signal,
  RooFitResult*    fitresult,
  const bool       use_data )
{
  // First plot against MC
  const double TotalLuminosity = mgr::SampleMgr::TotalLuminosity();
  const double xmin            = SampleRooFitMgr::x().getMin();
  const double xmax            = SampleRooFitMgr::x().getMax();

  TCanvas* c     = mgr::NewCanvas();
  TPad* toppad   = mgr::NewTopPad();
  TPad* botpad   = mgr::NewBottomPad();
  RooPlot* frame = SampleRooFitMgr::x().frame();

  // Objects to draw
  RooDataSet* set   = (RooDataSet*)( use_data ? data->DataSet() : mc->DataSet() );
  RooAbsPdf* bkgpdf = mc->Pdf( TemplatePdfName( "" ) );// Taking central value only
  RooAbsPdf* sigpdf = signal->Pdf( StitchKeyPdfName );

  /*******************************************************************************
  *   Objects for drawing top pad
  *******************************************************************************/
  toppad->Draw();
  toppad->cd();

  TGraph* setplot = mgr::PlotOn(
    frame,
    set,
    RooFit::DrawOption( PGS_DATA )
    );

  TGraph* pdferrplot = mgr::PlotOn(
    frame,
    bkgpdf,
    RooFit::VisualizeError( *fitresult, 1, kFALSE )
    );

  TGraph* pdfplot = mgr::PlotOn(
    frame,
    bkgpdf,
    RooFit::Normalization( set->sumEntries(), RooAbsReal::NumEvent )
    );

  TGraph* sigplot = mgr::PlotOn(
    frame,
    sigpdf,
    RooFit::DrawOption( PGS_SIGNAL ),
    RooFit::Normalization( signal->ExpectedYield(), RooAbsReal::NumEvent )
    );

  // Typical styling options
  frame->Draw();
  frame->SetMinimum( 0.3 );
  mgr::SetTopPlotAxis( frame );

  c->cd();
  /*******************************************************************************
  *   Objects for drawing bottom pad
  *******************************************************************************/
  botpad->Draw();
  botpad->cd();

  TGraphAsymmErrors* bgrelplot = mgr::DividedGraph(
    (TGraphAsymmErrors*)pdferrplot,
    pdfplot
    );
  TGraphAsymmErrors* datarelplot = mgr::DividedGraph(
    (TGraphAsymmErrors*)setplot,
    pdfplot
    );

  bgrelplot->Draw( "AF" );
  datarelplot->Draw( PGS_DATA );

  TLine cen( xmin, 1, xmax, 1 );
  TLine lineup( xmin, 1.5, xmax, 1.5 );
  TLine linedown( xmin, 0.5, xmax, 0.5 );
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
  bgrelplot->GetXaxis()->SetRangeUser( xmin, xmax );
  bgrelplot->GetYaxis()->SetTitle( "Data/Bkg.fit" );
  bgrelplot->SetMaximum( 1.6 );
  bgrelplot->SetMinimum( 0.4 );
  mgr::SetBottomPlotAxis( bgrelplot );

  c->cd();
  /*******************************************************************************
  *   Common styling and additiona objects
  *******************************************************************************/
  tstar::SetSignalStyle( sigplot );
  tstar::SetFitBGStyle( pdfplot );
  tstar::SetFitBGStyle( pdferrplot );
  tstar::SetSignalStyle( sigplot );

  tstar::SetFitBGErrStyle( bgrelplot );
  tstar::SetDataStyle( datarelplot );
  tstar::SetDataStyle( setplot );

  // Legend entries
  const double legend_x_min = 0.53;
  const double legend_y_min = 0.70;
  TLegend* l                = mgr::NewLegend( legend_x_min, legend_y_min );
  boost::format sigfmt( "%s (%.2lf pb)" );
  const string sigentry  = str( sigfmt % signal->RootName() % signal->Sample().CrossSection().CentralValue() );
  const string dataentry = ( use_data ) ? "Data" : "M.C. Bkg.";
  const string fitentry  = string( "Bkg. fit to MC" ) + ( use_data ? "(Norm.)" : "" );


  l->AddEntry( setplot, dataentry.c_str(), "lp" );
  l->AddEntry( pdfplot, fitentry.c_str(),  "l"  );
  l->AddEntry( sigplot, sigentry.c_str(),  "l"  );
  l->Draw();

  // Additional information plotting
  boost::format goffmt( "K_{prob} = %.3lf" );
  const double ksprob   = KSTest( *( set ), *( bkgpdf ), SampleRooFitMgr::x() );
  const string gofentry = str( goffmt % ksprob );

  mgr::DrawCMSLabel();
  mgr::DrawLuminosity( TotalLuminosity );

  LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
  .WriteLine( limnamer.GetChannelEXT( "Root Name" ) )
  .WriteLine( limnamer.GetExt<string>( "fitfunc", "Root Name" ) );
  latex.SetOrigin( PLOT_X_TEXT_MAX, legend_y_min-TEXT_MARGIN, TOP_RIGHT )
  .WriteLine( gofentry );

  // Range setting and saving
  const double ymax = mgr::GetYmax( pdfplot, setplot, sigplot );
  frame->SetMaximum( ymax * 1.5 );

  const string rootfile = limnamer.PlotRootFile();
  const string lastag   = use_data ? "fitmc-vs-data" : "fitmc-vs-mc";
  mgr::SaveToPDF( c, limnamer.PlotFileName( "", "fitplot", signal->Name(), lastag ) );
  mgr::SaveToROOT( c, rootfile, limnamer.PlotFileName( "fitplot", signal->Name(), lastag ) );
  frame->SetMaximum( ymax * 300 );
  toppad->SetLogy( kTRUE );
  mgr::SaveToPDF( c, limnamer.PlotFileName( "fitplot", signal->Name(), lastag, "log" ) );

  // Cleaning up
  delete frame;
  delete c;
  delete l;
}

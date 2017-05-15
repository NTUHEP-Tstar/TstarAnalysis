/*******************************************************************************
*
*  Filename    : RooFit_CompareBackground.cc
*  Description : Control flow of background function comparison fitting and plotting
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include "ManagerUtils/Maths/interface/RooFitExt.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"

#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooMinimizer.h"
#include "RooNLLVar.h"
#include "TMath.h"

#include <boost/format.hpp>
#include <vector>

using namespace std;

static Color_t sequence[] = {
  KBLUE,
  KGREEN,
  KRED,
  KPURPLE
};

struct MyFitResult
{
  RooFitResult*      fitresult;
  TGraph*            fitplot;
  TGraphAsymmErrors* pullplot;
  double             ksvalue;
};

/******************************************************************************/

void
CompareFitFunc( SampleRooFitMgr* mgr )
{
  TCanvas* c        = mgr::NewCanvas();
  TPad* toppad      = mgr::NewTopPad();
  TPad* botpad      = mgr::NewBottomPad();
  RooPlot* frame    = SampleRooFitMgr::x().frame();
  const double xmin = SampleRooFitMgr::x().getMin();
  const double xmax = SampleRooFitMgr::x().getMax();

  vector<MyFitResult> fitlist;
  const vector<string> funclist = limnamer.GetInputList<string>( "fitfunc" );

  /*******************************************************************************
  *   Making the top plots
  *******************************************************************************/
  toppad->Draw();
  toppad->cd();

  TGraphAsymmErrors* setplot = (TGraphAsymmErrors*)mgr::PlotOn(
    frame, mgr->DataSet( "" ),
    RooFit::DrawOption( PGS_DATA )
    );

  vector<double> prevfitvaluelist = {280, 0.51};

  for( const auto& fitfunc : funclist ){
    RooAbsPdf* pdf  = mgr->NewPdf( fitfunc, fitfunc );
    RooDataSet* set = (RooDataSet*)( mgr->DataSet( "" ) );

    // Setting up fitting parameters with previous fit results for faster convertions
    vector<RooRealVar*> fitparams = mgr->VarContains( fitfunc );

    for( size_t i = 0; i < fitparams.size(); ++i ){
      if( i < prevfitvaluelist.size() ){
        *( fitparams.at( i ) ) = prevfitvaluelist.at( i );
      } else {
        *( fitparams.at( i ) ) = 0;
      }
    }

    // Manually calling NNL minizer functions
    static RooCmdArg min    = RooFit::Minimizer( "Minuit", "Migrad" );
    static RooCmdArg sumerr = RooFit::SumW2Error( kTRUE );
    static RooCmdArg minos  = RooFit::Minos( kTRUE );
    static RooCmdArg save   = RooFit::Save();
    // static RooCmdArg ncpu   = RooFit::NumCPU( 6 );
    static RooCmdArg verb   = RooFit::Verbose( kFALSE );
    static RooCmdArg printl = RooFit::PrintLevel( -1 );
    static RooCmdArg printe = RooFit::PrintEvalErrors( -1 );
    static RooCmdArg printw = RooFit::Warnings( kFALSE );

    RooLinkedList fitopt;
    fitopt.Add( &min    );
    fitopt.Add( &sumerr );
    fitopt.Add( &minos  );
    fitopt.Add( &save   );
    // fitopt.Add( &ncpu   );
    fitopt.Add( &verb   );
    fitopt.Add( &printl );
    fitopt.Add( &printe );
    fitopt.Add( &printw );

    RooFitResult* ans = NULL;
    const unsigned maxiter = 50;
    unsigned iter = 0;
    while( !ans ){
      ans = pdf->fitTo( *set, fitopt );
      if( ans->status() ){// Not properly converged
        ++iter;
        if( iter > maxiter ) { break; }
        delete ans;
        ans = NULL;
      }
    }

    MyFitResult x;

    // Saving fit results
    x.fitresult = ans;
    x.fitplot   = mgr::PlotOn( frame, pdf );
    x.ksvalue   = KSTest( *set, *pdf, SampleRooFitMgr::x() );
    x.pullplot  = mgr::DividedGraph( setplot, x.fitplot );

    // Small information dump after fit
    prevfitvaluelist.clear();
    cout << "Minimum NLL:" << x.fitresult->minNll() << endl;
    cout << "Parameter fit values: " << endl;

    for( int i = 0; i < x.fitresult->floatParsFinal().getSize(); ++i ){
      RooRealVar* var = (RooRealVar*)( x.fitresult->floatParsFinal().at( i ) );
      cout << var->GetName() << " " << var->getVal() << " " << var->getError() << endl;
      prevfitvaluelist.push_back( var->getVal() );
    }

    cout << "===" << endl;

    fitlist.push_back( x );
  }


  frame->Draw();
  frame->SetMinimum( 0.3 );
  mgr::SetTopPlotAxis( frame );
  frame->SetTitle( "" );
  c->cd();

  /*******************************************************************************
  *   Drawing bottom pad.
  *******************************************************************************/
  botpad->Draw();
  botpad->cd();

  TMultiGraph* mg = new TMultiGraph;

  for( const auto fitresult : fitlist ){
    mg->Add( fitresult.pullplot, "Z" );
  }

  TLine line( xmin, 1, xmax, 1 );
  TLine line_top( xmin, 1.5, xmax, 1.5 );
  TLine line_bot( xmin, 0.5, xmax, 0.5 );

  mg->Draw( "AZ" );
  line.Draw( "SAME" );
  line_top.Draw( "SAME" );
  line_bot.Draw( "SAME" );
  mg->GetXaxis()->SetTitle( frame->GetXaxis()->GetTitle() );
  mg->GetYaxis()->SetTitle( "Data/Fit" );
  mg->GetXaxis()->SetRangeUser( xmin, xmax );
  mgr::SetBottomPlotAxis( mg );
  mg->SetMaximum( 1.6 );
  mg->SetMinimum( 0.4 );
  c->cd();

  /*******************************************************************************
  *   Object styling
  *******************************************************************************/
  tstar::SetDataStyle( setplot );

  line.SetLineColor( KRED );
  line.SetLineStyle( 1 );
  line_top.SetLineColor( kBlack );
  line_bot.SetLineColor( kBlack );
  line_top.SetLineStyle( 3 );
  line_bot.SetLineStyle( 3 );

  // Drawing common
  mgr::DrawCMSLabel();
  mgr::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );

  // Drawing legends
  TLegend* l = mgr::NewLegend( 0.45, 0.6 );
  if( limnamer.GetInput<string>( "sample" ) == "Data" ){
    l->AddEntry( setplot, "Data", "lp" );
  } else {
    l->AddEntry( setplot, "MC background", "lp" );
  }

  for( size_t i = 0; i < fitlist.size(); ++i ){
    auto fitplot  = fitlist.at( i ).fitplot;
    auto pullplot = fitlist.at( i ).pullplot;
    fitplot->SetLineColor( sequence[i] );
    pullplot->SetLineColor( sequence[i] );
    fitplot->SetLineWidth( 2 );
    pullplot->SetLineWidth( 2 );
    auto funcname = funclist.at( i );

    boost::format fitentryfmt( "%s K=%.3lf" );
    const string fitentry = boost::str(
      fitentryfmt
      % limnamer.ExtQuery<string>( "fitfunc", funcname, "Full Name" )
      % fitlist.at( i ).ksvalue
      );
    l->AddEntry( fitplot, fitentry.c_str(), "l" );
  }

  l->Draw();

  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
  .WriteLine( limnamer.GetChannelEXT( "Root Name" ) );

  boost::format chi2testfmt( "LR Test_{%d/%d} = %.3lf" );

  // Writing LR test results
  for( size_t i = 0; i+1 < fitlist.size(); ++i ){
    const double thisnll  = fitlist[i].fitresult->minNll();
    const double nextnll  = fitlist[i+1].fitresult->minNll();
    const int thisorder   = fitlist[i].fitresult->floatParsFinal().getSize();
    const int nextorder   = fitlist[i+1].fitresult->floatParsFinal().getSize();
    const double chi2test = TMath::Prob( 2*( std::max( fabs( thisnll-nextnll ), 0. ) ), nextorder-thisorder );

    latex.WriteLine( boost::str( chi2testfmt % thisorder % nextorder % chi2test ) );
  }

  /*******************************************************************************
  *   Updating everything and saving
  *******************************************************************************/
  toppad->Update();
  botpad->Update();
  c->Update();

  frame->SetMaximum( mgr::GetYmax( setplot )*1.5 );
  mgr::SaveToPDF( c, limnamer.PlotFileName( "bkgcomp" ) );
  mgr::SaveToROOT( c, limnamer.PlotRootFile(), limnamer.PlotFileName( "bkgcomp" ) );
  toppad->SetLogy( kTRUE );
  frame->SetMaximum( mgr::GetYmax( setplot )*300 );
  mgr::SaveToPDF( c, limnamer.PlotFileName( "bkgcomp", "log" ) );


  /*******************************************************************************
  *   Printing results and leaving
  *******************************************************************************/
  for( auto fit : fitlist ){
    cout << boost::str( boost::format( "%.6lf" ) % fit.fitresult->minNll() ) << endl;

    for( int i = 0; i < fit.fitresult->floatParsFinal().getSize(); ++i ){
      RooRealVar* var = (RooRealVar*)( fit.fitresult->floatParsFinal().at( i ) );
      cout << var->GetName() << " " << var->getVal() << " " << var->getError() << endl;
    }

    cout << "===" << endl;
  }

  delete toppad;
  delete botpad;
  delete c;
  delete frame;
}

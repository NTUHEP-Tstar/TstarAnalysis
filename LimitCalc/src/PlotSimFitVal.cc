/*******************************************************************************
*
*  Filename    : PlotSimFitVal.cc
*  Description : Plotting Pull distribution and others
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"

#include "ManagerUtils/Maths/interface/RooFitExt.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"

#include <algorithm>
#include <boost/format.hpp>
#include <fstream>

#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooRealVar.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"

using namespace std;
using namespace mgr;

/*******************************************************************************
*   Declaring global static variables
*******************************************************************************/
static RooRealVar p( "p", "p", 0, -7, 7 );

/*******************************************************************************
*   Defining datatype
*******************************************************************************/
const unsigned BKG_IDX = 0;
const unsigned SIG_IDX = 1;
const unsigned P1_IDX  = 2;
const unsigned P2_IDX  = 3;

struct PullResult
{
  pair<Parameter, Parameter> fitparm[4];
};


/*******************************************************************************
*   Defining main control flows
*******************************************************************************/
void
PlotGenFit( const vector<string>& masslist )
{

  map<int, PullResult> pullresults;

  p.setRange( "reduce", -1, 1 );

  for( const auto& masspoint : masslist ){
    const int mass = GetInt( masspoint );
    pullresults[mass] = PlotSingleGenFit( masspoint );
    cout << mass << endl;
  }

  MakePullComparePlot( pullresults, BKG_IDX, "bkg"    );
  MakePullComparePlot( pullresults, SIG_IDX, "sig"    );
  // MakePullComparePlot( pullresults, P1_IDX ,"param1" );
  // MakePullComparePlot( pullresults, P2_IDX ,"param2" );

}

/******************************************************************************/

PullResult
PlotSingleGenFit( const std::string& masstag )
{
  const string strgthtag = SigStrengthTag();
  const string filename  = limnamer.TextFileName( "valsimfit", masstag, strgthtag );
  ifstream result( filename );

  RooDataSet bkgset( "bkg", "bkg", RooArgSet( p ) );
  RooDataSet sigset( "sig", "sig", RooArgSet( p ) );
  RooDataSet p1set( "p1", "p1", RooArgSet( p ) );
  RooDataSet p2set( "p2", "p2", RooArgSet( p ) );
  RooDataSet signormset( "signorm", "signorm", RooArgSet( p ) );
  double bkg_real, sig_real, p1_real, p2_real;
  double bkg_fit, bkg_fiterr;
  double sig_fit, sig_fiterr;
  double p1_fit, p1_fiterr;
  double p2_fit, p2_fiterr;

  PullResult ans;

  // Begin reading files
  cout << "Reading file " << filename << endl;
  result >> bkg_real >> sig_real >> p1_real >> p2_real;

  while( result >> bkg_fit >>  bkg_fiterr >> sig_fit >>  sig_fiterr >> p1_fit >>  p1_fiterr >> p2_fit >>  p2_fiterr ){

    // Making pull distribution
    double bkgpull = ( bkg_fit - bkg_real ) / bkg_fiterr;
    double sigpull = ( sig_fit - sig_real ) / sig_fiterr;
    double p1pull  = ( p1_fit  - p1_real  ) / p1_fiterr;
    double p2pull  = ( p2_fit  - p2_real  ) / p2_fiterr;
    double signorm = ( sig_fit - sig_real ) / sig_real;

    if(
      bkgpull < -7 || bkgpull > +7 ||
      sigpull < -7 || sigpull > +7 ||
      p1pull  < -7 || p1pull  > +7 ||
      p2pull  < -7 || p2pull  > +7
      ){ continue; }

    p = bkgpull;
    bkgset.add( RooArgSet( p ) );
    p = sigpull;
    sigset.add( RooArgSet( p ) );
    p = p1pull;
    p1set.add( RooArgSet( p ) );
    p = p2pull;
    p2set.add( RooArgSet( p ) );
    p = signorm;
    signormset.add( RooArgSet( p ) );
  }

  ans.fitparm[BKG_IDX] = MakePullPlot( bkgset, masstag, "bkg"    );
  ans.fitparm[SIG_IDX] = MakePullPlot( sigset, masstag, "sig"    );
  ans.fitparm[P1_IDX]  = MakePullPlot( p1set,  masstag, "param1" );
  ans.fitparm[P2_IDX]  = MakePullPlot( p2set,  masstag, "param2" );

  return ans;
}


/*******************************************************************************
*   Plotting functions
*******************************************************************************/
pair<Parameter, Parameter>
MakePullPlot( RooDataSet& set, const string& masstag, const string& tag )
{
  RooRealVar mean( "m", "m", 0, -5, 5 );
  RooRealVar sigma( "s", "s", 1,  0, 5 );
  RooGaussian pullfit( "pull", "pull", p, mean, sigma );
  RooRealVar smean( "sm", "sm", 0, -5, 5 );
  RooRealVar ssigma( "ss", "ss", 1,  0, 5 );
  RooGaussian spullfit( "spull", "spull", p, smean, ssigma );

  TCanvas* c = mgr::NewCanvas();

  pullfit.fitTo( set,
    RooFit::Minimizer( "Minuit2", "Migrad" ),
    RooFit::Minos( kTRUE ),
    RooFit::Verbose( kFALSE ),
    RooFit::PrintLevel( -1 ),
    RooFit::PrintEvalErrors( -1 ),
    RooFit::Warnings( kFALSE )
    );

  spullfit.fitTo( set,
    RooFit::Minimizer( "Minuit2", "Migrad" ),
    RooFit::Minos( kTRUE ),
    RooFit::Verbose( kFALSE ),
    RooFit::PrintLevel( -1 ),
    RooFit::PrintEvalErrors( -1 ),
    RooFit::Warnings( kFALSE ),
    RooFit::Range( "reduce" )// Fitting to sub-range
    );

  RooPlot* frame  = p.frame();
  TGraph* setplot = mgr::PlotOn( frame, &set,
    RooFit::Binning( 40, p.getMin(), p.getMax() )
    );
  TGraph* fitplot = mgr::PlotOn( frame, &pullfit );
  TGraph* subplot = mgr::PlotOn( frame, &spullfit, RooFit::Range( "reduce" ) );
  frame->Draw();

  // Styling
  fitplot->SetLineColor( KBLUE );
  subplot->SetLineColor( KRED  );

  const double ymax = mgr::GetYmax( setplot, fitplot, subplot );
  frame->SetMaximum( ymax * 1.6 );

  // Calculation Kolmogorov-Smirnov Goodness-of-Fit Test
  const double ksres  = KSTest( set, pullfit, p );
  const double sksres = KSTest( set, spullfit, p, RooFit::CutRange( "reduce" ) );

  // Styling plots
  mgr::SetFrame( frame );
  mgr::DrawCMSLabel( SIMULATION );
  mgr::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );

  // Title Formats
  boost::format xtitlefmt( "pull_{%s}" );
  boost::format signalfmt( "Sig. mass = %d GeV/c^{2}" );
  boost::format fitfmt(  "#mu = %.2lf_{#pm%.3lf}  #sigma = %.3lf_{#pm%.3lf}"  );
  boost::format sfitfmt( "#mu' = %.2lf_{#pm%.3lf} #sigma' = %.3lf_{#pm%.3lf}"  );
  boost::format ksfmt( "K = %.3lf" );
  boost::format injectfmt( "signal events %1% %2%" );

  const string xtitle_s = str( xtitlefmt % tag );
  const string signal_s = str( signalfmt % GetInt( masstag ) );
  const string fit_s    = str( fitfmt    % mean.getVal() % mean.getError() % sigma.getVal() % sigma.getError() );
  const string sfit_s   = str( sfitfmt   % smean.getVal()% smean.getError()%ssigma.getVal() %ssigma.getError() );
  const string inject_s = limnamer.CheckInput( "relmag" ) ? boost::str( injectfmt % 'x' % limnamer.GetInput<double>( "relmag" ) ) :
                          boost::str( injectfmt % '=' % limnamer.GetInput<double>( "absmag" ) );

  // Making titles
  frame->SetTitle( "" );
  frame->GetXaxis()->SetTitle( xtitle_s.c_str() );

  // Legend for marking
  TLegend* tl = mgr::NewLegend( 0.5, 0.6 );
  tl->AddEntry( fitplot,        fit_s.c_str(),                     "l" );
  tl->AddEntry( (TObject*)NULL, boost::str( ksfmt%ksres ).c_str(),  "" );
  tl->AddEntry( subplot,       sfit_s.c_str(),                     "l" );
  tl->AddEntry( (TObject*)NULL, boost::str( ksfmt%sksres ).c_str(), "" );
  tl->Draw();

  LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT );
  latex.WriteLine( limnamer.GetChannelEXT( "Root Name" ) );
  latex.WriteLine( limnamer.GetExt<string>( "fitfunc", "Root Name" ) );
  latex.SetOrigin( PLOT_X_TEXT_MAX, 0.6, TOP_RIGHT );
  latex.WriteLine( signal_s );
  latex.WriteLine( inject_s );

  mgr::SaveToPDF( c, limnamer.PlotFileName( "valpulldist", masstag, tag, SigStrengthTag() ) );

  delete frame;
  delete c;

  return pair<Parameter, Parameter>( mean, sigma );
}


void
MakePullComparePlot(
  const map<int, PullResult>& pullresultlist,
  const unsigned idx,
  const string& tag
  )
{
  TCanvas* c = mgr::NewCanvas();
  c->SetLeftMargin( PLOT_X_MIN );
  c->SetRightMargin( 1 - PLOT_X_MAX );
  c->SetBottomMargin( PLOT_Y_MIN );
  c->SetTopMargin( 1 - PLOT_Y_MAX );

  TGraphAsymmErrors* graph   = new TGraphAsymmErrors( pullresultlist.size() );
  TGraphAsymmErrors* meanerr = new TGraphAsymmErrors( pullresultlist.size() );
  TGraphAsymmErrors* uperr   = new TGraphAsymmErrors( pullresultlist.size() );
  TGraphAsymmErrors* lowerr  = new TGraphAsymmErrors( pullresultlist.size() );

  unsigned i = 0;

  for( const auto& resultpair : pullresultlist ){
    const int mass          = resultpair.first;
    const PullResult result = resultpair.second;
    const Parameter fitmean = result.fitparm[idx].first;
    const Parameter fitsig  = result.fitparm[idx].second;

    graph->SetPoint( i, mass, fitmean.CentralValue() );
    meanerr->SetPoint( i, mass, fitmean.CentralValue() );
    uperr->SetPoint( i, mass, fitmean.CentralValue() + fitsig.CentralValue() );
    lowerr->SetPoint( i, mass, fitmean.CentralValue() - fitsig.CentralValue() );

    graph->SetPointError( i, 0, 0, fitsig.CentralValue(), fitsig.CentralValue() );
    meanerr->SetPointError( i, 0, 0, fabs( fitmean.AbsLowerError() ), fabs( fitmean.AbsUpperError() ) );
    uperr->SetPointError( i, 0, 0, fabs( fitsig.AbsLowerError() ), fabs( fitsig.AbsUpperError() ) );
    lowerr->SetPointError( i, 0, 0, fabs( fitsig.AbsUpperError() ), fabs( fitsig.AbsLowerError() ) );
    ++i;
  }

  TMultiGraph* mg = new TMultiGraph();
  mg->Add( meanerr, "A3" );
  mg->Add( uperr,   "A3" );
  mg->Add( lowerr,  "A3" );
  mg->Add( graph,   "LP" );

  mg->Draw( "A" );
  mg->SetTitle( "" );
  mg->GetXaxis()->SetTitle( "Signal mass (GeV/c^{2})" );
  mg->GetYaxis()->SetTitle( ( "pull_{"+tag+"}" ).c_str() );
  mg->SetMaximum( 3.25 );
  mg->SetMinimum( -1.75 );
  mgr::SetAxis( mg );
  mgr::DrawCMSLabel( SIMULATION );

  // Styling graphs
  graph->SetMarkerStyle( 21 );
  meanerr->SetFillStyle( 1001 );
  meanerr->SetFillColor( kBlue );
  meanerr->SetLineColor( kBlue );
  uperr->SetFillStyle( 1001 );
  uperr->SetFillColor( kCyan );
  uperr->SetFillColor( kCyan );
  lowerr->SetFillStyle( 1001 );
  lowerr->SetFillColor( kCyan );
  lowerr->SetFillColor( kCyan );


  const double xmin = mg->GetXaxis()->GetXmin();
  const double xmax = mg->GetXaxis()->GetXmax();
  TLine z( xmin, 0, xmax, 0 );
  TLine hi( xmin, 1, xmax, 1 );
  TLine lo( xmin, -1, xmax, -1 );
  z.SetLineColor( kRed );
  z.SetLineWidth( 3 );
  z.SetLineStyle( 2 );
  z.Draw();
  hi.SetLineColor( kBlue );
  hi.SetLineWidth( 2 );
  hi.SetLineStyle( 3 );
  hi.Draw();
  lo.SetLineColor( kBlue );
  lo.SetLineWidth( 2 );
  lo.SetLineStyle( 3 );
  lo.Draw();

  boost::format injectfmt( "signal events %1% %2%" );
  const string inject_s
    = limnamer.CheckInput( "relmag" ) ? boost::str( injectfmt % 'x' % limnamer.GetInput<double>( "relmag" ) ) :
      boost::str( injectfmt % '=' % limnamer.GetInput<double>( "absmag" ) );

  mgr::LatexMgr latex;
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
  .WriteLine( limnamer.GetChannelEXT( "Root Name" ) )
  .SetOrigin( PLOT_X_TEXT_MAX, PLOT_Y_TEXT_MAX, TOP_RIGHT )
  .WriteLine( limnamer.GetExt<string>( "fitfunc", "Root Name" ) )
  .WriteLine( "" )// Adding blank line
  .WriteLine( inject_s );

  mgr::SaveToPDF( c, limnamer.PlotFileName( "pullvmass", tag, SigStrengthTag() ) );

  delete graph;
  delete meanerr;
  delete uperr;
  delete lowerr;
  delete mg;
  delete c;
}

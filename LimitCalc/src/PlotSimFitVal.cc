/*******************************************************************************
*
*  Filename    : PlotSimFitVal.cc
*  Description : Plotting Pull distribution and others
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"

#include <boost/format.hpp>
#include <cstdlib>

#include "RooGaussian.h"
#include "RooRealVar.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"

using namespace std;

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

PullResult
PlotSingleGenFit( const std::string& masstag )
{
   const string strgthtag = SigStrengthTag();
   const string filename  = limnamer.TextFileName( "valsimfit", {masstag, strgthtag} );
   FILE* result           = fopen( filename.c_str(), "r" );

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
   fscanf( result, "%lf %lf %lf %lf", &bkg_real, &sig_real, &p1_real, &p2_real );

   while( 1 ){
      int scan = fscanf(
         result, "%lf %lf %lf %lf %lf %lf %lf %lf",
         &bkg_fit, &bkg_fiterr,
         &sig_fit, &sig_fiterr,
         &p1_fit, &p1_fiterr,
         &p2_fit, &p2_fiterr
         );
      if( scan == EOF ){break; }

      // Making pull distribution

      double bkgpull = ( bkg_fit - bkg_real )/bkg_fiterr;
      double sigpull = ( sig_fit - sig_real )/sig_fiterr;
      double p1pull  = ( p1_fit - p1_real )/p1_fiterr;
      double p2pull  = ( p2_fit - p2_real )/p2_fiterr;
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

   ans.fitparm[BKG_IDX] = MakePullPlot( bkgset, masstag,  "bkg"    );
   ans.fitparm[SIG_IDX] = MakePullPlot( sigset, masstag,  "sig"    );
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

   TCanvas* c = plt::NewCanvas();

   pullfit.fitTo( set,
      RooFit::Minos( kTRUE ),
      RooFit::Verbose( kFALSE ),
      RooFit::PrintLevel( -1 ),
      RooFit::PrintEvalErrors( -1 ),
      RooFit::Warnings( kFALSE )
      );

   RooPlot* frame = p.frame();
   PlotOn( frame, &set );
   PlotOn( frame, &pullfit );
   frame->Draw();

   // Styling plots
   SetFrame( frame );
   plt::DrawCMSLabel( SIMULATION );
   plt::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );

   // Title Formats
   boost::format xtitlefmt( "pull_{%1%}" );
   boost::format signalfmt( "Sig. mass = %1% GeV" );
   boost::format meanfmt( "#mu_{pull} = %.4f #pm %.4f " );
   boost::format sigmafmt( "#sigma_{pull} = %.4f #pm %.4f" );
   boost::format injectfmt( "signal events %1% %2%" );

   const string xtitle_s = str( xtitlefmt % tag );
   const string signal_s = str( signalfmt % GetInt( masstag ) );
   const string mean_s   = str( meanfmt   % mean.getVal() % mean.getError() );
   const string sigma_s  = str( sigmafmt  % sigma.getVal()% sigma.getError() );
   const string inject_s = limnamer.HasOption( "relmag" ) ?
                           str( injectfmt % 'x' % limnamer.InputDou( "relmag" ) ) :
                           str( injectfmt % '=' % limnamer.InputDou( "absmag" ) );

   // Making titles
   frame->SetTitle( "" );
   frame->GetXaxis()->SetTitle( xtitle_s.c_str() );

   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );

   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.02, limnamer.GetChannelEXT( "Root Name" ).c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.08, limnamer.GetExtName( "fitfunc", "Root Name" ).c_str() );
   tl.DrawLatex( PLOT_Y_MAX-0.02, PLOT_Y_MAX-0.20, signal_s.c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.26, inject_s.c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.32, mean_s.c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.38, sigma_s.c_str() );

   c->SaveAs( limnamer.PlotFileName( "valpulldist", {masstag, tag, SigStrengthTag()} ).c_str() );

   delete frame;
   delete c;

   cout << mean.getErrorHi() << " " << mean.getErrorLo() << endl;
   cout << sigma.getErrorHi() << " " << sigma.getErrorLo() << endl;

   return pair<Parameter, Parameter>( mean, sigma );
}


void
MakePullComparePlot(
   const map<int, PullResult>& pullresultlist,
   const unsigned idx,
   const string& tag
   )
{
   TCanvas* c = plt::NewCanvas();

   TGraphAsymmErrors* graph = new TGraphAsymmErrors( pullresultlist.size() );
   TGraphAsymmErrors* meanerr = new TGraphAsymmErrors( pullresultlist.size() );
   TGraphAsymmErrors* uperr = new TGraphAsymmErrors( pullresultlist.size() );
   TGraphAsymmErrors* lowerr = new TGraphAsymmErrors( pullresultlist.size() );

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

      graph->SetPointError( i, 0, 0  , fitsig.CentralValue(), fitsig.CentralValue() );
      meanerr->SetPointError( i, 0, 0, fabs(fitmean.AbsLowerError()), fabs(fitmean.AbsUpperError()) );
      uperr->SetPointError( i, 0, 0  , fabs(fitsig.AbsLowerError()), fabs(fitsig.AbsUpperError()) );
      lowerr->SetPointError( i, 0, 0 , fabs(fitsig.AbsUpperError()), fabs(fitsig.AbsLowerError()) );
      ++i;
   }

   TMultiGraph* mg = new TMultiGraph();
   mg->Add( meanerr, "A3" );
   mg->Add( uperr,   "A3" );
   mg->Add( lowerr,  "A3" );
   mg->Add( graph,   "LP" );

   mg->Draw("A");
   mg->SetTitle( "" );
   mg->GetXaxis()->SetTitle( "signal mass (GeV)" );
   mg->GetYaxis()->SetTitle( ( "pull_{"+tag+"}" ).c_str() );
   mg->SetMaximum( 2.75 );
   mg->SetMinimum( -2.75 );
   plt::DrawCMSLabel( SIMULATION );

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
   const string inject_s = limnamer.GetMap().count( "relmag" ) ?
                           str( injectfmt % 'x' % limnamer.GetMap()["relmag"].as<double>() ) :
                           str( injectfmt % '=' % limnamer.GetMap()["absmag"].as<double>() );

   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.02, limnamer.GetChannelEXT( "Root Name" ).c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.08, limnamer.GetExtName( "fitfunc", "Root Name" ).c_str() );
   tl.DrawLatex( PLOT_Y_MAX-0.02, PLOT_Y_MAX-0.20, inject_s.c_str() );

   c->SaveAs( limnamer.PlotFileName( "pullvmass", {tag, SigStrengthTag()} ).c_str() );

   delete graph;
   delete meanerr;
   delete uperr;
   delete lowerr;
   delete mg;
   delete c;
}

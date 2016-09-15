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
#include "TGraphErrors.h"

using namespace std;

/*******************************************************************************
*   Declaring global static variables
*******************************************************************************/
static RooRealVar p( "p", "p", 0, -7, 7 );

/*******************************************************************************
*   Defining datatype
*******************************************************************************/
struct PullResult
{
   Parameter bkgpull;
   Parameter sigpull;
   Parameter p1pull;
   Parameter p2pull;
};

// ------------------------------------------------------------------------------
//   Defining main control flows
// ------------------------------------------------------------------------------

void
PlotGenFit( const vector<string>& masslist )
{
   TGraphErrors* bkgplot = new TGraphErrors( masslist.size() );
   TGraphErrors* sigplot = new TGraphErrors( masslist.size() );
   TGraphErrors* p1plot  = new TGraphErrors( masslist.size() );
   TGraphErrors* p2plot  = new TGraphErrors( masslist.size() );

   unsigned i = 0;

   for( const auto& masspoint : masslist ){
      PullResult ans = PlotSingleGenFit( masspoint );
      const int mass = GetInt( masspoint );
      cout << mass << endl;
      bkgplot->SetPoint( i, mass, ans.bkgpull.CentralValue() );
      bkgplot->SetPointError( i, 0, ans.bkgpull.AbsAvgError() );
      sigplot->SetPoint( i, mass, ans.sigpull.CentralValue() );
      sigplot->SetPointError( i, 0, ans.sigpull.AbsAvgError() );
      p1plot->SetPoint( i, mass, ans.p1pull.CentralValue() );
      p1plot->SetPointError( i, 0, ans.p1pull.AbsAvgError() );
      p2plot->SetPoint( i, mass, ans.p2pull.CentralValue() );
      p2plot->SetPointError( i, 0, ans.p2pull.AbsAvgError() );
      ++i;
   }

   MakePullComparePlot( bkgplot, "bkg"    );
   MakePullComparePlot( sigplot, "sig"    );
   MakePullComparePlot( p1plot,  "param1" );
   MakePullComparePlot( p2plot,  "param2" );

   delete bkgplot;
   delete sigplot;
   delete p1plot;
   delete p2plot;
}

PullResult
PlotSingleGenFit( const std::string& masstag )
{
   const string strgthtag = SigStrengthTag();
   const string filename  = limit_namer.TextFileName( "valsimfit", {masstag, strgthtag} );
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

   ans.bkgpull = MakePullPlot( bkgset, masstag,  "bkg"    );
   ans.sigpull = MakePullPlot( sigset, masstag,  "sig"    );
   ans.p1pull  = MakePullPlot( p1set,  masstag, "param1" );
   ans.p2pull  = MakePullPlot( p2set,  masstag, "param2" );

   return ans;
}


// ------------------------------------------------------------------------------
//   Defining plotting functions
// ------------------------------------------------------------------------------
Parameter
MakePullPlot( RooDataSet& set, const string& masstag, const string& tag )
{
   RooRealVar mean( "m", "m", 0, -5, 5 );
   RooRealVar sigma( "s", "s", 1,  0, 5 );
   RooGaussian pullfit( "pull", "pull", p, mean, sigma );

   TCanvas c( "c", "c", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );

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
   const string inject_s = limit_namer.HasOption( "relmag" ) ?
                           str( injectfmt % 'x' % limit_namer.InputDou( "relmag" ) ) :
                           str( injectfmt % '=' % limit_namer.InputDou( "absmag" ) );

   // Making titles
   frame->SetTitle( "" );
   frame->GetXaxis()->SetTitle( xtitle_s.c_str() );

   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );

   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.02, limit_namer.GetChannelEXT( "Root Name" ).c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.08, limit_namer.GetExtName( "fitfunc", "Root Name" ).c_str() );
   tl.DrawLatex( PLOT_Y_MAX-0.02, PLOT_Y_MAX-0.20, signal_s.c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.26, inject_s.c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.32, mean_s.c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.38, sigma_s.c_str() );

   c.SaveAs( limit_namer.PlotFileName( "valpulldist", {masstag, tag, SigStrengthTag()} ).c_str() );

   return Parameter( mean.getVal(), sigma.getVal(), sigma.getVal() );
}


void
MakePullComparePlot( TGraph* graph, const string& tag )
{
   TCanvas c( "c", "c", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );
   graph->Draw( "AP" );
   graph->SetTitle( "" );
   graph->GetXaxis()->SetTitle( "signal mass (GeV)" );
   graph->GetYaxis()->SetTitle( ( "pull_{"+tag+"}" ).c_str() );
   graph->SetMarkerStyle( 21 );
   graph->SetMaximum( 2.5 );
   graph->SetMinimum( -2.5 );
   plt::DrawCMSLabel( SIMULATION );

   const double xmin = graph->GetXaxis()->GetXmin();
   const double xmax = graph->GetXaxis()->GetXmax();
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
   const string inject_s = limit_namer.GetMap().count( "relmag" ) ?
                           str( injectfmt % 'x' % limit_namer.GetMap()["relmag"].as<double>() ) :
                           str( injectfmt % '=' % limit_namer.GetMap()["absmag"].as<double>() );

   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.02, limit_namer.GetChannelEXT( "Root Name" ).c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.08, limit_namer.GetExtName( "fitfunc", "Root Name" ).c_str() );
   tl.DrawLatex( PLOT_Y_MAX-0.02, PLOT_Y_MAX-0.16, inject_s.c_str() );

   c.SaveAs( limit_namer.PlotFileName( "pullvmass", {tag, SigStrengthTag()} ).c_str() );
}

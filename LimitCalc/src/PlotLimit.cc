/*******************************************************************************
 *
 *  Filename    : PlotLimit.cc
 *  Description : Macros For plotting confidence level limits
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/Maths/interface/ParameterFormat.hpp"

#include <boost/algorithm/string.hpp>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TGraphAsymmErrors.h"

using namespace std;

//------------------------------------------------------------------------------
//   Helper static functions and variables
//------------------------------------------------------------------------------
static map<double,double>  pred_cross_section;
static void FillXSectionMap();
static Parameter GetInterSect( const TGraph* graph, const TGraph* errgraph );

void MakeLimitPlot()
{
   const mgr::ConfigReader cfg( limit_namer.MasterConfigFile() );
   const vector<string> signal_list = cfg.GetStaticStringList( "Signal List" );
   const size_t binCount = signal_list.size();
   FillXSectionMap();
   const size_t fine_bincount = pred_cross_section.size();

   double y_max = 0;
   double x_max = 0;
   double y_min = 10000000.;
   double x_min = 10000000.;
   size_t bin = 0;

   TGraphAsymmErrors* one_sig = new TGraphAsymmErrors(binCount);
   TGraphAsymmErrors* two_sig = new TGraphAsymmErrors(binCount);
   TGraph* exp                = new TGraph(binCount);
   TGraph* obs                = new TGraph(binCount);
   TGraph* theory             = new TGraph(fine_bincount);

   // Filling in theoretical part
   bin = 0 ;
   for( const auto& point : pred_cross_section ){
      const double mass = point.first;
      const double xsec = point.second ;
      theory->SetPoint( bin , mass , xsec );
      bin++;
   }

   bin=0;
   for( const auto& signal : signal_list ){
      const double mass     = GetInt(signal);
      const double exp_xsec = pred_cross_section[mass];
      const string file_name = limit_namer.RootFileName("combine",{signal});

      // Geting contents of higgs combine output file
      TFile* file = TFile::Open(file_name.c_str());
      if( !file ){
         fprintf(stderr,"Cannot open file (%s), skipping sample for %s\n" ,
            file_name.c_str(),
            signal.c_str() );
         continue;
      }
      double temp1;
      TTree* tree = ((TTree*)file->Get("limit"));
      tree->SetBranchAddress( "limit"    , &temp1 );
      tree->GetEntry(0); const double two_sig_down = temp1 * exp_xsec;
      tree->GetEntry(1); const double one_sig_down = temp1 * exp_xsec;
      tree->GetEntry(2); const double exp_lim      = temp1 * exp_xsec;
      tree->GetEntry(3); const double one_sig_up   = temp1 * exp_xsec;
      tree->GetEntry(4); const double two_sig_up   = temp1 * exp_xsec;
      tree->GetEntry(5); const double obs_lim      = temp1 * exp_xsec;
      file->Close();

      const double two_sig_err_up   = two_sig_up - exp_lim;
      const double two_sig_err_down = exp_lim    - two_sig_down;
      const double one_sig_err_up   = one_sig_up - exp_lim;
      const double one_sig_err_down = exp_lim    - one_sig_down;

      obs->SetPoint( bin, mass, obs_lim );
      exp->SetPoint( bin, mass, exp_lim );
      one_sig->SetPoint( bin, mass, exp_lim );
      one_sig->SetPointError( bin, 50, 50, one_sig_err_down, one_sig_err_up );
      two_sig->SetPoint( bin, mass, exp_lim );
      two_sig->SetPointError( bin, 50, 50, two_sig_err_down, two_sig_err_up );


      // Getting plot range
      y_max = std::max( two_sig_up, y_max );
      y_max = std::max( obs_lim   , y_max );
      y_min = std::min( two_sig_down, y_min );
      y_min = std::min( obs_lim     , y_min );
      x_max = std::max( mass , x_max );
      x_min = std::min( mass , x_min );
      ++bin;
   }

   TCanvas* c1 = new TCanvas("c1", "c1", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );

   //----- Setting Styles  --------------------------------------------------------
   one_sig->SetFillColor( kGreen +1 );
   one_sig->SetLineColor( kGreen +1 );
   one_sig->SetLineWidth(0);
   one_sig->SetFillStyle(1001);
   two_sig->SetFillColor( kOrange );
   two_sig->SetLineColor( kOrange );
   two_sig->SetLineWidth(0);
   two_sig->SetFillStyle(3001);

   exp->SetLineColor(kBlack);
   exp->SetLineWidth(2);
   exp->SetLineStyle(2);

   obs->SetLineColor(kBlack);
   obs->SetLineWidth(2);
   obs->SetLineStyle(1);

   theory->SetLineColor(kBlue);
   theory->SetLineWidth(2);
   theory->SetLineStyle(2);

   TMultiGraph* mg  = new TMultiGraph();
   mg->Add(two_sig);
   mg->Add(one_sig);
   mg->Add(exp);
   mg->Add(obs);
   mg->Add(theory);


   Parameter explim = GetInterSect(theory,two_sig) ;
   Parameter obslim = GetInterSect(theory,obs)     ;

   //----- Plotting  --------------------------------------------------------------
   mg->Draw("AL3");
   plt::SetAxis( mg );
   mg->GetXaxis()->SetTitle( "t* Mass (GeV/c^{2})" );    // MUST Be after draw!!
   mg->GetYaxis()->SetTitle( "#sigma(pp#rightarrow t*#bar{t}*) (pb)" ); // https://root.cern.ch/root/roottalk/roottalk09/0078.html
   mg->GetXaxis()->SetLimits( x_min-50 , x_max+50 );
   mg->SetMaximum(y_max*100.);
   mg->SetMinimum(y_min*0.3);


   const double legend_x_min = 0.55;
   const double legend_y_min = 0.65;
   TLegend* l  = plt::NewLegend(legend_x_min,legend_y_min);
   l->AddEntry( obs     , "95%% CL Limit (Obs.)" , "l" );
   l->AddEntry( exp     , "Medium expected limit", "l" );
   l->AddEntry( one_sig , "68%% expected"        , "f" );
   l->AddEntry( two_sig , "95%% expected"        , "f" );
   l->AddEntry( theory  , "Theory"               , "l" );
   l->Draw();


   plt::DrawCMSLabel();
   if( boost::contains( limit_namer.GetChannel(), "2016" ) ){
      plt::DrawLuminosity( cfg.GetStaticDouble("Total Luminosity 2016") );
   } else {
      plt::DrawLuminosity( cfg.GetStaticDouble("Total Luminosity 2015") );
   }

   TLatex tl;
   tl.SetNDC(kTRUE);
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_LEFT );
   tl.DrawLatex( PLOT_X_MIN+0.02, PLOT_Y_MAX-0.02, limit_namer.GetChannelEXT("Root Name").c_str() );
   tl.DrawLatex( PLOT_X_MIN+0.02, PLOT_Y_MAX-0.09, (limit_namer.GetExtName("fitmethod","Full Name")+", "+limit_namer.GetExtName("fitfunc","Full Name")).c_str() );

   tl.SetTextAlign( TOP_LEFT );
   char buffer[1024];
   sprintf( buffer , "Expected Lim (95%% CL.) = %.1lf^{+%.1lf}_{-%.1lf} GeV/c^{2}",
      explim.CentralValue(), explim.AbsUpperError(), explim.AbsLowerError() );
   tl.DrawLatex( 0.42 , legend_y_min - 0.02 , buffer );
   sprintf( buffer , "Observed Lim (95%% CL.) = %.1lf GeV/c^{2}",
      obslim.CentralValue() );
   tl.DrawLatex( 0.42 , legend_y_min - 0.08 , buffer );



   //----- Saving and cleaning up  ------------------------------------------------
   c1->SetLogy(kTRUE);
   c1->SaveAs( limit_namer.PlotFileName("limit").c_str() );
   delete one_sig;
   delete two_sig;
   delete obs;
   delete exp;
   delete theory;
   delete mg;
   delete l;
   delete c1;
}


//------------------------------------------------------------------------------
//   Helper function implementations
//------------------------------------------------------------------------------
void FillXSectionMap()
{
   FILE* xsec_file  = fopen(
      (limit_namer.SubPackageDir()+"data/excitedtoppair13TeV.dat").c_str(),"r");
   char*  line_buf = NULL ;
   size_t line_len = 0 ;
   double energy, mass, xsec_value ;
   while( getline(&line_buf, &line_len, xsec_file ) != -1) {
        sscanf( line_buf , "%lf%lf%lf", &energy, &mass, &xsec_value );
        pred_cross_section[mass] = xsec_value * 1000.;
   }
   if( line_buf ){
      free(line_buf);
   }
   fclose( xsec_file );
}

void getintersect(
   const double p1x, const double p1y,
   const double p2x, const double p2y,
   const double p3x, const double p3y,
   const double p4x, const double p4y,
   double& intx    , double& inty
);

Parameter GetInterSect( const TGraph* graph, const TGraph* errgraph )
{
   double y            = 0 ;
   double explimit     = 0 ;
   double explimit_up  = 0 ;
   double explimit_down= 0 ;
   for( int i = 0 ; i < graph->GetN()-1 ; ++i ){
      for( int j = 0 ; j < errgraph->GetN()-1 ; ++j ){
         const double graphmass        = graph->GetX()[i];
         const double graphmassnext    = graph->GetX()[i+1];
         const double errgraphmass     = errgraph->GetX()[j];
         const double errgraphmassnext = errgraph->GetX()[j+1];
         const double graphval         = graph->GetY()[i];
         const double graphvalnext     = graph->GetY()[i+1];
         const double errgraphval      = errgraph->GetY()[j];
         const double errgraphvalnext  = errgraph->GetY()[j+1];
         const double hierr            = errgraph->GetErrorYhigh(j) + errgraphval;
         const double hierrnext        = errgraph->GetErrorYhigh(j+1) + errgraphvalnext;
         const double loerr            = errgraphval + errgraph->GetErrorYlow(j);
         const double loerrnext        = errgraphvalnext + errgraph->GetErrorYlow(j+1);

         getintersect(
            graphmass       , graphval,
            graphmassnext   , graphvalnext,
            errgraphmass    , errgraphval,
            errgraphmassnext, errgraphvalnext,
            explimit        , y
         );
         getintersect(
            graphmass       , graphval,
            graphmassnext   , graphvalnext,
            errgraphmass    , errgraphval,
            hierr           , hierrnext,
            explimit_up     , y
         );
         getintersect(
            graphmass       , graphval,
            graphmassnext   , graphvalnext,
            errgraphmass    , errgraphval,
            loerr           , loerrnext,
            explimit_down   , y
         );
      }
   }
   return Parameter( explimit, explimit_up - explimit, explimit - explimit_down );
}

void getintersect(
   const double p1x, const double p1y,
   const double p2x, const double p2y,
   const double p3x, const double p3y,
   const double p4x, const double p4y,
   double& intx    , double& inty
){
   const double s1x = p2x - p1x;
   const double s1y = p2y - p1y;
   const double s2x = p4x - p3x;
   const double s2y = p4y - p3y;

   const double s = (-s1y * (p1x - p3x) + s1x * (p1y - p3y)) / (-s2x*s1y + s1x*s2y);
   const double t = ( s2x * (p1y - p3y) - s2y * (p1x - p3x)) / (-s2x*s1y + s1x*s2y);

   if (s >= 0 && s <= 1 && t >= 0 && t <= 1){
      intx = p1x + (t * s1x);
      inty = p1y + (t * s1y);
   } // else, leave untouched
}

/*******************************************************************************
 *
 *  Filename    : RooFit_SimFit_Val.cc
 *  Description : Validation of simfit method via signal injection
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <boost/format.hpp>

#include "TStyle.h"
#include "TFitResult.h"
#include "TGraphErrors.h"

using namespace std;

//------------------------------------------------------------------------------
//   Helper structures and functions
//------------------------------------------------------------------------------
struct val::PullResult{
   Parameter bkgpull;
   Parameter sigpull;
   Parameter p1pull;
   Parameter p2pull;
};

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void RunValidate( SampleRooFitMgr* data, vector<SampleRooFitMgr*>& siglist )
{
   TGraphErrors* bkgplot = new TGraphErrors( siglist.size() );
   TGraphErrors* sigplot = new TGraphErrors( siglist.size() );
   TGraphErrors* p1plot  = new TGraphErrors( siglist.size() );
   TGraphErrors* p2plot  = new TGraphErrors( siglist.size() );

   unsigned i = 0 ;
   for( auto sig : siglist ){
      auto result = val::RunSingle( data, sig );
      bkgplot->SetPoint( i , GetInt(sig->Name()) , result.bkgpull.CentralValue() );
      bkgplot->SetPointError( i , 0.                  , result.bkgpull.AbsAvgError()  );
      sigplot->SetPoint( i , GetInt(sig->Name()) , result.sigpull.CentralValue() );
      sigplot->SetPointError( i , 0.                  , result.sigpull.AbsAvgError()  );
      p1plot->SetPoint(  i , GetInt(sig->Name()) , result.p1pull.CentralValue()  );
      p1plot->SetPointError(  i , 0.                  , result.p1pull.AbsAvgError()   );
      p2plot->SetPoint(  i , GetInt(sig->Name()) , result.p2pull.CentralValue()  );
      p2plot->SetPointError(  i , 0.                  , result.p2pull.AbsAvgError()   );
      ++i;
   }

   val::PlotMassCompare( bkgplot , "bkg" );
   val::PlotMassCompare( sigplot , "sig" );
   val::PlotMassCompare( p1plot , "param1" );
   val::PlotMassCompare( p2plot , "param2" );
}

val::PullResult val::RunSingle( SampleRooFitMgr* data, SampleRooFitMgr* sigmgr )
{
   smft::FitPDFs( data, sigmgr ); // First fit to original dataset
   var_mgr.SetConstant(kFALSE);
   const string pdf_alias_full = data->MakePdfAlias( "bg" + sigmgr->Name() );
   RooAbsPdf*  model  = data->GetPdfFromAlias( sigmgr->Name() );
   RooRealVar* bkg    = var_mgr.FindByName( "nb" + sigmgr->Name() );
   RooRealVar* sig    = var_mgr.FindByName( "ns" + sigmgr->Name() );
   RooRealVar* p1     = var_mgr.FindByName( "a" + pdf_alias_full );
   RooRealVar* p2     = var_mgr.FindByName( "b" + pdf_alias_full );

   if( limit_namer.GetMap().count("relmag") ){
      *sig = sigmgr->ExpectedYield().CentralValue() * limit_namer.GetMap()["relmag"].as<double>();
   } else if (limit_namer.GetMap().count("absmag")){
      *sig = limit_namer.GetMap()["absmag"].as<double>() ;
   }


   RooMCStudy* mcstudy = new RooMCStudy(
      *model,
      SampleRooFitMgr::x(),
      RooFit::Binned(kFALSE),
      RooFit::Silence(),
      RooFit::Extended(),
		RooFit::FitOptions( // Suppressing as much output as possible
         RooFit::Warnings(kFALSE),
         RooFit::Verbose(kFALSE),
         RooFit::PrintLevel(-1),
         RooFit::PrintEvalErrors(-1)
      )
   );
   const int run = limit_namer.GetMap()["run"].as<int>();
   mcstudy->generateAndFit(run);

   val::PullResult ans;
   ans.bkgpull= val::DrawPull( mcstudy, bkg, sigmgr, "bkg"    );
   ans.sigpull= val::DrawPull( mcstudy, sig, sigmgr, "sig"    );
   ans.p1pull = val::DrawPull( mcstudy, p1 , sigmgr, "param1" );
   ans.p2pull = val::DrawPull( mcstudy, p2 , sigmgr, "param2" );
   return ans;
}

//------------------------------------------------------------------------------
//   Helper functions for Plotting
//------------------------------------------------------------------------------
Parameter val::DrawPull(
   RooMCStudy* study ,
   RooRealVar* var,
   SampleRooFitMgr* sigmgr,
   const string& name
)
{
   TCanvas* c = new TCanvas("c","c",DEFAULT_CANVAS_WIDTH,DEFAULT_CANVAS_HEIGHT);
   RooPlot* pullplot = study->plotPull(
      *var,
      RooFit::FrameRange(-5,5)
   );

   SetFrame( pullplot , AXIS_TITLE_FONT_SIZE);

   TGraph* gaussplot = (TGraph*)pullplot->getObject( pullplot->numItems() - 1  );
   auto fitr  = gaussplot->Fit("gaus","QSEM");
   pullplot->Draw();
   gStyle->SetOptStat(0) ;

   boost::format xform("pull_{%1%}");
   string x_title = str(xform % name );
   pullplot->SetTitle( "" );
   pullplot->GetXaxis()->SetTitle( x_title.c_str() );
   plt::DrawCMSLabel(SIMULATION);
   plt::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );

   boost::format sigtype("signal mass = %1% GeV");
   boost::format meanfmt("mean   = %.5f #pm %.5f");
   boost::format sigmafmt("#sigma = %.5f #pm %.5f");
   boost::format injfmt("inject signal events = %.2f");
   double injstrgh = 0;
   if( limit_namer.GetMap().count("absmag") ){
      injstrgh = limit_namer.GetMap()["absmag"].as<double>();
   } else if( limit_namer.GetMap().count("relmag") ){
      injstrgh = sigmgr->ExpectedYield().CentralValue() * limit_namer.GetMap()["relmag"].as<double>();
   }
   const int mass = GetInt( sigmgr->Name() );
   TLatex tl;
   tl.SetNDC(kTRUE);
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.02, limit_namer.GetChannelEXT("Root Name").c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.08, limit_namer.GetExtName("fitfunc","Root Name").c_str() );
   tl.DrawLatex( PLOT_Y_MAX-0.02, PLOT_Y_MAX-0.16, str( injfmt % injstrgh ).c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.22, str(sigtype % mass ).c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.28, str(meanfmt % fitr->Value(1) % fitr->Value(1) ).c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.34, str(sigmafmt% fitr->Value(2) % fitr->Value(2) ).c_str() );

   boost::format sectag("%1%_tstarM%2%_%3%%4%");
   string sec;
   if( limit_namer.GetMap().count("absmag") ){
      sec = str( sectag % name % mass % "abs" % limit_namer.GetMap()["absmag"].as<double>() );
   } else if( limit_namer.GetMap().count("relmag") ){
      sec = str( sectag % name % mass % "rel" % limit_namer.GetMap()["relmag"].as<double>() );
   }
   c->SaveAs(limit_namer.PlotFileName("pull",sec).c_str());
   delete c;

   return Parameter( fitr->Value(1), fitr->Value(2) , fitr->Value(2) );
}


void val::PlotMassCompare( TGraph* graph, const string& name )
{
   TCanvas* c = new TCanvas("c","c",DEFAULT_CANVAS_WIDTH,DEFAULT_CANVAS_HEIGHT);
   graph->Draw("AP");
   graph->SetTitle("");
   graph->GetXaxis()->SetTitle("signal mass (GeV)");
   graph->GetYaxis()->SetTitle( ("pull_{"+name+"}").c_str() );
   graph->SetMarkerStyle(21);
   graph->SetMaximum(2.5);
   graph->SetMinimum(-2.5);
   plt::DrawCMSLabel(SIMULATION);
   plt::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );

   const double xmin = graph->GetXaxis()->GetXmin();
   const double xmax = graph->GetXaxis()->GetXmax();
   TLine* z  = new TLine( xmin , 0 , xmax, 0 );
   TLine* hi = new TLine( xmin , 1 , xmax, 1 );
   TLine* lo = new TLine( xmin ,-1 , xmax,-1 );
   z ->SetLineColor( kRed); z ->SetLineWidth(3); z ->Draw();
   hi->SetLineColor(kBlue); hi->SetLineWidth(2); hi->Draw();
   lo->SetLineColor(kBlue); lo->SetLineWidth(2); lo->Draw();

   TLatex tl;
   boost::format injfmt("inject signal strength %s %.2f");
   boost::format sectag("%1%_%2%%3%");

   double injstrgh;
   string tlstr;
   string sec;
   if( limit_namer.GetMap().count("absmag") ){
      injstrgh = limit_namer.GetMap()["absmag"].as<double>();
      tlstr    = str( injfmt % "=" % injstrgh );
      sec      = str( sectag % name % "abs" % injstrgh );
   } else if( limit_namer.GetMap().count("relmag") ){
      injstrgh = limit_namer.GetMap()["relmag"].as<double>();
      tlstr    = str( injfmt % "#times" % injstrgh );
      sec = str( sectag % name % "rel" % injstrgh );
   }

   tl.SetNDC(kTRUE);
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_RIGHT );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.02, limit_namer.GetChannelEXT("Root Name").c_str() );
   tl.DrawLatex( PLOT_X_MAX-0.02, PLOT_Y_MAX-0.08, limit_namer.GetExtName("fitfunc","Root Name").c_str() );
   tl.DrawLatex( PLOT_Y_MAX-0.02, PLOT_Y_MAX-0.16, tlstr.c_str() );

   c->SaveAs(limit_namer.PlotFileName("pullvmass",sec).c_str() );

   delete c;
   delete z;
   delete hi;
   delete lo;
}

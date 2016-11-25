/*******************************************************************************
*
*  Filename    : MakeErrHist.cc
*  Description : Implementatin of functions defined in include/MakeErrHist.hpp
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/MakeErrHist.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/NameParse.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeHist.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"

#include <boost/format.hpp>
#include <boost/range/adaptor/reversed.hpp>
using namespace std;

void
MakeFullComparePlot(
   SampleErrHistMgr*          datamgr,
   vector<SampleErrHistMgr*>& background,
   SampleErrHistMgr*          signalmgr,
   const string&              label
   )
{
   SetBkgColor( background );

   for( const auto& histname : histnamelist ){

      // Declaring Histograms, for construction look below.
      THStack* stack = MakeBkgStack( background, histname );
      TH1D* bkgerror = MakeBkgError( background, histname );
      TH1D* datahist = (TH1D*)datamgr->Hist( histname )->Clone();
      TH1D* bkgrel   = MakeBkgRelHist( bkgerror );
      TH1D* datarel  = MakeDataRelHist( datahist, bkgerror );
      TH1D* sighist  = (TH1D*)signalmgr->Hist( histname )->Clone();

      // Scaling signal plot for clarity
      const unsigned datanum = datahist->Integral();
      const double signum    = sighist->Integral();
      const double sigscale  = datanum / signum / 2.;
      if( sighist->Integral() < datahist->Integral() /4.0 ){
         sighist->Scale( sigscale );
      }

      // Legend settings
      TLegend* l = plt::NewLegend( 0.6, 0.5 );
      l->AddEntry( datahist, datamgr->RootName().c_str(), "lp" );

      for( const auto& entry : background ){
         l->AddEntry( entry->Hist( histname ), entry->RootName().c_str(), "f" );
      }

      l->AddEntry( bkgerror, "Bkg. error (1#sigma)",                                                              "fl" );
      l->AddEntry( sighist,  boost::str( boost::format( "%s (#times%3.1lf)" )%signalmgr->RootName()%sigscale ).c_str(), "fl" );

      MakePlot(
         stack,
         bkgerror,
         datahist,
         sighist,
         bkgrel,
         datarel,
         l,
         "fullcomp",
         {histname, label,compnamer.InputStr("era")}
         );
      delete l;
      delete stack;
      delete bkgerror;
      delete datahist;
      delete bkgrel;
      delete datarel;
      delete sighist;
   }

}

/******************************************************************************/

extern void
Normalize(
   SampleErrHistMgr*               data,
   std::vector<SampleErrHistMgr*>& bg
   )
{
   const double datayield = data->ExpectedYield();
   double bgyield         = 0;

   for( const auto& sample : bg ){
      bgyield += sample->ExpectedYield();
   }

   const double scale = datayield / bgyield;

   for( auto& sample : bg ){
      sample->Scale( scale );
   }
}

/******************************************************************************/

TH1D*
MakeSumHistogram(
   const vector<SampleErrHistMgr*>& samplelist,
   const string&                    histname
   )
{
   const TH1D* temphist = samplelist.front()->Hist( histname );
   const unsigned bins  = temphist->GetXaxis()->GetNbins();
   const double xmin    = temphist->GetXaxis()->GetXmin();
   const double xmax    = temphist->GetXaxis()->GetXmax();

   const string newname = samplelist.front()->Name() + histname + "sum";
   const string title   = ";" + string( temphist->GetXaxis()->GetTitle() ) + ";" + string( temphist->GetYaxis()->GetTitle() );
   TH1D* sumhist        = new TH1D( newname.c_str(), title.c_str(), bins, xmin, xmax );

   for( const auto& sample : samplelist ){
      sumhist->Add( sample->Hist( histname ) );
   }

   sumhist->SetStats( 0 );
   return sumhist;
}


/******************************************************************************/

Parameter
ExpectedYield( const vector<SampleErrHistMgr*>& samplelist )
{
   Parameter ans;

   for( const auto& group : samplelist ){
      for( const auto& sample : group->SampleList() ){
         ans += sample.CrossSection() * sample.SelectionEfficiency();
      }
   }

   return ans;
}

/******************************************************************************/

void
SetBkgColor( vector<SampleErrHistMgr*>& bkg )
{
   bkg[0]->SetColor( TColor::GetColor( "#FFCC88" ) );
   bkg[1]->SetColor( TColor::GetColor( "#996600" ) );
   bkg[2]->SetColor( TColor::GetColor( "#FF3333" ) );
   bkg[3]->SetColor( TColor::GetColor( "#33EEEE" ) );
   bkg[4]->SetColor( TColor::GetColor( "#0066EE" ) );
}

/******************************************************************************/

THStack*
MakeBkgStack(
   const vector<SampleErrHistMgr*>& samplelist,
   const string&                    histname
   )
{
   THStack* stack = new THStack( ( histname+"bkgstack" ).c_str(), "" );

   for( const auto& sample : boost::adaptors::reverse( samplelist ) ){
      stack->Add( sample->Hist( histname ), "HIST" );
   }

   return stack;
}

/******************************************************************************/

TH1D*
MakeBkgError(
   const vector<SampleErrHistMgr*>& samplelist,
   const string&                    histname
   )
{
   TH1D* central = MakeSumHistogram( samplelist, histname );
   vector<pair<TH1D*, TH1D*> > errorhistlist;

   for( const auto& err : histerrlist ){
      errorhistlist.emplace_back(
         MakeSumHistogram( samplelist, histname + err.tag + "up" ),
         MakeSumHistogram( samplelist, histname + err.tag + "down" )
         );
   }

   const Parameter expyield = ExpectedYield( samplelist );
   const Parameter normerr  = Parameter(
      1,
      expyield.RelUpperError(),
      expyield.RelLowerError()
      );

   for( int i = 1; i <= central->GetSize(); ++i ){
      const double bincont = central->GetBinContent( i );
      const double binerr  = central->GetBinError( i );
      Parameter binerror(
         1,
         binerr / bincont,
         binerr / bincont
         );

      for( const auto pair : errorhistlist ){
         const double upbincont   = pair.first->GetBinContent( i );
         const double downbincont = pair.second->GetBinContent( i );
         const double upabserr    = std::max( 0.0, upbincont-bincont );
         const double downabserr  = std::max( 0.0, bincont-downbincont );
         const Parameter errorparm( 1, upabserr/bincont, downabserr/bincont );
         binerror *= errorparm;
      }

      // Additional errors
      binerror *= normerr;// cross section and selection eff
      binerror *= Parameter( 1, 0.046, 0.046 );// lumierror
      binerror *= Parameter( 1, 0.03, 0.03 );// leptonic errors

      if( bincont != 0 ){
         central->SetBinError( i, bincont * binerror.RelAvgError() );
      }
   }


   for( auto& pair : errorhistlist ){
      delete pair.first;
      delete pair.second;
   }

   return central;
}

/******************************************************************************/

void
PlotErrCompare(
   const string&                    listname,
   const vector<SampleErrHistMgr*>& samplelist,
   const string&                    histname,
   const ErrorSource&               err
   )
{
   TH1D* central = MakeSumHistogram( samplelist, histname );
   TH1D* errup   = MakeSumHistogram( samplelist, histname + err.tag + "up" );
   TH1D* errdown = MakeSumHistogram( samplelist, histname + err.tag + "down" );


   // Making duplicate objects
   TH1D* uprel   = (TH1D*)errup->Clone();
   TH1D* downrel = (TH1D*)errdown->Clone();
   uprel->Divide( central );
   downrel->Divide( central );

   const double xmin = central->GetXaxis()->GetXmin();
   const double xmax = central->GetXaxis()->GetXmax();

   // Plotting
   TCanvas* c = plt::NewCanvas();

   // TOPPAD
   TPad* pad1 = plt::NewTopPad();
   pad1->Draw();
   pad1->cd();
   central->Draw( "axis" );
   errup->Draw( PS_HIST PS_SAME );
   errdown->Draw( PS_HIST PS_SAME );
   central->Draw( PS_HIST PS_SAME );
   c->cd();


   TPad* pad2      = plt::NewBottomPad();
   TLine* line     = new TLine( xmin, 1, xmax, 1 );
   TLine* line_top = new TLine( xmin, 1.5, xmax, 1.5 );
   TLine* line_bot = new TLine( xmin, 0.5, xmax, 0.5 );
   pad2->Draw();
   pad2->cd();

   uprel->Draw( "axis" );
   uprel->Draw( "LE SAME" );
   downrel->Draw( "LE SAME" );
   line->Draw( "same" );
   line_top->Draw( "SAME" );
   line_bot->Draw( "SAME" );
   c->cd();


   // Drawing legend
   const double legxmin = 0.5;
   const double legymin = 0.7 ;
   TLegend* tl        = plt::NewLegend( legxmin, legymin );
   const string label = ( err.label != "" ) ? err.label : "1#sigma";
   tl->AddEntry( central, "Central Value",                               "l" );
   tl->AddEntry( errup,   ( err.rootname + "(+" + label + ")" ).c_str(), "l" );
   tl->AddEntry( errdown, ( err.rootname + "(-" + label + ")" ).c_str(), "l" );

   tl->Draw( "same" );

   // Common styling
   central->SetLineColor( kBlack );
   errup->SetLineColor( kRed );
   errdown->SetLineColor( kBlue );
   uprel->SetLineColor( kRed );
   downrel->SetLineColor( kBlue );

   // accessory styling
   line->SetLineColor( kBlack );
   line->SetLineStyle( 1 );
   line->SetLineWidth( 2 );
   line_top->SetLineColor( kBlack );
   line_bot->SetLineColor( kBlack );
   line_top->SetLineStyle( 3 );
   line_bot->SetLineStyle( 3 );

   plt::SetTopPlotAxis( central );
   plt::SetBottomPlotAxis( uprel );

   // Setting plot range
   const double ymax = plt::GetYmax( {central, errup, errdown} );
   central->SetMaximum( ymax * 1.5 );
   uprel->SetMaximum( 2.2 );
   uprel->SetMinimum( -0.2 );
   uprel->GetYaxis()->SetTitle( "#frac{up,down}{Norm}" );


   TPaveText* tb = plt::NewTextBox(
      PLOT_X_MIN + 0.05 ,
      PLOT_Y_MAX - (TEXT_FONT_SIZE*2.0)/(DEFAULT_CANVAS_HEIGHT),
      PLOT_X_MIN + 0.40,
      PLOT_Y_MAX - 0.025
   );
   tb->AddText( compnamer.GetChannelEXT( "Root Name" ).c_str() );
   tb->Draw();

   TLatex ltx;
   ltx.SetNDC( kTRUE );
   ltx.SetTextFont( FONT_TYPE );
   ltx.SetTextSize( AXIS_TITLE_FONT_SIZE );
   ltx.SetTextAlign( TOP_RIGHT );
   try {
      ltx.DrawLatex( PLOT_X_MAX-0.04, legymin-0.08, str( boost::format( "t* %dGeV" )%GetInt( listname ) ).c_str() );
   } catch( std::exception ){
      ltx.DrawLatex( PLOT_X_MAX-0.04, legymin-0.08, listname.c_str() );
   }

   // cleaning up
   if( listname.find( "Data" ) != string::npos ){
      plt::DrawCMSLabel( PRELIMINARY );
   } else {
      plt::DrawCMSLabel( SIMULATION );
   }
   plt::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );

   plt::SaveToPDF( c, compnamer.PlotFileName( "errcomp", {listname, histname, err.tag} ) );
   plt::SaveToROOT(
      c,
      compnamer.PlotRootFile(),
      compnamer.MakeFileName( "", "errcomp", {listname, histname, err.tag} )
      );

   delete tl;
   delete uprel;
   delete downrel;
   delete central;
   delete errup;
   delete errdown;
   delete c;
}

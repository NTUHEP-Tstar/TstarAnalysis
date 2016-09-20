/*******************************************************************************
*
*  Filename    : KinematicCompare
*  Description : Plotting function for Implementation
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeHist.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"

#include <boost/format.hpp>
#include <boost/range/adaptor/reversed.hpp>

using namespace std;


/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
MakeComparePlot(
   SampleHistMgr*          datamgr,
   vector<SampleHistMgr*>& background,
   SampleHistMgr*          signalmgr,
   const string            label
   )
{
   const double total_lumi = mgr::SampleMgr::TotalLuminosity();

   SetBkgColor( background );

   for( const auto& histname : datamgr->AvailableHistList() ){
      const Parameter bkgyield = GetBkgYield( background );

      // Declaring Histograms, for construction look below.
      THStack* stack = MakeBkgStack( background, histname );
      TH1D* bkgerror = MakeBkgError( background, histname );
      TH1D* datahist = (TH1D*)datamgr->Hist( histname )->Clone();
      TH1D* bkgrel   = MakeBkgRelHist( bkgerror, bkgyield );
      TH1D* datarel  = MakeDataRelHist( datahist, bkgerror );
      TH1D* sighist  = (TH1D*)signalmgr->Hist( histname )->Clone();

      // Scaling signal plot for clarity
      const unsigned datanum = datamgr->EventsInFile();
      const double signum    = signalmgr->ExpectedYield();
      const double sigscale  = datanum / signum / 2.;
      if( sighist->Integral() < datahist->Integral() /4.0 ){
         sighist->Scale( sigscale );
      }

      // Legend settings
      TLegend* l = plt::NewLegend( 0.62, 0.3 );
      l->AddEntry( datahist, datamgr->RootName().c_str(), "lp" );

      for( const auto& entry : background ){
         l->AddEntry( entry->Hist( histname ), entry->RootName().c_str(), "f" );
      }

      l->AddEntry( bkgerror, "Bkg. error",                                                                        "fl" );
      l->AddEntry( sighist,  boost::str( boost::format( "%s (x%.1lf)" )%signalmgr->RootName()%sigscale ).c_str(), "fl" );

      const double xmin = datahist->GetXaxis()->GetXmin();
      const double xmax = datahist->GetXaxis()->GetXmax();

      // Object plotting
      TCanvas* c = new TCanvas( "c", "c", DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT );

      // Drawing the top canvas
      TPad* pad1 = plt::NewTopPad();
      pad1->Draw();
      pad1->cd();
      stack->Draw( "HIST" );
      datahist->Draw( "same LPE1" );
      bkgerror->Draw( "same E2" );
      sighist->Draw( "same HIST" );
      l->Draw( "same" );
      c->cd();

      // Drawing bottom canvas
      TPad* pad2      = plt::NewBottomPad();
      TLine* line     = new TLine( xmin, 1, xmax, 1 );
      TLine* line_top = new TLine( xmin, 2, xmax, 2 );
      TLine* line_bot = new TLine( xmin, 0, xmax, 0 );
      pad2->Draw();
      pad2->cd();
      datarel->Draw( "axis" );
      datarel->Draw( "same LPE1" );
      bkgrel->Draw( "same E2" );
      line->Draw( "same" );
      line_top->Draw( "same" );
      line_bot->Draw( "same" );
      c->cd();

      // Plot styling ( see Common/interface/PlotStyle.hpp )
      tstar::SetErrorStyle( bkgerror );
      tstar::SetErrorStyle( bkgrel );
      tstar::SetDataStyle( datahist );
      tstar::SetDataStyle( datarel );
      tstar::SetSignalStyle( sighist );

      // Font and title settings
      plt::SetAxis( stack );
      plt::DisableXAxis( stack );
      stack->GetYaxis()->SetTitle( datahist->GetYaxis()->GetTitle() );

      plt::SetAxis( datarel );
      datarel->GetXaxis()->SetTitleOffset( 5.5 );
      datarel->GetXaxis()->SetTitle( sighist->GetXaxis()->GetTitle() );
      datarel->GetYaxis()->SetTitle( "#frac{Data}{MC}" );
      datarel->SetMaximum( 2.2 );
      datarel->SetMinimum( -0.2 );
      line->SetLineColor( kRed );
      line->SetLineStyle( 1 );
      line_top->SetLineColor( kBlack );
      line_bot->SetLineColor( kBlack );
      line_top->SetLineStyle( 3 );
      line_bot->SetLineStyle( 3 );

      // Writing captiosn
      plt::DrawCMSLabel();
      plt::DrawLuminosity( total_lumi );
      TPaveText* tb = plt::NewTextBox( 0.12, 0.88, 0.30, 0.94 );
      tb->AddText( compare_namer.GetChannelEXT( "Root Name" ).c_str() );
      tb->Draw();

      // setting ranges and saving plots
      const double ymax = plt::GetYmax( {bkgerror, datahist, sighist} );
      stack->SetMaximum( ymax * 1.2 );
      c->SaveAs( compare_namer.PlotFileName( histname, {label} ).c_str() );
      pad1->SetLogy();
      stack->SetMaximum( ymax * 30 );
      stack->SetMinimum( 0.3 );
      c->SaveAs( compare_namer.PlotFileName( histname, {label, "log"} ).c_str() );

      delete pad1;
      delete pad2;
      delete c;
      delete l;
      delete stack;
      delete bkgerror;
      delete datahist;
      delete bkgrel;
      delete datarel;
      delete sighist;
      delete line;
      delete line_top;
      delete line_bot;
      delete tb;
   }

}


void
Normalize( SampleHistMgr* data, vector<SampleHistMgr*>& bgstack )
{
   for( const auto& histname : data->AvailableHistList() ){
      double datamag = data->Hist( histname )->Integral();
      double bgmag   = 0;

      for( const auto mgr : bgstack ){
         bgmag += mgr->Hist( histname )->Integral();
      }

      double scale = datamag/bgmag;

      for( auto& mgr : bgstack ){
         mgr->Hist( histname )->Scale( scale );
      }
   }
}



/*******************************************************************************
*   Plotting Helper functions
*******************************************************************************/
Parameter
GetBkgYield( const vector<SampleHistMgr*>& bkg )
{
   Parameter ans;

   for( const auto& sample : bkg ){
      ans += sample->ExpectedYield();
   }

   return ans;
}

/******************************************************************************/

void
SetBkgColor( vector<SampleHistMgr*>& bkg )
{
   bkg[0]->SetColor( TColor::GetColor( "#FFCC88" ) );
   bkg[1]->SetColor( TColor::GetColor( "#996600" ) );
   bkg[2]->SetColor( TColor::GetColor( "#FF3333" ) );
   bkg[3]->SetColor( TColor::GetColor( "#33EEEE" ) );
   bkg[4]->SetColor( TColor::GetColor( "#0066EE" ) );
}

/******************************************************************************/

THStack*
MakeBkgStack( const vector<SampleHistMgr*>& bkg, const string& histname )
{
   THStack* stack = new THStack( ( histname+"stack" ).c_str(), "" );

   for( const auto& sample : boost::adaptors::reverse( bkg ) ){
      stack->Add( sample->Hist( histname ), "HIST" );
   }

   return stack;
}

/******************************************************************************/

TH1D*
MakeBkgError( const vector<SampleHistMgr*>& bkg, const string& histname )
{
   const unsigned bins = bkg.front()->Hist( histname )->GetXaxis()->GetNbins();
   const double xmin   = bkg.front()->Hist( histname )->GetXaxis()->GetXmin();
   const double xmax   = bkg.front()->Hist( histname )->GetXaxis()->GetXmax();

   TH1D* error = new TH1D( ( histname+"error" ).c_str(), "", bins, xmin, xmax );

   for( const auto& sample : bkg ){
      error->Add( sample->Hist( histname ) );
   }

   error->SetStats( 0 );
   return error;
}

/******************************************************************************/

TH1D*
MakeBkgRelHist( TH1D* bkghist, const Parameter& scale )
{
   const Parameter normscale(
      1,
      scale.RelAvgError(),
      scale.RelAvgError()
      );
   TH1D* bkgrelerr = (TH1D*)bkghist->Clone( "bkgrelhist" );

   for( int i = 0; i < bkghist->GetSize(); ++i ){
      const double bincont = bkghist->GetBinContent( i );
      const double binerr  = bkghist->GetBinError( i );
      const Parameter bincontpar( bincont, binerr, binerr );
      const double newBinError = ( bincontpar*normscale ).AbsAvgError();

      // scaling original error
      bkghist->SetBinError( i, newBinError );

      // Setting new historgram
      bkgrelerr->SetBinContent( i, 1.0 );
      if(  bincont != 0 ){
         bkgrelerr->SetBinError( i, newBinError/bincont );
      } else {
         bkgrelerr->SetBinError( i, 0.0 );
      }
   }

   bkgrelerr->SetStats( 0 );
   return bkgrelerr;
}

/******************************************************************************/

TH1D*
MakeDataRelHist( const TH1D* data, const TH1D* bkg )
{
   TH1D* datarelerr = (TH1D*)data->Clone( "datarelhist" );
   datarelerr->Divide( bkg );

   for( int i = 0; i < data->GetSize(); ++i ){
      const double databinerr = data->GetBinError( i );
      const double bkgbincont = bkg->GetBinContent( i );
      if( bkgbincont != 0 ){
         datarelerr->SetBinError( i, databinerr/bkgbincont );
      } else {
         datarelerr->SetBinError( i, 0.0 );
      }
   }

   datarelerr->SetStats( 0 );
   return datarelerr;
}

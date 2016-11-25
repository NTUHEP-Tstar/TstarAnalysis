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
   SetBkgColor( background );

   for( const auto& histname : datamgr->AvailableHistList() ){
      // Declaring Histograms, for construction look below.
      THStack* stack = MakeBkgStack( background, histname );
      TH1D* bkgerror = MakeBkgError( background, histname );
      TH1D* datahist = (TH1D*)datamgr->Hist( histname )->Clone();
      TH1D* bkgrel   = MakeBkgRelHist( bkgerror );
      TH1D* datarel  = MakeDataRelHist( datahist, bkgerror );
      TH1D* sighist  = (TH1D*)signalmgr->Hist( histname )->Clone();

      // Scaling signal plot for better clarity
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

      l->AddEntry( bkgerror, "Bkg. stat. error",                                                                  "fl" );
      l->AddEntry( sighist,  boost::str( boost::format( "%s (x%3.1lf)" )%signalmgr->RootName()%sigscale ).c_str(), "fl" );

      MakePlot(
         stack,
         bkgerror,
         datahist,
         sighist,
         bkgrel,
         datarel,
         l,
         "quickcomp",
         {histname, label,compnamer.InputStr("era")}
         );

      delete stack;
      delete bkgerror;
      delete bkgrel;
      delete datahist;
      delete datarel;
      delete sighist;
      delete l;
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
      stack->Add( sample->Hist( histname ), PS_HIST );
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
MakeBkgRelHist( const TH1D* bkghist )
{
   TH1D* bkgrelerr = (TH1D*)bkghist->Clone( "bkgrelhist" );

   for( int i = 1; i <= bkghist->GetSize(); ++i ){
      const double bincont = bkghist->GetBinContent( i );
      const double binerr  = bkghist->GetBinError( i );

      // Setting new historgram
      bkgrelerr->SetBinContent( i, 1.0 );
      if(  bincont != 0 ){
         bkgrelerr->SetBinError( i, binerr/bincont );
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

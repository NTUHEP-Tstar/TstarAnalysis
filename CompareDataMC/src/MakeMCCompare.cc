/*******************************************************************************
*
*  Filename    : MakeMCCompare.cc
*  Description : Implementaion of plotting functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/MakeMCCompare.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"

using namespace std;

static Color_t colorseq[] ={
  kBlue,
  kRed,
  kGreen,
  kBlack,
  kCyan
};

void MakeMCCompare( vector<SampleErrHistMgr*>& samplelist )
{
  for( const auto& histname : histnamelist ){
    TCanvas* c = mgr::NewCanvas();

    // Getting histogram clones
    vector<TH1*> histlist;
    for( size_t i = 0 ; i < samplelist.size() ; ++i  ){
      const auto& sample = samplelist.at(i);
      TH1* hist = (TH1*)(sample->Hist(histname)->Clone( sample->RootName().c_str() ));

      // Basic Initializing
      hist->SetLineColor( colorseq[i] );
      hist->SetLineWidth( 3 ) ;

      if( compnamer.CheckInput("normalize") ){
        hist->Scale( 1./hist->Integral() );
      }

      histlist.push_back(  hist );
    }

    // Additiona plotting settings
    for( const auto hist : histlist ){
      hist->Draw("hist same");
    }

    // Legend
    TLegend* legend = mgr::NewLegend( 0.5,0.6 );
    for( const auto hist : histlist ){
      const string entry = hist->GetName();
      legend->AddEntry( hist, entry.c_str() , "l" );
    }
    legend->Draw();



    // styling
    const double ymax = mgr::GetYmax( histlist );
    histlist.front()->SetMaximum( ymax*1.5 );
    mgr::DrawCMSLabel();
    mgr::SetSinglePad( c );
    mgr::SetAxis( histlist.front() );


    // Saving and cleaning up
    mgr::SaveToPDF( c, compnamer.PlotFileName("mccomp",histname) );

    for( auto hist : histlist ){
      delete hist;
    }
    delete legend;
    delete c;
  }
}

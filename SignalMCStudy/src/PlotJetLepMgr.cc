/*******************************************************************************
*
*  Filename    : PlotJetLepName.cc
*  Description : Control flow for generating plots
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/SignalMCStudy/interface/Common.hpp"
#include "TstarAnalysis/SignalMCStudy/interface/JetLepMgr.hpp"

using namespace std;

static Color_t colorseq[] = {
  kBlue,
  kRed,
  kGreen,
  kBlack,
  kCyan
};

void
MakeJetLepCompare( vector<JetLepMgr*>& list )
{
  for( const auto& histname : list.front()->AvailableHistList() ){
    TCanvas* c = mgr::NewCanvas();

    // Getting histogram clones
    vector<TH1*> histlist;

    for( size_t i = 0; i < list.size(); ++i  ){
      TH1* hist = (TH1*)( list[i]->Hist( histname )->Clone( list[i]->Name().c_str() ) );
      // Basic Initializing
      hist->SetLineColor( colorseq[i] );
      hist->SetLineWidth( 3 );

      histlist.push_back( hist );
    }

    // Additiona plotting settings
    for( const auto hist : histlist ){
      hist->Draw( "hist same" );
    }

    // Legend
    TLegend* legend = mgr::NewLegend( 0.5, 0.6 );

    for( const auto hist : histlist ){
      const string entry = "M_{t*}=" + string(hist->GetName()) + "GeV/c^{2}";
      legend->AddEntry( hist, entry.c_str(), "l" );
    }

    legend->Draw();


    // styling
    TH1* axishist = histlist.front() ;
    const double xmin       = axishist->GetXaxis()->GetXmin();
    const double xmax       = axishist->GetXaxis()->GetXmax();
    const double ymax       = mgr::GetYmax( histlist );
    const unsigned exponent = mgr::HistMgr::GetExponent( ymax*1.5 );
    const string xunit      = mgr::HistMgr::GetXUnit( (TH1D*)(axishist) );
    const double binsize    = (xmax-xmin)/axishist->GetNbinsX();
    const string newytitle  = mgr::HistMgr::MakeYTitle( binsize, xunit, exponent );
    axishist->SetMaximum( ymax*1.5 );
    axishist->GetYaxis()->SetTitle( newytitle.c_str() );
    mgr::DrawCMSLabel();
    mgr::SetSinglePad( c );
    mgr::SetAxis( axishist );


    // Saving and cleaning up
    mgr::SaveToPDF( c, PlotFileName( "jetlepcomp_"+histname ) );
    c->SetLogy( kTRUE );
    mgr::SaveToPDF( c, PlotFileName( "jetlepcomp_"+histname+"_log" ) );
    for( auto hist : histlist ){
      delete hist;
    }

    delete legend;
    delete c;
  }

}

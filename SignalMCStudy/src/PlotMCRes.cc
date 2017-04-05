/*******************************************************************************
*
*  Filename    : PlotMCRes.cc
*  Description : Plotting control flow
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/SignalMCStudy/interface/Common.hpp"
#include "TstarAnalysis/SignalMCStudy/interface/MCResMgr.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"

#include <boost/format.hpp>
#include <string>
using namespace std;

void
MakeMCResPlot( MCResMgr* mgr )
{
  boost::format legentry( "%s (r.m.s=%.1lf)" );
  {// w mass plots
    TCanvas* c = mgr::NewCanvas();

    TH1D* hist = mgr->Hist( "HadWMass" );
    hist->Draw( "HIST" );
    mgr::SetSinglePad( c );
    mgr::SetAxis( hist );
    hist->SetMaximum( mgr::GetYmax( hist ) * 1.2 );

    const string wentry = boost::str(legentry % "Hadronic W boson" % hist->GetRMS());
    TLegend* leg        = mgr::NewLegend( 0.5, 0.7 );
    leg->AddEntry( hist, wentry.c_str(), "l" );
    leg->Draw();


    mgr::DrawCMSLabel( SIMULATION );
    mgr::SaveToPDF( c, PlotFileName( "wres_" + mgr->Name() ) );

    delete c;
  }
  {// top resolution
    TCanvas* c  = mgr::NewCanvas();
    TH1D* hist1 = mgr->Hist( "HadTopMass" );
    TH1D* hist2 = mgr->Hist( "LepTopMass" );
    hist1->Draw( "HIST" );
    hist2->Draw( "HISTSAME" );
    mgr::SetSinglePad( c );
    mgr::SetAxis( hist1 );

    hist1->GetXaxis()->SetTitle( "Top mass (truth reconstruction) (GeV/c^{2})" );
    hist1->SetMaximum( mgr::GetYmax( hist1, hist2 ) *1.2 );

    hist1->SetLineColor( KBLUE );
    hist2->SetLineColor( KRED  );

    const string hadtop = boost::str(legentry % "Hadronic top" % hist1->GetRMS());
    const string leptop = boost::str(legentry % "Leptonic top" % hist2->GetRMS());
    TLegend* leg = mgr::NewLegend( 0.5, 0.7 );
    leg->AddEntry( hist1, hadtop.c_str(), "l" );
    leg->AddEntry( hist2, leptop.c_str(), "l" );
    leg->Draw();

    mgr::DrawCMSLabel( SIMULATION );
    mgr::SaveToPDF( c, PlotFileName( "topres_" + mgr->Name() ) );
    delete leg;
    delete c;
  }
  {// tstar resolution
    TCanvas* c = mgr::NewCanvas();

    TH1D* hist1 = mgr->Hist( "HadTstarMass" );
    TH1D* hist2 = mgr->Hist( "LepTstarMass" );
    hist1->Draw( "HIST" );
    hist2->Draw( "HISTSAME" );

    mgr::SetSinglePad( c );
    mgr::SetAxis( hist1 );
    hist1->GetXaxis()->SetTitle( "Top mass (truth reconstruction) (GeV/c^{2})" );
    hist1->SetMaximum( mgr::GetYmax( hist1, hist2 ) * 1.2 );

    hist1->SetLineColor( KBLUE );
    hist2->SetLineColor( KRED  );

    const string hadtstar = boost::str(legentry % "Hadronic t*" % hist1->GetRMS());
    const string leptstar = boost::str(legentry % "Leptonic t*" % hist2->GetRMS());

    TLegend* leg = mgr::NewLegend( 0.5, 0.7 );
    leg->AddEntry( hist1, hadtstar.c_str(), "l" );
    leg->AddEntry( hist2, leptstar.c_str(), "l" );
    leg->Draw();

    mgr::DrawCMSLabel( SIMULATION );
    mgr::SaveToPDF( c, PlotFileName( "tstarres_" + mgr->Name() ) );
    delete leg;
    delete c;
  }
  {// tstar resolution by difference
    TCanvas* c = mgr::NewCanvas();

    TH1D* hist = mgr->Hist( "TstarMassDiff" );
    hist->Draw( "HIST" );
    mgr::SetSinglePad( c );
    mgr::SetAxis( hist );
    hist->SetMaximum( mgr::GetYmax( hist ) * 1.2 );

    const string diffentry = boost::str(legentry % "(Had t*)-(Lep t*)" % hist->GetRMS());
    TLegend* leg        = mgr::NewLegend( 0.5, 0.7 );
    leg->AddEntry( hist, diffentry.c_str(), "l" );
    leg->Draw();

    mgr::DrawCMSLabel( SIMULATION );
    mgr::SaveToPDF( c, PlotFileName( "tstardiffres_" + mgr->Name() ) );
    delete c;
  }
}

/*******************************************************************************
 *
 *  Filename    : ScalePlot.cc
 *  Description : Drawing the comparison plots for effects of scaling
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/ScaleHistMgr.hpp"
#include "TstarAnalysis/TstarMassReco/interface/Bin_Common.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"

#include "TCanvas.h"

using namespace std;

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void ScalePlot( ScaleHistMgr* mgr )
{
   TCanvas* c = new TCanvas("c","c",DEFAULT_CANVAS_WIDTH,DEFAULT_CANVAS_HEIGHT);
   TLegend* tl = plt::NewLegend(0.6,0.7);
   // Drawing everything
   mgr->Hist("TstarMass_resup")->Draw("SAME");
   mgr->Hist("TstarMass_resdw")->Draw("SAME");
   mgr->Hist("TstarMass")->Draw("SAME");

   // Styling
   mgr->Hist("TstarMass_resup")->SetLineColor(kRed) ;
   mgr->Hist("TstarMass_resdw")->SetLineColor(kRed) ;

   tl->AddEntry(mgr->Hist("TstarMass") , "Central Value" , "l" );
   tl->AddEntry(mgr->Hist("TstarMass_resdw") , "Effect of Jet Resolution" , "l" );
   tl->Draw();
   c->SaveAs( reconamer.PlotFileName("scale",{"res"}).c_str() );
   delete c;

   c = new TCanvas("c","c",DEFAULT_CANVAS_WIDTH,DEFAULT_CANVAS_HEIGHT);
   tl = plt::NewLegend(0.6,0.7);

   // Drawing everything
   mgr->Hist("TstarMass_jecup")->Draw("SAME");
   mgr->Hist("TstarMass_jecdw")->Draw("SAME");
   mgr->Hist("TstarMass")->Draw("SAME");

   // Styling
   mgr->Hist("TstarMass_jecup")->SetLineColor(kRed) ;
   mgr->Hist("TstarMass_jecdw")->SetLineColor(kRed) ;
   tl->AddEntry(mgr->Hist("TstarMass") , "Central Value" , "l" );
   tl->AddEntry(mgr->Hist("TstarMass_resdw") , "Effect of Jet Correction Errors" , "l" );
   tl->Draw();

   c->SaveAs( reconamer.PlotFileName("scale",{"jec"}).c_str() );
}

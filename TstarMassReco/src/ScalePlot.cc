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

   // Drawing everything
   mgr->Hist("TstarMass")->Draw();
   mgr->Hist("TstarMass_resup")->Draw("SAME");
   mgr->Hist("TstarMass_resdw")->Draw("SAME");
   mgr->Hist("TstarMass_jecup")->Draw("SAME");
   mgr->Hist("TstarMass_jecdw")->Draw("SAME");

   // Styling
   mgr->Hist("TstarMass_resup")->SetLineColor(kRed) ;
   mgr->Hist("TstarMass_resdw")->SetLineColor(kRed) ;
   mgr->Hist("TstarMass_jecup")->SetLineColor(kGreen) ;
   mgr->Hist("TstarMass_jecdw")->SetLineColor(kGreen) ;

   c->SaveAs( reconamer.PlotFileName("scale").c_str() );
   delete c;
}

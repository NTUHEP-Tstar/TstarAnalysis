/*******************************************************************************
*
*  Filename    : RooFit_CompareBackground.cc
*  Description : Control flow of background function comparison fitting and plotting
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"

#include <vector>
#include "RooDataSet.h"

using namespace std;

static Color_t sequence[] = {
  KBLUE,
  KGREEN,
  KRED
};

void
CompareFitFunc( SampleRooFitMgr* mgr )
{
  TCanvas* c = mgr::NewCanvas();
  RooPlot* frame = SampleRooFitMgr::x().frame();

  TGraph*  setplot = mgr::PlotOn( frame, mgr->DataSet("") );

  vector<TGraph*>  pdfplotlist ;

  const vector<string>  funclist = limnamer.GetInputList<string>("fitfunc");

  for( const auto& fitfunc : funclist ){
    RooAbsPdf* pdf  = mgr->NewPdf( fitfunc, fitfunc );
    RooDataSet* set = (RooDataSet*)( mgr->DataSet( "" ) );
    pdf->fitTo(
      *set,
      RooFit::Range( "FitRange" ),
      RooFit::Minos( kTRUE ),
      RooFit::Save(),
      RooFit::Verbose( kFALSE ),
      RooFit::PrintLevel( -1 ),
      RooFit::PrintEvalErrors( -1 ),
      RooFit::Warnings( kFALSE )
      );
    pdfplotlist.push_back(
      mgr::PlotOn( frame, pdf )
    );
  }

  frame->Draw();
  mgr::SetSinglePad( c );
  mgr::SetFrame( frame );
  frame->SetMinimum(0.3);

  // Common styling
  tstar::SetDataStyle( setplot );

  for( size_t i = 0 ; i < pdfplotlist.size() ; ++i ){
    auto pdfplot = pdfplotlist.at(i);
    pdfplot->SetLineColor( sequence[i] );
    pdfplot->SetLineWidth(2);
  }

  // Drawing common
  mgr::DrawCMSLabel();
  mgr::DrawLuminosity( mgr::SampleMgr::TotalLuminosity() );

  // Drawing legends
  TLegend* l  = mgr::NewLegend( 0.5,0.6 );
  if( limnamer.GetInput<string>("sample")=="Data" ){
    l->AddEntry( setplot, "Data", "lp" );
  } else {
    l->AddEntry( setplot, "MC background", "lp" );
  }

  for( size_t i = 0 ; i < pdfplotlist.size() ; ++i ){
    auto pdfplot = pdfplotlist.at(i);
    auto funcname = funclist.at(i);
    l->AddEntry( pdfplot , limnamer.ExtQuery<string>("fitfunc", funcname,"Full Name").c_str() , "l" );
  }
  l->Draw();

  mgr::LatexMgr  latex;
  latex.SetOrigin( PLOT_X_TEXT_MIN, PLOT_Y_TEXT_MAX, TOP_LEFT )
  .WriteLine( limnamer.GetChannelEXT( "Root Name" ) );


  mgr::SaveToPDF( c, limnamer.PlotFileName("bkgcomp") );
  c->SetLogy(kTRUE);
  frame->SetMaximum( mgr::GetYmax(setplot)*300 );
  mgr::SaveToPDF( c, limnamer.PlotFileName("bkgcomp",{"log"} ) );

  delete c;
  delete frame;
}

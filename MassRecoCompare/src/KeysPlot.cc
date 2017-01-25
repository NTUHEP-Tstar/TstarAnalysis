/*******************************************************************************
*
*  Filename    : KeysPlot.cc
*  Description : Implementation for Keys Comparison plots
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"

#include <boost/format.hpp>

using namespace std;

void
MakeKeysPlots( KeysCompMgr* mgr )
{
   TCanvas* c     = mgr::NewCanvas();
   RooPlot* frame = KeysCompMgr::x().frame();

   RooAbsData* dataset = mgr->DataSet( "" );

   TGraph* setplot = mgr::PlotOn( frame, dataset ,
      RooFit::DrawOption( PGS_DATA )
   );

   vector<TGraph*> keyplotlist ;
   for( const auto& pdfname : mgr->PdfNameList() ){
      keyplotlist.push_back(
         mgr::PlotOn( frame, mgr->Pdf(pdfname) )
      );
   }

   frame->Draw();

   // Styling
   mgr::SetFrame( frame );
   mgr::DrawCMSLabel( SIMULATION );
   for( size_t i = 0 ; i < keyplotlist.size() ; ++i ) {
      keyplotlist.at(i)->SetLineColor( Color_Sequence[i] );
   }

   // Adding additional label
   TLatex tl;
   tl.SetNDC( kTRUE );
   tl.SetTextFont( FONT_TYPE );
   tl.SetTextSize( AXIS_TITLE_FONT_SIZE );
   tl.SetTextAlign( TOP_LEFT );
   tl.DrawLatex( PLOT_X_MIN + 0.02, PLOT_Y_MAX-0.02, mgr->LatexName().c_str() );

   // Making legend
   TLegend* leg = mgr::NewLegend( 0.55,0.6 );

   boost::format setfmt( "Signal (%dGeV/c^{2})" );
   const string setentry  = str(setfmt % reconamer.InputInt("mass"));
   leg->AddEntry( setplot, setentry.c_str(), "lp" );

   boost::format pdffmt( "PDF approx (%s)" );
   for( size_t i = 0 ; i < keyplotlist.size() ; ++i ){
      const TGraph* pdfplot = keyplotlist.at(i);
      const string pdfname = mgr->PdfNameList().at(i);
      const RooAbsPdf* pdf = mgr->Pdf( pdfname );
      const string pdfentry = str( pdffmt % pdf->GetTitle() );
      leg->AddEntry( pdfplot, pdfentry.c_str(), "l" );
   }
   leg->Draw();

   // Y axis range settings
   keyplotlist.push_back( setplot );
   const double ymax = mgr::GetYmax( keyplotlist );

   // for Linear scale
   frame->SetMinimum( 0 );
   frame->SetMaximum( ymax * 1.2 );
   mgr::SaveToPDF( c, reconamer.PlotFileName( "keyscomp", {} ) );

   // for Log scale
   frame->SetMinimum( 0.3 );
   frame->SetMaximum( pow( 10, (log10(ymax)+0.5)*1.2) );
   c->SetLogy(kTRUE);
   mgr::SaveToPDF( c, reconamer.PlotFileName( "keyscomp", {"log"} ) );

   delete c;
   delete frame;
}

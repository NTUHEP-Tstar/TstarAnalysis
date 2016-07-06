/*******************************************************************************
 *
 *  Filename    : FitParPlots.cc
 *  Description : implemetations for comparing fitting results
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"

#include "RooFitResult.h"
#include "RooRealVar.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------

void MakeFitParPlots( const unsigned num_of_par )
{
//   RooRealVar*          temp_var_list[num_of_par] ;
//   map<int,RooRealVar*> simfit_var_list[num_of_par];
//
//   // Getting variables
//   SetMethod("Template");
//   TFile* temp_file = TFile::Open( GetFitResults().c_str() );
//   RooFitResult* temp_result = (RooFitResult*)(temp_file->Get("results"));
//
//   for( unsigned i = 0 ; i < num_of_par ; ++i ){
//      temp_var_list[i] = (RooRealVar*)temp_result->floatParsFinal().at(i) ;
//   }
//
//   const mgr::ConfigReader& cfg( GetJsonFile() );
//   vector<string> masspoint_list = cfg.GetStaticStringList( "SignalList" );
//
//   SetMethod("SimFit");
//   TFile* simfit_file = TFile::Open( GetFitResults().c_str() );
//   for( auto& masspoint : masspoint_list ){
//      RooFitResult* simfit_result = (RooFitResult*)(simfit_file->Get( ("results_" + //masspoint).c_str() ));
//      for( unsigned i = 0 ; i < num_of_par ; ++i ) {
//         simfit_var_list[i][GetSignalMass(masspoint)] = //(RooRealVar*)simfit_result->floatParsFinal().at(i) ;
//      }
//   }
//
//   for( unsigned i = 0 ; i < num_of_par ; ++i ){
//      TMultiGraph* mg = new TMultiGraph();
//      TGraphAsymmErrors* temp_plot    = new TGraphAsymmErrors( masspoint_list.size() );
//      TGraphAsymmErrors* simfit_plot  = new TGraphAsymmErrors( masspoint_list.size() );
//
//      size_t bin = 0;
//      double y_max = -3000;
//      double y_min = 3000;
//      for( auto& x : simfit_var_list[i] ){
//         const double mass = x.first ;
//         RooRealVar* temp_var   = temp_var_list[i];
//         RooRealVar* simfit_var = x.second ;
//
//         cout << mass << " " << temp_var->GetName() << " " <<  simfit_var->GetName() << endl;
//         temp_plot->SetPoint( bin, mass, temp_var->getVal() );
//         temp_plot->SetPointError( bin, 0,0, -temp_var->getAsymErrorLo(), temp_var->getAsymErrorHi() //);
//
//         simfit_plot->SetPoint( bin, mass, simfit_var->getVal() );
//         simfit_plot->SetPointError( bin, 0,0, -simfit_var->getAsymErrorLo(), //simfit_var->getAsymErrorHi() );
//
//         y_max = max( temp_var->getVal() + temp_var->getAsymErrorHi() , y_max );
//         y_max = max( simfit_var->getVal() + simfit_var->getAsymErrorHi() , y_max );
//
//         y_min = max( temp_var->getVal() - temp_var->getAsymErrorLo() , y_min );
//         y_min = max( simfit_var->getVal() - simfit_var->getAsymErrorLo() , y_min );
//
//         ++bin;
//      }
//
//      y_max = max( y_max*2 , y_max + fabs(y_max) );
//      y_min = max( y_min*2 , y_min - fabs(y_min) );
//
//      temp_plot->SetFillStyle(3004);
//      temp_plot->SetLineColor(kBlue);
//      temp_plot->SetFillColor(kBlue);
//
//      simfit_plot->SetMarkerStyle(21);
//      simfit_plot->SetLineColor(kBlack);
//      simfit_plot->SetFillColor(kBlack);
//
//      mg->Add( temp_plot , "L3" );
//      mg->Add( simfit_plot, "P" );
//
//      string varname;
//      varname.push_back( 'a'+i );
//      TCanvas* c = new TCanvas("c1","c1",CANVAS_WIDTH,CANVAS_HEIGHT);
//      mg->Draw("A");
//      c->SaveAs( GetFitCompare(varname).c_str() );
//
//      mg->SetMaximum( y_max );
//      mg->SetMinimum( y_min );
//
//      delete temp_plot;
//      delete simfit_plot;
//      delete mg ;
//      delete c;
//   }
//
//   return;
}

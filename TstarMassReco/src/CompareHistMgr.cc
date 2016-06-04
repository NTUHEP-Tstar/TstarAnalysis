/*******************************************************************************
 *
 *  Filename    : CompareHistMgr.cc
 *  Description : Implementation of comparison manager
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/CompareHistMgr.hh"

#include "TCanvas.h"

#include <iostream>

using namespace std;
using fwlite::Event;
using fwlite::Handle;

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
CompareHistMgr::CompareHistMgr(
   const string& name,
   const string& latex_name,
   const string& model,
   const string& product,
   const string& process
):
   Named(name), // From virtual inheritance
   HistMgr(name),
   _module_label(model),
   _product_label(product),
   _process_label(process)
{
   SetLatexName( latex_name );
   HistMgr::AddHist( "TstarMass" , "t^{*} Mass"            , "GeV/c^{2}", 120, 0, 3000 );
   HistMgr::AddHist( "ChiSq"     , "#chi^{2} of Method"    , ""         , 120, 0, 3000 );
   HistMgr::AddHist( "LepTopMass", "Leptonic Top Mass"     , "GeV/c^{2}", 100, 0, 500  );
   HistMgr::AddHist( "HadTopMass", "Hadronic Top Mass"     , "GeV/c^{2}", 100, 0, 500  );
   HistMgr::AddHist( "HadWMass"  , "Hadronic W Boson Mass" , "GeV/c^{2}", 40,  0, 200  );
   HistMgr::AddHist( "LepPtDiff"  , "Difference in Lepton p_{T}",         "GeV/c", 80,  -200, 200 );
   HistMgr::AddHist( "LepBPtDiff" , "Difference in Leptonic b-tag p_{T}", "GeV/c", 80,  -200, 200 );
   HistMgr::AddHist( "LepGPtDiff" , "Difference in Leptonic gluon p_{T}", "GeV/c", 80,  -200, 200 );
   HistMgr::AddHist( "HadBPtDiff" , "Difference in Hadronic b-tag p_{T}", "GeV/c", 80,  -200, 200 );
   HistMgr::AddHist( "HadGPtDiff" , "Difference in Hadronic gluon p_{T}", "GeV/c", 80,  -200, 200 );
   HistMgr::AddHist( "NeuPz"     , "Neutrino P_{z}"        , "GeV/c",     40,  0, 400 );


   _match_map = new TH2D( (_name + "JetMatchMap").c_str(), (_name+"JetMatchMap").c_str(),
      5 , 0 , 5 , // Xaxis  fitted result
      6 , 0 , 6   // Yaxis  mc truth particle (must include unknown)
   );

   _event_count  = 0 ;
   _lep_match    = 0 ;
   _lep_b_match  = 0 ;
   _lep_g_match  = 0 ;
   _had_j1_match = 0 ;
   _had_j2_match = 0 ;
   _had_b_match  = 0 ;
   _had_g_match  = 0 ;
}

CompareHistMgr::~CompareHistMgr()
{
   delete _match_map;
}

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
int GetBinPosition( Particle_Label );

void CompareHistMgr::AddEvent( const fwlite::Event& ev )
{
   _result_handle.getByLabel( ev, _module_label.c_str(), _product_label.c_str(), _process_label.c_str() );
   const RecoResult& result = *(_result_handle);
   Hist( "TstarMass"  )->Fill( result.TstarMass() );
   Hist( "ChiSq"      )->Fill( result.ChiSquare() );
   Hist( "LepTopMass" )->Fill( result.LeptonicTop().M()   );
   Hist( "HadTopMass" )->Fill( result.HadronicTop().M()   );
   Hist( "HadWMass"   )->Fill( result.HadronicW().M()   );
   Hist( "LepPtDiff"  )->Fill( result.Lepton().FittedP4().Pt()        - result.Lepton().ObservedP4().Pt() );
   Hist( "LepBPtDiff" )->Fill( result.LeptonicBJet().FittedP4().Pt()  - result.LeptonicBJet().ObservedP4().Pt() );
   Hist( "LepGPtDiff" )->Fill( result.LeptonicGluon().FittedP4().Pt() - result.LeptonicGluon().ObservedP4().Pt() );
   Hist( "HadBPtDiff" )->Fill( result.HadronicBJet().FittedP4().Pt()  - result.HadronicBJet().ObservedP4().Pt() );
   Hist( "HadGPtDiff" )->Fill( result.HadronicGluon().FittedP4().Pt() - result.HadronicGluon().ObservedP4().Pt() );
   Hist( "NeuPz"      )->Fill( result.Neutrino().FittedP4().Pz()   );

   // Match counting
   ++_event_count;
   if( result.Lepton().FitMatchTruth() ) {
      ++_lep_match;
   }
   if( result.LeptonicBJet().FitMatchTruth() ){
      ++_lep_b_match;
   }
   if( result.LeptonicGluon().FitMatchTruth() ){
      ++_lep_g_match;
   }
   if( result.HadronicJet1().FitMatchTruth() ) {
      ++_had_j1_match;
   }
   if( result.HadronicJet2().FitMatchTruth() ) {
      ++_had_j2_match;
   }
   if( result.HadronicBJet().FitMatchTruth() ) {
      ++_had_b_match;
   }
   if( result.HadronicGluon().FitMatchTruth() ) {
      ++_had_g_match;
   }
   _match_map->Fill( GetBinPosition(lepb_label), GetBinPosition(result.LeptonicBJet().TypeFromTruth()) );
   _match_map->Fill( GetBinPosition(lepg_label), GetBinPosition(result.LeptonicGluon().TypeFromTruth()) );
   _match_map->Fill( GetBinPosition(hadw1_label), GetBinPosition(result.HadronicJet1().TypeFromTruth()) , 0.5);
   _match_map->Fill( GetBinPosition(hadw2_label), GetBinPosition(result.HadronicJet2().TypeFromTruth()) , 0.5);
   _match_map->Fill( GetBinPosition(hadb_label), GetBinPosition(result.HadronicBJet().TypeFromTruth()) );
   _match_map->Fill( GetBinPosition(hadg_label), GetBinPosition(result.HadronicGluon().TypeFromTruth()) );
}

void CompareHistMgr::SaveMatchMap()
{
   TCanvas* c = new TCanvas("c","c",500,600);
   _match_map->Scale(1./EventCount());
   _match_map->Draw("COLZ");
   //_match_map->SetMaximum(1.);
   _match_map->SetMinimum(0.);
   _match_map->SetStats(0);
   _match_map->GetXaxis()->SetTitle("Type from Fit");
   _match_map->GetXaxis()->SetBinLabel(1,"Leptonic b-quark");
   _match_map->GetXaxis()->SetBinLabel(2,"Leptonic gluon");
   _match_map->GetXaxis()->SetBinLabel(3,"Hadronic W jet");
   _match_map->GetXaxis()->SetBinLabel(4,"Hadronic b-quark");
   _match_map->GetXaxis()->SetBinLabel(5,"Hadronic gluon");
   _match_map->GetYaxis()->SetTitle("Type from MC Truth");
   _match_map->GetYaxis()->SetBinLabel(1,"Leptonic b-quark");
   _match_map->GetYaxis()->SetBinLabel(2,"Leptonic gluon");
   _match_map->GetYaxis()->SetBinLabel(3,"Hadronic W jet");
   _match_map->GetYaxis()->SetBinLabel(4,"Hadronic b-quark");
   _match_map->GetYaxis()->SetBinLabel(5,"Hadronic gluon");
   _match_map->GetYaxis()->SetBinLabel(6,"unknown");

   c->SaveAs( (_name+"_jetmatchmap.png").c_str() );
   delete c;
}

int GetBinPosition( Particle_Label x )
{
   if( x == lepb_label ){
      return 0;
   } else if( x == lepg_label ){
      return 1;
   } else if( x == hadw1_label || x == hadw2_label ){
      return 2;
   } else if( x == hadb_label ){
      return 3;
   } else if( x== hadg_label ){
      return 4;
   } else {
      return 5;
   }
}


//------------------------------------------------------------------------------
//   Match rate computations
//------------------------------------------------------------------------------
unsigned CompareHistMgr::EventCount() const { return _event_count; }
double CompareHistMgr::LepMatchRate() const  { return (double)_lep_match/double(_event_count); }
double CompareHistMgr::LepBMatchRate() const { return (double)_lep_b_match/double(_event_count); }
double CompareHistMgr::LepGMatchRate() const { return (double)_lep_g_match/double(_event_count); }
double CompareHistMgr::HadJ1MatchRate() const{ return (double)_had_j1_match/double(_event_count); }
double CompareHistMgr::HadJ2MatchRate() const{ return (double)_had_j2_match/double(_event_count); }
double CompareHistMgr::HadBMatchRate() const { return (double)_had_b_match/double(_event_count); }
double CompareHistMgr::HadGMatchRate() const { return (double)_had_g_match/double(_event_count); }

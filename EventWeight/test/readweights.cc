/*******************************************************************************
*
*  Filename    : readweights.cc
*  Description : Testing the read weights functions for output files
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"

#include "ManagerUtils/EDMUtils/interface/Counter.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"

#include "TFile.h"
#include <iostream>

using namespace std;

int
main( int argc, char const* argv[] )
{
  const std::string testfile =
  "/wk_cms2/yichen/TstarAnalysis/EDMStore_New/tstarbaseline/Muon/TstarTstarToTgluonTgluon_M-1000_TuneCUETP8M1_13TeV-madgraph-pythia8_0.root";
  fwlite::Event ev( TFile::Open( testfile.c_str() ) );

  // auto& pdfidgroup = GetPdfIdGrouping( testfile );

  for( ev.toBegin(); !ev.atEnd(); ++ev ){
    // cout << "\nTotal event weight:" <<  GetEventWeight( ev ) << endl;
    // cout << "\tPileup Weight:" << GetPileupWeight( ev ) << endl;
    // cout << "\tPileup Weight (best fit):" << GetPileupWeightBestFit( ev ) << endl;
    // cout << "\tPileup Weight (+4.6%):" << GetPileupWeightXsecup( ev ) << endl;
    // cout << "\tPileup Weight (-4.6%):" << GetPileupWeightXsecdown( ev ) << endl;
    // cout << "\tElectron Weight:" << GetElectronWeight( ev ) << endl;
    // cout << "\tElectron RECO weight:" << GetElectronTrackWeight( ev ) << endl;
    // cout << "\tElectron ID weight:" << GetElectronCutWeight( ev ) << endl;
    // cout << "\tElectron Trigger Weight:" << GetElectronTriggerWeight( ev ) << endl;
    // cout << "\tbtag weight:" << GetBtagWeight( ev ) << endl;
    // cout << "\tbtag weight (up):" << GetBtagWeightUp( ev ) << endl;
    // cout << "\tbtag weight (down):" << GetBtagWeightDown( ev ) << endl;
    // cout << "\tTop Pt weights: " << GetEventTopPtWeight( ev ) << endl;
    // cout << "\tMuon Weight:" << GetMuonWeight( ev ) << endl;
    // cout << "\tMuon Trigger Weight:" << GetMuonTriggerWeight( ev ) << endl;
    // cout << "\tMuon ISO weight:" << GetMuonIsoWeight( ev ) << endl;
    // cout << "\tMuon ID weight: " << GetMuonIDWeight( ev ) << endl;
    cout << "\tPDF weight err: " << GetPdfWeightError( ev, pdfidgroup ) << endl;
  }




  fwlite::Run run( TFile::Open( testfile.c_str() ) );
  fwlite::Handle<mgr::Counter> counthandle;

  for( run.toBegin(); !run.atEnd(); ++run ){
    counthandle.getByLabel( run, "EventWeightAll", "WeightSum" );
    cout << counthandle->value << endl;
  }

  return 0;
}

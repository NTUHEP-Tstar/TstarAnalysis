/*******************************************************************************
 *
 *  Filename    : counter_read.cc
 *  Description : Reading the counters
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/LuminosityBlock.h"
#include "DataFormats/FWLite/interface/Run.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/Common/interface/MergeableCounter.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/MET.h"

#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"

#include "TFile.h"
#include <vector>
#include <iostream>
using namespace std;

int main(int argc, char const *argv[]) {

   cout << "Checking per run information ==============\n\n" << endl;
   fwlite::Run myRun( TFile::Open("tstarBaseline.root") );
   fwlite::Handle<edm::MergeableCounter>  positive;
   fwlite::Handle<edm::MergeableCounter>  negative;
   for( myRun.toBegin() ; !myRun.atEnd() ; ++myRun ){
      positive.getByLabel( myRun , "beforeAny" , "positiveEvents" );
      negative.getByLabel( myRun , "beforeAny" , "negativeEvents" );
      cout << myRun.id() << ": " << positive->value << " /-" << negative->value << endl;
   }

   cout << "\n\n\nChecking per event information ============\n\n" << endl;
   fwlite::Event ev( TFile::Open("tstarBaseline.root") );
   fwlite::Handle<std::vector<pat::Jet>>      jethandle;
   fwlite::Handle<std::vector<pat::Muon>>     muonhandle;
   fwlite::Handle<std::vector<pat::Electron>> elechandle;
   for( ev.toBegin() ; !ev.atEnd() ; ++ev ){
      jethandle.getByLabel ( ev, "skimmedPatJets" );
      muonhandle.getByLabel( ev, "skimmedPatMuons" );
      elechandle.getByLabel( ev, "skimmedPatElectrons" );

      cout << "\n\nJet info =========== \n" << endl;
      for( const auto& jet : jethandle.ref() ){
         cout << "Original momentum:" << GetLorentzVector( jet , "" ).Pt() << endl;
         cout << "Smeared  momentum:" << GetLorentzVector( jet , "ResP4" ).Pt() << endl;
      }

      cout << "\n\nMuon info =========== \n" << endl;
      for( const auto& muon : muonhandle.ref()  ){
         cout << "Original momentum:" << GetLorentzVector( muon , "" ).Pt() << endl;
         cout << "Trigger  momentum:" << GetLorentzVector( muon , "TrigP4" ).Pt() << endl;
      }

      cout << "\n\nElectron info =========== \n" << endl;
      for( const auto& elec : elechandle.ref()  ){
         cout << "Original momentum:" << GetLorentzVector( elec , "" ).Pt() << endl;
         cout << "Trigger  momentum:" << GetLorentzVector( elec , "TrigP4" ).Pt() << endl;
      }
    }

   return 0;
}

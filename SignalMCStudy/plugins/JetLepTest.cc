/*******************************************************************************
*
*  Filename    : JetLepTest
*  Description : Filtering by JetLepTest
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <memory>

#include "TstarAnalysis/BaseLineSelector/interface/TypeDef.hpp"
#include <iostream>
#include <vector>

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"
#include "TTree.h"

using namespace std;

class JetLepTest : public edm::stream::EDAnalyzer<>
{
public:
  explicit
  JetLepTest( const edm::ParameterSet& );
  ~JetLepTest();
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
  virtual void beginJob() ;
  virtual void endJob() ;
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;

  // Data Members
  const edm::EDGetToken _jetsrc;
  const edm::EDGetToken _musrc;
  const edm::EDGetToken _elecsrc;
  edm::Handle<tstar::JetList> _jethandle;
  edm::Handle<tstar::MuonList> _muonhandle;
  edm::Handle<tstar::ElectronList> _elechandle;

  TTree* _tree;
  unsigned _nlepton;
  unsigned _njetlep;
  double _jetlepsep[64];
};

/******************************************************************************/

JetLepTest::JetLepTest( const edm::ParameterSet& iconf ) :
  _jetsrc( GETTOKEN( iconf, tstar::JetList, "jetsrc" ) ),
  _musrc( GETTOKEN( iconf, tstar::MuonList, "muonsrc" ) ),
  _elecsrc( GETTOKEN(iconf,tstar::ElectronList,"elecsrc"))
{
  edm::Service<TFileService> fs;
  _tree = fs->make<TTree>( "MCRes", "MCRes" );

  _tree->Branch( "NLepton", &_nlepton, "NLepton/i");
  _tree->Branch( "NJetLep", &_njetlep, "NJetLep/i" );
  _tree->Branch( "JetLepSep", _jetlepsep, "MinJetLepSep[NJetLep]/D" );
}

/******************************************************************************/

void
JetLepTest::beginJob()
{
}

/******************************************************************************/

void
JetLepTest::endJob()
{}


/******************************************************************************/

void
JetLepTest::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  iEvent.getByToken( _jetsrc, _jethandle );
  iEvent.getByToken( _musrc,  _muonhandle );
  iEvent.getByToken( _elecsrc, _elechandle );

  // Skipping over events with bad number of leptons
  _nlepton = _muonhandle->size() + _elechandle->size();
  _njetlep = 0;

  // Looping over jets
  for( const auto& jet : *_jethandle ){
    TLorentzVector jetp4( jet.px(), jet.py(), jet.pz(), jet.energy() );

    // actual jet selection
    if( jetp4.Pt() < 30. ){ continue; }
    if( fabs( jetp4.Eta() ) > 2.4 ){ continue; }

    // Jet ID Selection
    if( jet.neutralHadronEnergyFraction() > 0.99 ){ continue; }
    if( jet.neutralEmEnergyFraction()     > 0.99 ){ continue; }
    if( jet.numberOfDaughters() <= 1             ){ continue; }
    if( fabs( jetp4.Eta() ) < 2.4 ){
      if( jet.chargedHadronEnergyFraction() <= 0 ){ continue; }
      if( jet.chargedMultiplicity()         <= 0 ){ continue; }
      if( jet.chargedEmEnergyFraction()   > 0.99 ){ continue; }
    }

    // Looping over muons
    for( const auto& mu : *_muonhandle ){
      const TLorentzVector mup4 = mgr::GetLorentzVector( mu, "" );// getting default four momentum
      _jetlepsep[_njetlep++] = jetp4.DeltaR( mup4 );
    }
    for( const auto& el : *_elechandle ){
      const TLorentzVector elp4 = mgr::GetLorentzVector( el, "" );// getting default four momentum
      _jetlepsep[_njetlep++] = jetp4.DeltaR( elp4 );
    }

  }

  _tree->Fill();
}


/*******************************************************************************
*   Generic Plugin requirements
*******************************************************************************/
JetLepTest::~JetLepTest()
{
}


void
JetLepTest::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
  // The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( JetLepTest );

/*******************************************************************************
*
*  Filename    : MCResolution
*  Description : Filtering by MCResolution
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include <memory>

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hpp"
#include <iostream>
#include <vector>

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"
#include "TTree.h"

using namespace std;

class MCResolution : public edm::stream::EDAnalyzer<>
{
public:
  explicit
  MCResolution( const edm::ParameterSet& );
  ~MCResolution();
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
  virtual void beginJob();
  virtual void endJob();
  virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;

  // Data Members
  const edm::EDGetToken _jetsrc;
  const edm::EDGetToken _musrc;
  const edm::EDGetToken _elecsrc;
  const edm::EDGetToken _gensrc;
  edm::Handle<std::vector<pat::Jet> > _jethandle;
  edm::Handle<std::vector<pat::Muon> > _muonhandle;
  edm::Handle<std::vector<pat::Electron> > _elechandle;
  edm::Handle<vector<reco::GenParticle> > _genhandle;

  TTree* _tree;
  double _lepwmass;
  double _leptopmass;
  double _leptstarmass;
  double _hadwmass;
  double _hadtopmass;
  double _hadtstarmass;

  const pat::Muon* _muon;
  const pat::Electron* _electron;
  const reco::GenParticle* _neutrino;
  const pat::Jet* _lepbjet;
  const pat::Jet* _lepgjet;
  const pat::Jet* _hadwjet1;
  const pat::Jet* _hadwjet2;
  const pat::Jet* _hadbjet;
  const pat::Jet* _hadgjet;

  // Helper functions
  bool FindObjects();
  bool ComputeMass();

};

/******************************************************************************/

MCResolution::MCResolution( const edm::ParameterSet& iconf ) :
  _jetsrc( GETTOKEN( iconf, vector<pat::Jet>, "jetsrc" ) ),
  _musrc( GETTOKEN( iconf, vector<pat::Muon>, "muonsrc" ) ),
  _elecsrc( GETTOKEN( iconf, vector<pat::Electron>, "elecsrc" ) ),
  _gensrc( GETTOKEN( iconf, vector<reco::GenParticle>, "gensrc" ) )
{
  edm::Service<TFileService> fs;
  _tree = fs->make<TTree>( "MCResolution", "MCResolution" );

  _tree->Branch( "LepWMass",     &_lepwmass,     "LepWMass/D"     );
  _tree->Branch( "LepTopMass",   &_leptopmass,   "LepTopMass/D"   );
  _tree->Branch( "LepTstarMass", &_leptstarmass, "LepTstarMass/D" );
  _tree->Branch( "HadWMass",     &_hadwmass,     "HadWMass/D"     );
  _tree->Branch( "HadTopMass",   &_hadtopmass,   "HadTopMass/D"   );
  _tree->Branch( "HadTstarMass", &_hadtstarmass, "HadTstarMass/D" );
}

/******************************************************************************/

void
MCResolution::beginJob()
{}

/******************************************************************************/

void
MCResolution::endJob()
{}


/******************************************************************************/

void
MCResolution::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  iEvent.getByToken( _jetsrc,  _jethandle );
  iEvent.getByToken( _musrc,   _muonhandle );
  iEvent.getByToken( _elecsrc, _elechandle );
  iEvent.getByToken( _gensrc,  _genhandle );

  // Early exit if unable to find objects
  if( !FindObjects() ){ return; }

  // Early exit if leptonic w cannot be solve properly
  if( !ComputeMass() ){ return; }

  // Fill tree if everything is OK
  _tree->Fill();
}


/******************************************************************************/
bool
MCResolution::FindObjects()
{
  // Set all pointer to zero
  _muon     = nullptr;
  _electron = nullptr;
  _neutrino = nullptr;
  _lepbjet  = nullptr;
  _lepgjet  = nullptr;
  _hadwjet1 = nullptr;
  _hadwjet2 = nullptr;
  _hadbjet  = nullptr;
  _hadgjet  = nullptr;

  // Finding lepton in reco collection
  if( _muonhandle.isValid() ){
    for( const auto& lepton : *_muonhandle ){
      if( !_muon && lepton.genLepton() && FromLeptonicW( lepton.genLepton() ) ){
        _muon = &lepton;
        break;
      }
    }
  }

  if( _elechandle.isValid() ){
    for( const auto lepton : *_elechandle ){
      if( !_electron && lepton.genLepton() && FromLeptonicW( lepton.genLepton() ) ){
        _electron = &lepton;
        break;
      }
    }
  }

  if( !_electron && !_muon ){
    // cout << "Could not find lepton" << endl;
    return false;
  }

  // Finding jet in reco collection;
  for( const auto& jet : *_jethandle ){
    const auto gen = jet.genParton();
    if( !gen ){
      continue;
    } else if( !_lepbjet  && FromLeptonicTop( gen ) ){
      _lepbjet = &jet;
    } else if( !_lepgjet  && FromLeptonicTstar( gen ) ){
      _lepgjet = &jet;
    } else if( !_hadwjet1 && FromHadronicW( gen ) ){
      _hadwjet1 = &jet;
    } else if( !_hadwjet2 && FromHadronicW( gen ) ){
      _hadwjet2 = &jet;
    } else if( !_hadbjet  && FromHadronicTop( gen ) ){
      _hadbjet = &jet;
    } else if( !_hadgjet  && FromHadronicTstar( gen ) ){
      _hadgjet = &jet;
    }
  }

  if( !_lepbjet || !_lepgjet || !_hadwjet1 || !_hadwjet2  || !_hadbjet || !_hadgjet ){
    return false;
  }


  // Finding neutrino in **GEN** collection
  for( const auto& gen : *_genhandle ){
    if( !_neutrino && FromLeptonicW( &gen ) && ( abs( gen.pdgId() ) == 12 || abs( gen.pdgId() == 14 ) ) ){
      _neutrino = &gen;
      break;
    }
  }

  if( !_neutrino ){
    // cout << "Could not find neutrino" << endl;
    return false;
  }

  return true;
}

/******************************************************************************/

bool
MCResolution::ComputeMass()
{
  const auto lepw     = ((_muon)?_muon->p4():_electron->p4()) + _neutrino->p4();
  const auto leptop   = lepw + _lepbjet->p4();
  const auto leptstar = leptop + _lepgjet->p4();
  const auto hadw     = _hadwjet1->p4() + _hadwjet2->p4();
  const auto hadtop   = hadw + _hadbjet->p4();
  const auto hadtstar = hadtop + _hadgjet->p4();

  _lepwmass     = lepw.M();
  _leptopmass   = leptop.M();
  _leptstarmass = leptstar.M();
  _hadwmass     = hadw.M();
  _hadtopmass   = hadtop.M();
  _hadtstarmass = hadtstar.M();
  return true;
}


/*******************************************************************************
*   Generic Plugin requirements
*******************************************************************************/
MCResolution::~MCResolution()
{
}


void
MCResolution::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
  // The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( MCResolution );

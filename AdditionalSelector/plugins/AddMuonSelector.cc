/*******************************************************************************
*
*  Filename    : AddMuonSelector.cc
*  Description : Additional lepton selector
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/PhysUtils/interface/TriggerMatching.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/TypeDef.hpp"

#include <memory>
#include <vector>

class AddMuonSelector : public edm::stream::EDFilter<>
{
public:
  explicit
  AddMuonSelector( const edm::ParameterSet& );
  ~AddMuonSelector();
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
  virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

  // Data Members
  const edm::EDGetToken _muonsrc;
  const edm::EDGetToken _hltsrc;
  const edm::EDGetToken _triggerobjsrc;

  const std::vector<std::pair<std::string, std::string> > _reqtriggerlist;
  std::vector<std::pair<std::string, std::string> > GetTriggerList( const edm::ParameterSet& iConfig, const std::string& tag ) const;

  edm::Handle<std::vector<pat::Muon> > _muonHandle;
  edm::Handle<edm::TriggerResults> _hltHandle;
  edm::Handle<pat::TriggerObjectStandAloneCollection> _triggerObjectHandle;

  bool passtrigger( const pat::Muon&, const edm::Event& ) const;
};

using namespace tstar;
AddMuonSelector::AddMuonSelector( const edm::ParameterSet& iConfig ) :
  _muonsrc( GETTOKEN( iConfig,  MuonList,  "muonsrc" ) ),
  _hltsrc( GETTOKEN( iConfig,  TriggerResults, "hltsrc" ) ),
  _triggerobjsrc( GETTOKEN( iConfig, TriggerObjList, "trgobjsrc" ) ),
  _reqtriggerlist( GetTriggerList( iConfig, "reqtrigger" ) )
{
}


bool
AddMuonSelector::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  iEvent.getByToken( _muonsrc,       _muonHandle   );
  iEvent.getByToken( _hltsrc,        _hltHandle    );
  iEvent.getByToken( _triggerobjsrc, _triggerObjectHandle );

  if( _muonHandle->size() != 1 ){ return true; }
  // only parsing events that contain exactly one muon

  const auto& mu = _muonHandle->at( 0 );

  // if( mu.pt() < 30 ){ return false; }
  if( fabs( mu.eta() ) > 2.1 ){ return false; }

  return passtrigger( mu, iEvent );
}

/*******************************************************************************
*   Helper functions for initialization trigger list
*******************************************************************************/
std::vector<std::pair<std::string, std::string> >
AddMuonSelector::GetTriggerList(
  const edm::ParameterSet& iConfig,
  const std::string&       tag
  ) const
{
  std::vector<std::pair<std::string, std::string> > ans;

  for( const auto& pset : iConfig.getParameter<std::vector<edm::ParameterSet> >( tag ) ){
    ans.emplace_back(
      pset.getParameter<std::string>( "hltname" ),
      pset.getParameter<std::string>( "filtername" )
      );
  }

  return ans;
}

/*******************************************************************************
*   Trigger selection
*******************************************************************************/
bool
AddMuonSelector::passtrigger( const pat::Muon& mu, const edm::Event& iEvent ) const
{
  if( _reqtriggerlist.size() == 0 ){
    // returning true is no HLT matching is listed
    return true;
  }

  for( const auto& triggerpair : _reqtriggerlist ){
    const bool hasMatch = mgr::HasTriggerMatch(
      mu,
      *_triggerObjectHandle,
      triggerpair.first,
      triggerpair.second,
      iEvent.triggerNames( *_hltHandle )
      );
    if( hasMatch ){
      // return true if any match is found
      return true;
    }
  }

  return false;
}


/*******************************************************************************
*   Generic Plugin requirements
*******************************************************************************/
AddMuonSelector::~AddMuonSelector()
{
}

void
AddMuonSelector::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
  // The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( AddMuonSelector );

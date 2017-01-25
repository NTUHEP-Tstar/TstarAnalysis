/*******************************************************************************
*
*  Filename    : AddElectronSelector.cc
*  Description : Additional lepton selector
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/PhysUtils/interface/TriggerMatching.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/TypeDef.hpp"
#include "TstarAnalysis/Common/interface/IDCache.hpp"

#include <memory>
#include <vector>

class AddElectronSelector : public edm::stream::EDFilter<>
{
public:
  explicit
  AddElectronSelector( const edm::ParameterSet& );
  ~AddElectronSelector();
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
  virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

  // Data Members
  const edm::EDGetToken _rhosrc;
  const edm::EDGetToken _electronsrc;
  const edm::EDGetToken _hltsrc;
  const edm::EDGetToken _triggerobjsrc;

  edm::Handle<double> _rhoHandle;
  edm::Handle<std::vector<pat::Electron> > _electronHandle;
  edm::Handle<edm::TriggerResults> _hltHandle;
  edm::Handle<pat::TriggerObjectStandAloneCollection> _triggerObjectHandle;

  const std::vector<std::pair<std::string, std::string> > _reqtriggerlist;

  std::vector<std::pair<std::string, std::string> > GetTriggerList( const edm::ParameterSet& iConfig, const std::string& tag ) const;

  bool passtriggersafe( const pat::Electron&, const double ) const;
  bool passtrigger( const pat::Electron&, const edm::Event& ) const;
};


/******************************************************************************/
using namespace tstar;

AddElectronSelector::AddElectronSelector( const edm::ParameterSet& iConfig ) :
  _rhosrc( GETTOKEN( iConfig, double, "rhosrc" ) ),
  _electronsrc( GETTOKEN( iConfig,  ElectronList,  "electronsrc" ) ),
  _hltsrc( GETTOKEN( iConfig,  TriggerResults, "hltsrc" ) ),
  _triggerobjsrc( GETTOKEN( iConfig, TriggerObjList, "trgobjsrc" ) ),
  _reqtriggerlist( GetTriggerList( iConfig, "reqtrigger" ) )
{
}

/******************************************************************************/

bool
AddElectronSelector::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  iEvent.getByToken( _electronsrc, _electronHandle   );

  if( _electronHandle->size() != 1 ){
    // only parsing events that contain exactly one electron
    return true;
  }

  const auto& el = _electronHandle->at( 0 );

  // Basic kinematic cuts:
  if( el.pt() < 35 ){ return false; }

  iEvent.getByToken( _rhosrc,        _rhoHandle );
  iEvent.getByToken( _hltsrc,        _hltHandle    );
  iEvent.getByToken( _triggerobjsrc, _triggerObjectHandle );

  return passtrigger( el, iEvent ) && passtriggersafe( el, *_rhoHandle );
}

/*******************************************************************************
*   Helper functions for initialization trigger list
*******************************************************************************/
std::vector<std::pair<std::string, std::string> >
AddElectronSelector::GetTriggerList(
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
*   Pass trigger safe cuts
*******************************************************************************/
bool
AddElectronSelector::passtriggersafe( const pat::Electron& x, const double rho ) const
{
  // Using cached results of VID
  return PassHLTID( x );

  // Legacy code from Arun
  // const double pt  = x.pt();
  // const double eta = x.superCluster()->eta();

  // const double ecalPFIso = x.ecalPFClusterIso();
  // const double hcalPFIso = x.hcalPFClusterIso();
  // const double trackIso  = x.trackIso();
  // const double detaseed  = x.deltaEtaSeedClusterTrackAtCalo();
  // const double dphiin    = x.deltaPhiSuperClusterTrackAtVtx();
  // const double chi2      = x.gsfTrack()->normalizedChi2();
  // const int misshit      = x.gsfTrack()->hitPattern().numberOfHits( reco::HitPattern::MISSING_INNER_HITS );

  // if( fabs( eta ) <= 1.479 ){// barrel cuts
  //    if( ( ecalPFIso - rho*0.165 )/pt > 0.160 ){ return false; }
  //    if( ( hcalPFIso - rho*0.060 )/pt > 0.120 ){ return false; }
  //    if( ( trackIso/pt )              > 0.08  ){ return false; }
  //    if( fabs( detaseed )             > 0.004 ){ return false; }
  //    if( fabs( dphiin )               > 0.020 ){ return false; }
  //    if( misshit                      > 0     ){ return false; }
  //    return true;
  // } else if( fabs( eta ) < 2.5 ){// endcap cuts
  //    if( ( ecalPFIso - rho*0.132 )/pt > 0.120 ){ return false; }
  //    if( ( hcalPFIso - rho*0.131 )/pt > 0.120 ){ return false; }
  //    if( ( trackIso/pt )              > 0.08  ){ return false; }
  //    if( fabs( chi2 )                 > 3     ){ return false; }
  //    if( misshit                      > 0     ){ return false; }
  //    return true;
  // } else {// bad eta region
  //    return false;
  // }
}


/******************************************************************************/

bool
AddElectronSelector::passtrigger( const pat::Electron& el, const edm::Event& iEvent ) const
{
  if( _reqtriggerlist.size() == 0 ){
    // return true is no trigger is listed
    return true;
  }

  for( const auto& triggerpair : _reqtriggerlist ){
    const bool hasMatch = mgr::HasTriggerMatch(
      el,
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
AddElectronSelector::~AddElectronSelector()
{
}

/******************************************************************************/

void
AddElectronSelector::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
  // The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( AddElectronSelector );

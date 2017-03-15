/*******************************************************************************
*
*  Filename    : LeptonSeparator.cc
*  Description : Selectron according to required leptons
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/TypeDef.hpp"

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class LeptonSeparator : public edm::EDFilter
{
public:
  explicit
  LeptonSeparator( const edm::ParameterSet& );
  ~LeptonSeparator();
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
  virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

  // EDM Tags
  const edm::EDGetToken _muonsrc;
  const edm::EDGetToken _electronsrc;
  edm::Handle<tstar::MuonList> _muonHandle;
  edm::Handle<tstar::ElectronList> _electronHandle;

  const unsigned _reqmuon;
  const unsigned _reqelec;
};


// ------------------------------------------------------------------------------
//   Constructor and Destructor
// ------------------------------------------------------------------------------
LeptonSeparator::LeptonSeparator( const edm::ParameterSet& iConfig ) :
  _muonsrc( GETTOKEN( iConfig, tstar::MuonList, "muonsrc" ) ),
  _electronsrc( GETTOKEN( iConfig, tstar::ElectronList, "electronsrc" ) ),
  _reqmuon( iConfig.getParameter<int>( "reqmuon" ) ),
  _reqelec( iConfig.getParameter<int>( "reqelec" ) )
{
}



bool
LeptonSeparator::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  iEvent.getByToken( _muonsrc,     _muonHandle     );
  iEvent.getByToken( _electronsrc, _electronHandle );

  if( _muonHandle->size() != _reqmuon ){ return false; }
  if( _electronHandle->size() != _reqelec ){ return false; }
  return true;
}

/*******************************************************************************
*   Generic Plugin requirements
*******************************************************************************/
LeptonSeparator::~LeptonSeparator(){}

void
LeptonSeparator::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

DEFINE_FWK_MODULE( LeptonSeparator );

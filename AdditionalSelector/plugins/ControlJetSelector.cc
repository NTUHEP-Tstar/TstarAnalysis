/*******************************************************************************
*
*  Filename    : ControlJetSelector.cc
*  Description : Final checking for filtering
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include <memory>

#include "DataFormats/PatCandidates/interface/Jet.h"
#include <iostream>
#include <string>
#include <vector>

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/PhysUtils/interface/BTagChecker.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/TypeDef.hpp"

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class ControlJetSelector : public edm::EDFilter
{
public:
  explicit
  ControlJetSelector( const edm::ParameterSet& );
  ~ControlJetSelector();
  static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
  virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

  // EDM Tags
  const edm::EDGetToken _jetsrc;
  const edm::EDGetToken _metsrc;
  edm::Handle<tstar::JetList> _jetHandle;
  edm::Handle<tstar::METList> _methandle;

  const mgr::BTagChecker _checker;

  // Control flags for jet counting
  const unsigned _maxjet;
  const unsigned _minjet;

  // Control flags for b tag checking
  const unsigned _btagcheckorder;
  const unsigned _maxfailloose;
  const unsigned _minfailloose;
  const unsigned _maxpassmedium;
  const unsigned _minpassmedium;

  // MET cut
  const double _minmet;
};


// ------------------------------------------------------------------------------
//   Constructor and Destructor
// ------------------------------------------------------------------------------
ControlJetSelector::ControlJetSelector( const edm::ParameterSet& iConfig ) :
  _jetsrc( GETTOKEN( iConfig, tstar::JetList, "jetsrc" ) ),
  _metsrc( GETTOKEN( iConfig, tstar::METList, "metsrc" ) ),
  _checker( "mycheck", GETFILEPATH( iConfig, "btagchecker" ) ),
  _maxjet( iConfig.getParameter<int>( "maxjet" ) ),
  _minjet( iConfig.getParameter<int>( "minjet" ) ),
  _btagcheckorder( iConfig.getParameter<int>( "checkorder" ) ),
  _maxfailloose( iConfig.getParameter<int>( "maxfailloose" ) ),
  _minfailloose( iConfig.getParameter<int>( "minfailloose" ) ),
  _maxpassmedium( iConfig.getParameter<int>( "maxpassmedium" ) ),
  _minpassmedium( iConfig.getParameter<int>( "minpassmedium" ) ),
  _minmet( iConfig.getParameter<double>( "minmet" ) )
{
}

bool
ControlJetSelector::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  iEvent.getByToken( _jetsrc, _jetHandle );
  iEvent.getByToken( _metsrc, _methandle );

  if( !_methandle->size() ) { return false; }
  if( _methandle->front().pt() < _minmet ){ return false; }

  if( _jetHandle->size() >  _maxjet ){ return false; }
  if( _jetHandle->size() <  _minjet ){ return false; }

  unsigned passmedium = 0;
  unsigned failloose  = 0;

  for( unsigned i = 0; i < _btagcheckorder && i < _jetHandle->size(); ++i ){
    const auto& jet = _jetHandle->at( i );
    if( _checker.PassMedium( jet ) ){
      passmedium++;
    } else if( !_checker.PassLoose( jet ) ){
      failloose++;
    }
  }

  if( passmedium > _maxpassmedium ){ return false; }
  if( passmedium < _minpassmedium ){ return false; }
  if( failloose  > _maxfailloose  ){ return false; }
  if( failloose  < _minpassmedium ){ return false; }
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
ControlJetSelector::~ControlJetSelector()
{
}


void
ControlJetSelector::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
  // The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( ControlJetSelector );

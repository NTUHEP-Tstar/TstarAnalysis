/*******************************************************************************
*
*  Filename    : SupSelector.cc
*  Description : Final checking for filtering
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include <memory>

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include <iostream>
#include <string>
#include <vector>

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/TypeDef.hpp"

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class SupSelector : public edm::EDFilter
{
public:
   explicit
   SupSelector( const edm::ParameterSet& );
   ~SupSelector();
   static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
   virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

   // EDM Tags
   const edm::EDGetToken _muonsrc;
   const edm::EDGetToken _electronsrc;
   const edm::EDGetToken _jetsrc;
   edm::Handle<tstar::MuonList> _muonHandle;
   edm::Handle<tstar::ElectronList> _electronHandle;
   edm::Handle<tstar::JetList> _jetHandle;

};


// ------------------------------------------------------------------------------
//   Constructor and Destructor
// ------------------------------------------------------------------------------
SupSelector::SupSelector( const edm::ParameterSet& iConfig ) :
   _muonsrc( GETTOKEN( iConfig, tstar::MuonList, "muonsrc" ) ),
   _electronsrc( GETTOKEN( iConfig, tstar::ElectronList, "electronsrc" ) ),
   _jetsrc( GETTOKEN( iConfig, tstar::JetList, "jetsrc" ) )
{
}

SupSelector::~SupSelector()
{
}


// ------------------------------------------------------------------------------
//   Main control follow
// ------------------------------------------------------------------------------
bool
SupSelector::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   iEvent.getByToken( _muonsrc,     _muonHandle     );
   iEvent.getByToken( _electronsrc, _electronHandle );
   iEvent.getByToken( _jetsrc,      _jetHandle      );

   // Addtional jet pt selection
   if( _jetHandle->at(0).pt() < 120 ) { return false; }
   if( _jetHandle->at(1).pt() < 80 )  { return false; }
   if( _jetHandle->at(2).pt() < 40 )  { return false; }

   return true;
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
SupSelector::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
   // The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( SupSelector );

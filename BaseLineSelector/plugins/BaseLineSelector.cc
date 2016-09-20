/*******************************************************************************
*
*  Filename    : BaseLineSelector.cc
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
*   Constants
*******************************************************************************/
static const int GG_MUON_SIGNAL     = 0;
static const int GG_ELECTRON_SIGNAL = 1;

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class BaseLineSelector : public edm::EDFilter
{
public:
   explicit
   BaseLineSelector( const edm::ParameterSet& );
   ~BaseLineSelector();
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

   // Operation Flags
   const std::string _runMode;
};


// ------------------------------------------------------------------------------
//   Constructor and Destructor
// ------------------------------------------------------------------------------
BaseLineSelector::BaseLineSelector( const edm::ParameterSet& iConfig ) :
   _muonsrc( GETTOKEN( iConfig, tstar::MuonList, "muonsrc" ) ),
   _electronsrc( GETTOKEN( iConfig, tstar::ElectronList, "electronsrc" ) ),
   _jetsrc( GETTOKEN( iConfig, tstar::JetList, "jetsrc" ) ),
   _runMode( iConfig.getParameter<std::string>( "runMode" ) )
{
}

BaseLineSelector::~BaseLineSelector()
{
}


// ------------------------------------------------------------------------------
//   Main control follow
// ------------------------------------------------------------------------------
bool
BaseLineSelector::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   iEvent.getByToken( _muonsrc,     _muonHandle     );
   iEvent.getByToken( _electronsrc, _electronHandle );
   iEvent.getByToken( _jetsrc,      _jetHandle      );

   // Selection for different channels
   if( _runMode == "MuonSignal" ){
      if( _muonHandle->size() != 1 ){
         return false;
      }
      if( _electronHandle->size() > 0 ){
         return false;
      }
   } else if( _runMode == "ElectronSignal" ){
      if( _electronHandle->size() != 1 ){
         return false;
      }
      if( _muonHandle->size() > 0 ){
         return false;
      }
   }
   return true;
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
BaseLineSelector::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
   // The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( BaseLineSelector );

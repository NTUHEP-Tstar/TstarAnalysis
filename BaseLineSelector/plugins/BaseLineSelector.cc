#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include <vector>
#include <string>
#include <iostream>

//------------------------------------------------------------------------------
//   Typedefs and constants
//------------------------------------------------------------------------------
typedef std::vector<pat::Jet> JetList;
typedef std::vector<pat::Muon> MuonList;
typedef std::vector<pat::Electron> ElectronList;

static const int GG_MUON_SIGNAL     = 0;
static const int GG_ELECTRON_SIGNAL = 1;

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class BaseLineSelector : public edm::EDFilter {
public:
   explicit BaseLineSelector( const edm::ParameterSet& );
   ~BaseLineSelector();
   static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
   virtual bool filter( edm::Event&, const edm::EventSetup& )override;

   // EDM Tags
   const edm::EDGetToken  _muonsrc;
   const edm::EDGetToken  _electronsrc;
   const edm::EDGetToken  _jetsrc;
   edm::Handle<MuonList>     _muonHandle;
   edm::Handle<ElectronList> _electronHandle;
   edm::Handle<JetList>      _jetHandle ;

   // Operation Flags
   const int _runMode;
};


//------------------------------------------------------------------------------
//   Constructor and Destructor
//------------------------------------------------------------------------------
BaseLineSelector::BaseLineSelector(const edm::ParameterSet& iConfig):
   _muonsrc(consumes<MuonList>(iConfig.getParameter<edm::InputTag>("muonsrc"))),
   _electronsrc( consumes<ElectronList>(iConfig.getParameter<edm::InputTag>("electronsrc"))),
   _jetsrc( consumes<JetList>(iConfig.getParameter<edm::InputTag>("jetsrc"))),
   _runMode( iConfig.getParameter<int>("runMode") )
{
}

BaseLineSelector::~BaseLineSelector()
{
}


//------------------------------------------------------------------------------
//   Main control follow
//------------------------------------------------------------------------------
bool BaseLineSelector::filter( edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   iEvent.getByToken( _muonsrc     , _muonHandle     );
   iEvent.getByToken( _electronsrc , _electronHandle );
   iEvent.getByToken( _jetsrc      , _jetHandle      );

   // Selection for different channels
   if( _runMode == GG_MUON_SIGNAL ){
      if( _muonHandle->size() != 1 ) {
         return false; }
      if( _electronHandle->size() > 0 ) {
         return false; }
   } else if( _runMode == GG_ELECTRON_SIGNAL ){
      if( _electronHandle->size() != 1 ){
         return false; }
      if( _muonHandle->size() > 0 ) {
         return false; }
   }
   return true;
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void BaseLineSelector::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
   //The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(BaseLineSelector);

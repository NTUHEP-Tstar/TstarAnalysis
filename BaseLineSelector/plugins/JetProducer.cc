#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include <vector>

#include "TstarAnalysis/BaseLineSelector/interface/ObjectCache.hh"

typedef std::vector<pat::Jet> JetList;
typedef std::vector<pat::Muon> MuonList;
typedef std::vector<pat::Electron> ElectronList;
//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class JetProducer : public edm::stream::EDFilter<> {
public:
   explicit JetProducer(const edm::ParameterSet&);
   ~JetProducer();
   static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   bool IsSelectedJet( const pat::Jet& ) const;

private:
   virtual bool filter(edm::Event&, const edm::EventSetup&) override;

   // Data Members
   const edm::EDGetTokenT<JetList>       _jetsrc;
   const edm::EDGetTokenT<MuonList>      _muonsrc;
   const edm::EDGetTokenT<ElectronList>  _electronsrc;
   edm::Handle<JetList>      _jetHandle;
   edm::Handle<MuonList>     _muonHandle;
   edm::Handle<ElectronList> _electronHandle;
};

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------

JetProducer::JetProducer(const edm::ParameterSet& iConfig):
   _jetsrc( consumes<JetList>(iConfig.getParameter<edm::InputTag>("jetsrc"))),
   _muonsrc( consumes<MuonList>(iConfig.getParameter<edm::InputTag>("muonsrc"))),
   _electronsrc(consumes<ElectronList>(iConfig.getParameter<edm::InputTag>("electronsrc")))
{
   produces<JetList> ();
}

JetProducer::~JetProducer()
{
}

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
bool JetProducer::filter( edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   iEvent.getByToken( _jetsrc , _jetHandle );
   iEvent.getByToken( _muonsrc , _muonHandle );
   iEvent.getByToken( _electronsrc , _electronHandle );

   std::auto_ptr<JetList> selectedJets( new JetList );

   unsigned num_bjets = 0;
   for( const auto& jet : *(_jetHandle.product())){
      if( IsSelectedJet(jet)  ){
         selectedJets->push_back(jet);
         if( jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.86 ){
            ++num_bjets;
         }
      }
   }

   // Jet Selection criteria
   if( selectedJets->size() < 6 ){ return false; }
   if( num_bjets < 1 ){ return false; }

   for( unsigned i = 0 ; i < selectedJets->size() ; ++i ){
      if( selectedJets->at(i).eta() > 2.4 ){
         std::cout << "Weird Jet Found!" << std::endl;}
      AddJetVariables( selectedJets->at(i) );
   }
   iEvent.put( selectedJets );

   return true;
}

//------------------------------------------------------------------------------
//   Selection Criterias
//------------------------------------------------------------------------------
bool JetProducer::IsSelectedJet( const pat::Jet& jet ) const
{
   if( jet.pt() < 30. ) { return false; }
   if( fabs(jet.eta()) > 2.4 ) { return false; }

   // Jet ID Selection
   if( jet.neutralHadronEnergyFraction() > 0.99 ){ return false; }
   if( jet.neutralEmEnergyFraction() > 0.99 ) { return false; }
   if( jet.numberOfDaughters() <=1 ){ return false; }
   if( abs(jet.eta()) < 2.4 ){
      if( jet.chargedHadronEnergyFraction() <= 0 ){ return false; }
      if( jet.chargedMultiplicity()         <= 0 ){ return false; }
      if( jet.chargedEmEnergyFraction()   > 0.99 ){ return false; }
   }

   //Cleaning against leptons
   for( const auto& mu : *(_muonHandle.product())){
      if( reco::deltaR(mu,jet) < 0.4 ) {
         return false; }
   }
   for( const auto& el : *(_electronHandle.product()) ){
      if( reco::deltaR(el,jet) < 0.4 ) {
         return false; }
   }
   
   return true;
}


//------------------------------------------------------------------------------
//   EDM Plugin requirements
//------------------------------------------------------------------------------

void
JetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
   //The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(JetProducer);

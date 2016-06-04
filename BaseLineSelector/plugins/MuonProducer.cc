#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "TstarAnalysis/BaseLineSelector/interface/ObjectCache.hh"
#include <vector>

typedef std::vector<pat::Muon> MuonList;

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class MuonProducer : public edm::stream::EDFilter<> {
public:
   explicit MuonProducer(const edm::ParameterSet&);
   ~MuonProducer();
   static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   // Public for use !
   bool IsSelectedMuon( const pat::Muon& , const reco::Vertex& ) const ;
   bool IsVetoMuon( const pat::Muon& ) const;
private:
   virtual bool filter(edm::Event&, const edm::EventSetup&) override;

   // Data Members
   const edm::EDGetTokenT<reco::VertexCollection> _vertexsrc;
   const edm::EDGetTokenT<MuonList>               _muonsrc;
   const edm::EDGetTokenT<pat::PackedCandidateCollection> _packedsrc;
   edm::Handle<reco::VertexCollection>         _vertexHandle;
   edm::Handle<pat::PackedCandidateCollection> _packedHandle;
   edm::Handle<MuonList>                       _muonHandle;

   //Helper private functions
   const reco::Vertex GetPrimaryVertex() const;
};

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------

MuonProducer::MuonProducer(const edm::ParameterSet& iConfig):
   _vertexsrc( consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexsrc"))),
   _muonsrc( consumes<MuonList>(iConfig.getParameter<edm::InputTag>("muonsrc"))),
   _packedsrc( consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedsrc")))
{
   produces<MuonList>();
}

MuonProducer::~MuonProducer()
{
}

//------------------------------------------------------------------------------
//   Main Control Flow
//------------------------------------------------------------------------------
bool MuonProducer::filter( edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   iEvent.getByToken( _muonsrc   , _muonHandle   );
   iEvent.getByToken( _vertexsrc , _vertexHandle );
   iEvent.getByToken( _packedsrc , _packedHandle );

   std::auto_ptr<MuonList> selectedMuons( new MuonList );

   // Object selection
   reco::Vertex pv = GetPrimaryVertex();
   for( const auto& mu : *(_muonHandle.product()) ) {
      bool isSelected = IsSelectedMuon( mu, pv );
      bool isVeto = IsVetoMuon(mu);
      if( isSelected ){
         if( selectedMuons->empty() ){
            selectedMuons->push_back( mu );
         } else{
            return false;
         }
      } else if( isVeto ){
         return false;
      }
   }

   // Adding additional variables
   if( selectedMuons->size() == 1 ){
      AddMuonVariables(
         selectedMuons->at(0),
         pv,
         _packedHandle );
   } else if( selectedMuons->size() > 1 ){
      return false;
   }

   iEvent.put( selectedMuons );
   return true;
}

//------------------------------------------------------------------------------
//   Selection Criterias
//------------------------------------------------------------------------------
const reco::Vertex MuonProducer::GetPrimaryVertex() const
{
   for( const auto& vertex : *(_vertexHandle.product())){
      if( vertex.isFake()      ) { continue; }
      if( vertex.ndof() <   4. ) { continue; }
      if( vertex.z()    >= 24. ) { continue; }
      if( vertex.position().Rho() >= 2.0) { continue; }
      return vertex;
   }
   return *(_vertexHandle->begin());
}

bool MuonProducer::IsSelectedMuon( const pat::Muon& mu, const reco::Vertex& vtx ) const
{
   if( !mu.isTightMuon(vtx) ) { return false; }
   if( mu.pt()  < 30  ) { return false; }
   if( fabs(mu.eta()) > 2.1 ) { return false; }
   if( mu.trackIso()/mu.pt() > 0.05 ){ return false; }
   return true;
}

bool MuonProducer::IsVetoMuon( const pat::Muon& mu ) const
{
   if( !mu.isLooseMuon() ) { return false; }
   if( mu.pt() < 15.0 )    { return false; }
   if( fabs(mu.eta()) > 2.4 ) { return false; }
   if( mu.trackIso()/mu.pt() > 0.10 ){ return false; }
   return true;
}




//------------------------------------------------------------------------------
//   EDM Plugin requirements
//------------------------------------------------------------------------------
void MuonProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions)
{
   //The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonProducer);

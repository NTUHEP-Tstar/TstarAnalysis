/*******************************************************************************
 *
 *  Filename    : MuonProducer.cc
 *  Description : Defining Direct EDM file interaction functions
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/BaseLineSelector/interface/MuonProducer.hpp"

MuonProducer::MuonProducer(const edm::ParameterSet& iConfig):
   _rhosrc    ( consumes<double>                (iConfig.getParameter<edm::InputTag>("rhosrc"))),
   _vertexsrc ( consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertexsrc"))),
   _packedsrc ( consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedsrc"))),
   _muonsrc   ( consumes<MuonList>              (iConfig.getParameter<edm::InputTag>("muonsrc"))),
   _hltsrc    ( consumes<edm::TriggerResults>   (iConfig.getParameter<edm::InputTag>("hltsrc"))),
   _triggerobjsrc( consumes<pat::TriggerObjectStandAloneCollection>(iConfig.getParameter<edm::InputTag>("trgobjsrc"))),
   _reqtrigger  ( iConfig.getParameter<std::string>("reqtrigger"))
{
   produces<MuonList>();
}

MuonProducer::~MuonProducer()
{
}

bool MuonProducer::filter( edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   iEvent.getByToken( _rhosrc    , _rhoHandle    );
   iEvent.getByToken( _muonsrc   , _muonHandle   );
   iEvent.getByToken( _vertexsrc , _vertexHandle );
   iEvent.getByToken( _packedsrc , _packedHandle );
   iEvent.getByToken( _hltsrc    , _hltHandle    );
   iEvent.getByToken( _triggerobjsrc, _triggerObjectHandle );

   std::auto_ptr<MuonList> selectedMuons( new MuonList );

   // Object selection
   if( !GetPrimaryVertex() ) { return false; }  // early exit if cannot find primary vertex

   for( auto mu : *_muonHandle ) { // Using duplicate
      bool isSelected = IsSelectedMuon(mu,iEvent);
      bool isVeto     = IsVetoMuon(mu,iEvent);
      if( isSelected ){
         if( selectedMuons->empty() ){
            selectedMuons->push_back(mu);
         } else{
            return false;
         }
      } else if( isVeto ){
         return false;
      }
   }

   if( selectedMuons->size() == 1 ){
      AddMuonVariables( selectedMuons->at(0) , iEvent );
   } else if( selectedMuons->size() > 1 ){
      return false;
   }

   iEvent.put( selectedMuons );
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

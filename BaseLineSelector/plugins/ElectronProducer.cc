#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "TstarAnalysis/BaseLineSelector/interface/ObjectCache.hh"

#include <vector>
#include <iostream>

typedef std::vector<pat::Electron> ElectronList;

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class ElectronProducer : public edm::stream::EDFilter<> {
public:
   explicit ElectronProducer(const edm::ParameterSet&);
   ~ElectronProducer();
   static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   // Public selection functions
   bool IsSelectedElectron( const pat::Electron&  ) const ;
   bool IsVetoElectron( const pat::Electron& ) const ;

private:
   virtual bool filter(edm::Event&, const edm::EventSetup&) override;

   // Data Members
   const edm::EDGetToken  _electronsrc    ;
   const edm::EDGetToken  _vetoMapToken   ;
   const edm::EDGetToken  _looseMapToken  ;
   const edm::EDGetToken  _mediumMapToken ;
   const edm::EDGetToken  _tightMapToken  ;
   const edm::EDGetToken  _packedsrc      ;
   edm::Handle<ElectronList>         _electronHandle;
   edm::Handle<pat::PackedCandidateCollection> _packedHandle;
   edm::Handle<edm::ValueMap<bool>>  _vetoMapHandle;
   edm::Handle<edm::ValueMap<bool>>  _looseMapHandle;
   edm::Handle<edm::ValueMap<bool>>  _mediumMapHandle;
   edm::Handle<edm::ValueMap<bool>>  _tightMapHandle;

};

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
ElectronProducer::ElectronProducer(const edm::ParameterSet& iConfig):
   _electronsrc( consumes<ElectronList>(iConfig.getParameter<edm::InputTag>("electronsrc"))),
   _vetoMapToken( consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("vetoMap"))),
   _looseMapToken( consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("looseMap"))),
   _mediumMapToken( consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("mediumMap"))),
   _tightMapToken( consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("tightMap"))),
   _packedsrc( consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedsrc")))
{
   produces<ElectronList>();
}

ElectronProducer::~ElectronProducer()
{
}

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
bool ElectronProducer::filter( edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   iEvent.getByToken( _electronsrc , _electronHandle );
   iEvent.getByToken( _vetoMapToken , _vetoMapHandle );
   iEvent.getByToken( _looseMapToken , _looseMapHandle );
   iEvent.getByToken( _mediumMapToken , _mediumMapHandle );
   iEvent.getByToken( _tightMapToken , _tightMapHandle );
   iEvent.getByToken( _packedsrc , _packedHandle );

   // Making a non-const copy
   ElectronList _tempList( *(_electronHandle.product()) );

   std::auto_ptr<ElectronList> selectedElectrons( new ElectronList );

   for( size_t i = 0 ; i < _electronHandle->size() ; ++i ){
      const edm::Ptr<pat::Electron> elPtr( _electronHandle, i );
      pat::Electron& el = _tempList[i];

      // Inserting as user int first to save read time
      el.addUserInt( "passVeto"  , (*_vetoMapHandle)[elPtr] );
      el.addUserInt( "passLoose" , (*_looseMapHandle)[elPtr] );
      el.addUserInt( "passMedium", (*_mediumMapHandle)[elPtr] );
      el.addUserInt( "passTight" , (*_tightMapHandle)[elPtr] );

      if( IsSelectedElectron(el) ){
         if( selectedElectrons->empty() ){
            selectedElectrons->push_back(el);
         } else{
            return false;
         }
      } else if( IsVetoElectron(el) ){
         return false;
      }
   }
   // Adding variable to final electron
   if( selectedElectrons->size() == 1 ){
      AddElectronVariables(
         selectedElectrons->at(0),
         _packedHandle
      );
   } else if( selectedElectrons->size() > 1 ){
      return false;
   }

   iEvent.put( selectedElectrons );

   return true;
}

//------------------------------------------------------------------------------
//   Selection Criterias
//------------------------------------------------------------------------------
bool ElectronProducer::IsSelectedElectron( const pat::Electron& el ) const
{
   if( !el.userInt("passLoose") ) { return false; }
   if( el.pt() < 30. ) { return false; }
   if( fabs(el.eta()) > 2.1 ){ return false; }
   if( fabs(el.eta()) > 1.44 && fabs(el.eta()) < 1.57 ) { return false; }
   return true;
}

bool ElectronProducer::IsVetoElectron( const pat::Electron& el ) const
{
   if( !el.userInt("passVeto") ) { return false; }
   if( el.pt() < 15. ) { return false; }
   if( fabs(el.eta()) > 2.4 ) { return false; }
   if( fabs(el.eta()) > 1.44 && fabs(el.eta()) < 1.57 ) { return false; }
   return true;
}


//------------------------------------------------------------------------------
//   EDM Plugin requirements
//------------------------------------------------------------------------------

void
ElectronProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
   //The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ElectronProducer);

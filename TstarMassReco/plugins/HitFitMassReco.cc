/*******************************************************************************
 *
 *  Filename    : HitFitMassReco.cc
 *  Description : Tstar Mass reconstruction algorithms
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include <vector>
#include <iostream>

#include "TstarAnalysis/TstarMassReco/interface/RecoResult.hh"
#include "TstarAnalysis/TstarMassReco/interface/HitFitter.hh"

typedef std::vector<pat::MET>      METList;
typedef std::vector<pat::Muon>     MuonList;
typedef std::vector<pat::Electron> ElectronList ;
typedef std::vector<pat::Jet>      JetList;

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class HitFitMassReco : public edm::EDProducer {
public:
   HitFitMassReco(const edm::ParameterSet& );
   virtual ~HitFitMassReco();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& );

   const edm::EDGetToken  _metsrc;
   const edm::EDGetToken  _muonsrc;
   const edm::EDGetToken  _elecsrc;
   const edm::EDGetToken  _jetsrc;
   edm::Handle<METList>      _metHandle;
   edm::Handle<MuonList>     _muonHandle;
   edm::Handle<ElectronList> _elecHandle;
   edm::Handle<JetList>      _jetHandle;

   HitFitter       _hitfitter;
};

//------------------------------------------------------------------------------
//   Constructor and Desctructor
//------------------------------------------------------------------------------
HitFitMassReco::HitFitMassReco( const edm::ParameterSet& iConfig ):
   _metsrc(  consumes<METList>     (iConfig.getParameter<edm::InputTag>("metsrc"))      ),
   _muonsrc( consumes<MuonList>    (iConfig.getParameter<edm::InputTag>("muonsrc"))     ),
   _elecsrc( consumes<ElectronList>(iConfig.getParameter<edm::InputTag>("electronsrc")) ),
   _jetsrc(  consumes<JetList>     (iConfig.getParameter<edm::InputTag>("jetsrc"))      ),
   _hitfitter(iConfig)
{
   produces<RecoResult>("HitFitResult").setBranchAlias("HitFitResult");
}

HitFitMassReco::~HitFitMassReco()
{}


//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void HitFitMassReco::produce( edm::Event& iEvent, const edm::EventSetup& )
{
   iEvent.getByToken( _metsrc  , _metHandle  );
   iEvent.getByToken( _muonsrc , _muonHandle );
   iEvent.getByToken( _elecsrc , _elecHandle );
   iEvent.getByToken( _jetsrc  , _jetHandle  );
   std::auto_ptr<RecoResult>     _hitfit( new RecoResult );

   const METList&  metList   = *(_metHandle.product() );
   const MuonList& muList    = *(_muonHandle.product());
   const ElectronList& elecList = *(_elecHandle.product());
   const JetList&  jetList   = *(_jetHandle);

   //----- HitFitter -----
   _hitfitter.ClearAll();
   _hitfitter.SetMET( &metList.front() );
   for( const auto& mu : muList ){
      _hitfitter.SetMuon( &mu );
   }
   for( const auto& el : elecList ){
      _hitfitter.SetElectron( &el );
   }
   for( const auto& jet : jetList ) {
      if( jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.86 ){
        _hitfitter.AddBTagJet( &jet );
      } else {
       _hitfitter.AddLightJet( &jet );
      }
   }
   _hitfitter.RunPermutations();
   *_hitfit = _hitfitter.BestResult();

   iEvent.put( _hitfit, "HitFitResult");
}

DEFINE_FWK_MODULE(HitFitMassReco);

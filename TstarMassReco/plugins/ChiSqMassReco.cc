/*******************************************************************************
*
*  Filename    : ChiSqMassReco.cc
*  Description : Tstar Mass reconstruction algorithms
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
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include <iostream>
#include <vector>

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"
#include "TstarAnalysis/TstarMassReco/interface/ChiSquareSolver.hpp"

typedef std::vector<pat::MET>      METList;
typedef std::vector<pat::Muon>     MuonList;
typedef std::vector<pat::Electron> ElectronList;
typedef std::vector<pat::Jet>      JetList;

// ------------------------------------------------------------------------------
//   Class Definition
// ------------------------------------------------------------------------------
class ChiSqMassReco : public edm::EDProducer
{
public:
   ChiSqMassReco( const edm::ParameterSet& );
   virtual
   ~ChiSqMassReco();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& );

   const edm::EDGetToken _metsrc;
   const edm::EDGetToken _muonsrc;
   const edm::EDGetToken _elecsrc;
   const edm::EDGetToken _jetsrc;
   edm::Handle<METList> _metHandle;
   edm::Handle<MuonList> _muonHandle;
   edm::Handle<ElectronList> _elecHandle;
   edm::Handle<JetList> _jetHandle;

   ChiSquareSolver _solver;
};

// ------------------------------------------------------------------------------
//   Constructor and Desctructor
// ------------------------------------------------------------------------------
ChiSqMassReco::ChiSqMassReco( const edm::ParameterSet& iConfig ) :
   _metsrc( GETTOKEN( iConfig, METList, "metsrc" ) ),
   _muonsrc( GETTOKEN( iConfig, MuonList, "muonsrc" ) ),
   _elecsrc( GETTOKEN( iConfig, ElectronList, "electronsrc" ) ),
   _jetsrc( GETTOKEN( iConfig, JetList, "jetsrc" ) ),
   _solver( iConfig )
{
   produces<RecoResult>( "ChiSquareResult" ).setBranchAlias( "ChiSquareResult" );
}

ChiSqMassReco::~ChiSqMassReco()
{}


// ------------------------------------------------------------------------------
//   Main control flow
// ------------------------------------------------------------------------------
void
ChiSqMassReco::produce( edm::Event& iEvent, const edm::EventSetup& )
{
   iEvent.getByToken( _metsrc,  _metHandle  );
   iEvent.getByToken( _muonsrc, _muonHandle );
   iEvent.getByToken( _elecsrc, _elecHandle );
   iEvent.getByToken( _jetsrc,  _jetHandle  );
   std::auto_ptr<RecoResult> chisq( new RecoResult );

   const METList& metList       = *( _metHandle.product() );
   const MuonList& muList       = *( _muonHandle.product() );
   const ElectronList& elecList = *( _elecHandle.product() );
   const JetList& jetList       = *( _jetHandle );

   // ----- ChiSquare solver -----
   _solver.SetIsData( iEvent.isRealData() );
   _solver.ClearAll();
   _solver.SetMET( &( metList.front() ) );

   for( const auto& mu : muList ){
      _solver.SetMuon( &mu );
   }

   for( const auto& el : elecList ){
      _solver.SetElectron( &el );
   }

   for( const auto& jet : jetList ){
      _solver.AddJet( &jet );
   }

   _solver.RunPermutations();
   *chisq = _solver.BestResult();

   iEvent.put( chisq, "ChiSquareResult" );
}

DEFINE_FWK_MODULE( ChiSqMassReco );

/*******************************************************************************
 *
 *  Filename    : ChiSqMassReco.cc
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
#include "TstarAnalysis/TstarMassReco/interface/ChiSquareSolver.hh"

typedef std::vector<pat::MET> METList;
typedef std::vector<pat::Muon>  MuonList;
typedef std::vector<pat::Electron> ElectronList ;
typedef std::vector<pat::Jet>   JetList;

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class ChiSqMassReco : public edm::EDProducer {
public:
   ChiSqMassReco(const edm::ParameterSet& );
   virtual ~ChiSqMassReco();

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

   ChiSquareSolver _solver;
};

//------------------------------------------------------------------------------
//   Constructor and Desctructor
//------------------------------------------------------------------------------
ChiSqMassReco::ChiSqMassReco( const edm::ParameterSet& iConfig ):
   _metsrc(  consumes<METList>     (iConfig.getParameter<edm::InputTag>("metsrc"))      ),
   _muonsrc( consumes<MuonList>    (iConfig.getParameter<edm::InputTag>("muonsrc"))     ),
   _elecsrc( consumes<ElectronList>(iConfig.getParameter<edm::InputTag>("electronsrc")) ),
   _jetsrc(  consumes<JetList>     (iConfig.getParameter<edm::InputTag>("jetsrc"))      ),
   _solver   (iConfig)
{
   produces<RecoResult>("ChiSquareResult").setBranchAlias("ChiSquareResult");
}

ChiSqMassReco::~ChiSqMassReco()
{}


//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void ChiSqMassReco::produce( edm::Event& iEvent, const edm::EventSetup& )
{
   iEvent.getByToken( _metsrc  , _metHandle  );
   iEvent.getByToken( _muonsrc , _muonHandle );
   iEvent.getByToken( _elecsrc , _elecHandle );
   iEvent.getByToken( _jetsrc  , _jetHandle  );
   std::auto_ptr<RecoResult>  chisq( new RecoResult );

   const METList&  metList   = *(_metHandle.product() );
   const MuonList& muList    = *(_muonHandle.product());
   const ElectronList& elecList = *(_elecHandle.product());
   const JetList&  jetList   = *(_jetHandle);

   //----- ChiSquare solver -----
   _solver.ClearAll();
   _solver.SetMET( &(metList.front()) );
   for( const auto& mu : muList ){
      _solver.SetLepton( &mu );
   }
   for( const auto& el : elecList ){
      _solver.SetLepton( &el );
   }
   for( const auto& jet : jetList ){
      _solver.AddJet( &jet );
   }
   _solver.RunPermutations();
   *chisq = _solver.BestResult();

   iEvent.put( chisq , "ChiSquareResult" );
}

DEFINE_FWK_MODULE(ChiSqMassReco);

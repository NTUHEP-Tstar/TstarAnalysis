/*******************************************************************************
 *
 *  Filename    : FakeMassReco.cc
 *  Description : Plugins for mixing jets from exisiting events
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "TRandom3.h"

using namespace std;
using namespace tstar;
//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class FakeMassReco : public edm::EDProducer {
public:
   FakeMassReco(const edm::ParameterSet& );
   virtual ~FakeMassReco();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& );

   const edm::EDGetToken    _recosrc;
   edm::Handle<RecoResult>  _recohandle;
   const int                _swap_count;
   vector<FitParticle>      _previous_list;
   TRandom3                 _rand;
};

//------------------------------------------------------------------------------
//   Constructor and Desctructor
//------------------------------------------------------------------------------
FakeMassReco::FakeMassReco( const edm::ParameterSet& iConfig ):
   _recosrc(  consumes<RecoResult>(iConfig.getParameter<edm::InputTag>("recosrc")) ),
   _swap_count( iConfig.getParameter<int>("swap") )
{
   produces<RecoResult>("FakeResult").setBranchAlias("FakeResult");

   // Generating first from random number generator
   for( int i = 0 ; i < _swap_count ; ++i  ){
      _previous_list.push_back( FitParticle() );
      double pt  = _rand.Uniform(0,250);
      double eta = _rand.Uniform(-1.5,1.5);
      double phi = _rand.Uniform(-3.14,+3.14);
      double e   = pt*asin(2*atan(exp(-eta)));
      _previous_list.back().P4(original).SetPtEtaPhiE( pt, eta, phi, e );
      _previous_list.back().P4(fitted)  .SetPtEtaPhiE( pt, eta, phi, e );
      _previous_list.back().P4(gen)     .SetPtEtaPhiE( pt, eta, phi, e );
   }
}

FakeMassReco::~FakeMassReco()
{
}

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void FakeMassReco::produce( edm::Event& iEvent, const edm::EventSetup& )
{

   iEvent.getByToken( _recosrc  , _recohandle  );
   std::auto_ptr<RecoResult>  fakeresult( new RecoResult );

   // Making fake result ;
   (*fakeresult) = (*_recohandle); // Duplicating result from the old one
   vector<Particle_Label> changelist ;
   for( int i = 0 ; i < _swap_count ; ++i  ){
      Particle_Label labeltochange = higgs_label ;
      while( labeltochange == higgs_label &&
             find(changelist.begin(), changelist.end(), labeltochange) != changelist.end() ){
         labeltochange = Particle_Label(11+_rand.Integer(7));
      } // Must skip over higgs_label and existing labels
      changelist.push_back( labeltochange );
   }

   // Pushing fake objects into result object
   cout << "Before copy: " << fakeresult->TstarMass() << endl;
   for( int i = 0 ; i < _swap_count ; ++i ){
      _previous_list[i].TypeFromFit() = changelist[i];
      fakeresult->GetParticle( changelist[i] ) = _previous_list[i];
   }
   fakeresult->TstarMass() = fakeresult->ComputeFromPaticleList();
   cout << "After copy: " << fakeresult->TstarMass() << endl;

   // Saving results to be used in next interation
   changelist.clear();
   for( int i = 0 ; i < _swap_count ; ++i  ){
      Particle_Label labeltochange = higgs_label ;
      while( labeltochange == higgs_label &&
         find(changelist.begin(), changelist.end(), labeltochange) != changelist.end() ){
         labeltochange = Particle_Label(11+_rand.Integer(7));
      } // Must skip over higgs_label and existing labels
      changelist.push_back( labeltochange );
      _previous_list[i] = _recohandle->GetParticle( labeltochange );
   }

   iEvent.put( fakeresult , "FakeResult" );
}

DEFINE_FWK_MODULE(FakeMassReco);

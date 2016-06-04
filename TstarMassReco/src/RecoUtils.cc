/*******************************************************************************
 *
 *  Filename    : RecoUtils.hh
 *  Description : Implementations for Reco utility functions
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/FitParticle.hh"
#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hh"

#include "TLorentzVector.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
Particle_Label GetJetType( const reco::GenParticle* );
Particle_Label GetLeptonType( const reco::GenParticle* );

//------------------------------------------------------------------------------
//   Main control flow functions
//------------------------------------------------------------------------------
TLorentzVector ConvertToRoot( const reco::Candidate& x )
{
   return TLorentzVector( x.px() , x.py(), x.pz(), x.energy() );
}

TLorentzVector ConvertToRoot( const CLHEP::HepLorentzVector& x )
{
   return TLorentzVector( x.px(), x.py(), x.pz(), x.e() );
}

FitParticle MakeResultJet( const pat::Jet* jet, int x )
{
   FitParticle ans ;
   ans.ObservedP4() = ConvertToRoot( *jet );
   ans.FittedP4()   = ConvertToRoot( *jet ); // Requires overwrite for HitFit
   ans.TypeFromFit()= (Particle_Label)(x);
   ans.IsBTagged()  = ( jet->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.86 ) ;

   if( jet->genParton() ){
      ans.GenP4() = ConvertToRoot( *(jet->genParton()) );
      ans.TypeFromTruth() = GetJetType( jet->genParton() );
      ans.TruthFlavour() = jet->hadronFlavour();
   } else {
      ans.GenP4() = ConvertToRoot( *jet );
      ans.TypeFromTruth() = unknown_label;
      ans.TruthFlavour()  = 0;
   }

   return ans;
}

FitParticle MakeResultMET( const pat::MET* met )
{
   FitParticle ans;
   ans.ObservedP4() = ConvertToRoot( *met ) ;
   ans.GenP4() = ConvertToRoot( *met );
   ans.TypeFromFit()   = neutrino_label;
   ans.TypeFromTruth() = unknown_label;
   ans.IsBTagged() = false;
   ans.TruthFlavour() = 0 ;
   return ans;
}

FitParticle MakeResultLepton( const reco::RecoCandidate* lepton )
{
   FitParticle ans;

   ans.ObservedP4() = ConvertToRoot( *lepton );
   ans.FittedP4()   = ConvertToRoot( *lepton );
   ans.TypeFromFit()= lepton_label;

   if( ((const pat::Electron*)(lepton))->genLepton() ){
      const reco::GenParticle* gen =  ((const pat::Electron*)(lepton))->genLepton();
      ans.GenP4() = ConvertToRoot( *gen );
      ans.TypeFromTruth() = GetLeptonType( gen ) ;
   } else if( ((const pat::Muon*)(lepton))->genLepton() ){
      const reco::GenParticle* gen =  ((const pat::Muon*)(lepton))->genLepton();
      ans.GenP4() = ConvertToRoot( *gen );
      ans.TypeFromTruth() = GetLeptonType( gen ) ;
   } else {
      ans.GenP4().SetPtEtaPhiE(0,0,0,0);
      ans.TypeFromTruth() = unknown_label;
   }
   ans.IsBTagged()    = false;
   ans.TruthFlavour() = 0 ;

   return ans;
}

//------------------------------------------------------------------------------
//   Topology Tracing functions
//------------------------------------------------------------------------------
#define BOTTOM_ID    5
#define TOP_QUARK_ID 6
#define ELEC_ID      11
#define MUON_ID      13
#define GLUON_ID     21
#define W_BOSON_ID   24
#define TSTAR_ID     9000005

Particle_Label GetJetType( const reco::GenParticle* x )
{
   if( FromLeptonicTop(x) ){
      if( abs(x->pdgId()) == BOTTOM_ID ){
         return lepb_label;
      } else {
         return unknown_label;
      }
   } else if( FromHadronicW(x) ){
      return hadw1_label;
   } else if( FromHadronicTop(x) ){
      if( abs(x->pdgId()) == BOTTOM_ID ){
         return hadb_label;
      } else {
         return unknown_label;
      }
   } else if( FromLeptonicTstar(x) ){
      if( abs(x->pdgId()) == GLUON_ID ){
         return lepg_label;
      } else {
         return unknown_label;
      }
   } else if( FromHadronicTstar(x) ){
      if( abs(x->pdgId()) == GLUON_ID ){
         return hadg_label;
      } else {
         return unknown_label;
      }
   } else {
      return unknown_label;
   }
   return unknown_label;
}

Particle_Label GetLeptonType( const reco::GenParticle* x )
{
   if( FromLeptonicW(x) ){
      if( abs(x->pdgId()) == ELEC_ID ){
         return lepton_label;
      } else if( abs(x->pdgId()) == MUON_ID ){
         return lepton_label;
      } else {
         return unknown_label;
      }
   } else {
      return unknown_label;
   }
}

//------------------------------------------------------------------------------
//   Decay Path Crawling algotihms
//------------------------------------------------------------------------------

// Using BFS to search mother and children
#include <queue>
using namespace std;

const reco::Candidate* GetDirectMother( const reco::Candidate* x , int target_ID )
{
   queue<const reco::Candidate*> bfs_queue;
   bfs_queue.push( x );
   while( !bfs_queue.empty() ){
      const reco::Candidate* temp = bfs_queue.front();
      bfs_queue.pop();
      if( abs(temp->pdgId()) == abs(target_ID) ){
         return temp;
      }
      for( unsigned i = 0 ; i < temp->numberOfMothers() ; ++i ){
         if( temp->mother(i)->pdgId() == temp->pdgId() ){
            bfs_queue.push( temp->mother(i) );
         } else if( abs(temp->mother(i)->pdgId()) == abs(target_ID) ){
            return temp->mother(i);
         }
      }
   }
   return NULL;
}

const reco::Candidate* GetDaughter( const reco::Candidate* x, int target_ID )
{
   queue<const reco::Candidate*> bfs_queue;
   bfs_queue.push( x );
   while( !bfs_queue.empty() ){
      const reco::Candidate* temp = bfs_queue.front();
      bfs_queue.pop();
      if( abs(temp->pdgId()) == abs(target_ID) ){
         // Moving to bottom of single decay chain
         while( temp->numberOfDaughters() == 1 ){
            temp = temp->daughter(0);
         }
         return temp;
      }
      for( unsigned i = 0 ; i < temp->numberOfDaughters() ; ++i ){
         bfs_queue.push( temp->daughter(i) );
      }
   }
   return NULL;
}

bool IsLeptonicW( const reco::Candidate* x )
{
   if( abs(x->pdgId()) != W_BOSON_ID ) { return false; }
   if( x->numberOfDaughters() != 2 ) { return false; }
   if( abs(x->daughter(0)->pdgId()) == ELEC_ID ||
       abs(x->daughter(0)->pdgId()) == MUON_ID ) { return true; }
   if( abs(x->daughter(1)->pdgId()) == ELEC_ID ||
       abs(x->daughter(1)->pdgId()) == MUON_ID ) { return true; }
   return false;
}

bool IsHadronicW( const reco::Candidate* x )
{
   if( abs(x->pdgId()) != W_BOSON_ID ) { return false; }
   if( x->numberOfDaughters() != 2 ) { return false; }
   if( abs(x->daughter(0)->pdgId()) >= 1 &&
       abs(x->daughter(0)->pdgId()) <= 6 &&
       abs(x->daughter(1)->pdgId()) >= 1 &&
       abs(x->daughter(1)->pdgId()) <= 6 ) return true;
   return false;
}

bool FromLeptonicTop( const reco::Candidate* x )
{
   const reco::Candidate* mother_top = GetDirectMother( x , TOP_QUARK_ID );
   if( mother_top == NULL ) { return false; }
   const reco::Candidate* decay_w = GetDaughter( mother_top, W_BOSON_ID );
   if( decay_w == NULL ) { return false; }
   if( IsLeptonicW(decay_w) ){ return true; }
   return false;
}

bool FromHadronicTop( const reco::Candidate* x )
{
   const reco::Candidate* mother_top = GetDirectMother( x , TOP_QUARK_ID );
   if( mother_top == NULL ) { return false; }
   const reco::Candidate* decay_w = GetDaughter( mother_top, W_BOSON_ID );
   if( decay_w == NULL ) { return false; }
   if( IsHadronicW(decay_w) ) { return true; }
   return false;
}

bool FromLeptonicW( const reco::Candidate* x )
{
   const reco::Candidate* mother_w = GetDirectMother( x , W_BOSON_ID );
   if( mother_w == NULL ) { return false; }
   if( IsLeptonicW(mother_w) ){ return true; }
   return false;
}

bool FromHadronicW( const reco::Candidate* x )
{
   const reco::Candidate* mother_w = GetDirectMother( x , W_BOSON_ID );
   if( mother_w == NULL ) { return false; }
   if( IsHadronicW(mother_w) ) { return true; }
   return false;
}

bool FromLeptonicTstar( const reco::Candidate* x )
{
   const reco::Candidate* mother_tstar = GetDirectMother( x , TSTAR_ID );
   if( mother_tstar == NULL ) { return false; }
   const reco::Candidate* decay_w = GetDaughter( mother_tstar, W_BOSON_ID );
   if( decay_w == NULL ) { return false; }
   if( IsLeptonicW(decay_w) ){ return true; }
   return false;
}

bool FromHadronicTstar( const reco::Candidate* x )
{
   const reco::Candidate* mother_tstar = GetDirectMother( x , TSTAR_ID );
   if( mother_tstar == NULL ) { return false; }
   const reco::Candidate* decay_w = GetDaughter( mother_tstar, W_BOSON_ID );
   if( decay_w == NULL ) { return false; }
   if( IsHadronicW(decay_w) ){ return true; }
   return false;
}

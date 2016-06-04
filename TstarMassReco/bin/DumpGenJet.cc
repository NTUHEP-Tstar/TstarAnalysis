/*******************************************************************************
 *
 *  Filename    : DumpGenJet.cc
 *  Description : Dumping gen jet and related topologies
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hh"

#include "TFile.h"

#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
   fwlite::Event event( TFile::Open("./test/signal.root") );
   fwlite::Handle<vector<reco::GenParticle>>  genHandle;
   fwlite::Handle<vector<pat::Jet>>  jetHandle;

   unsigned i = 0 ;
   for( event.toBegin() ; !event.atEnd() && i < 100 ; ++event, ++i ){
      cout << "Event [" << i << "]" << endl;
      genHandle.getByLabel( event, "prunedGenParticles" );
      jetHandle.getByLabel( event, "skimmedPatJets");
      const auto& gen_list = *(genHandle);
      const auto& jet_list = *(jetHandle);

      vector<const reco::Candidate*> gen_ptr_list ;
      for( const auto& gen : gen_list ){
         gen_ptr_list.push_back( &gen );
      }

      unsigned i = 0 ;
      for( const auto& jet : jet_list ){
         const reco::GenParticle* gen = jet.genParton();
         int this_idx = -1;
         int this_id  = 0 ;
         int mother_idx = -1;
         int mother_id  = 0 ;
         int from_had_w = 0;
         if( gen ){
            this_idx = find(gen_ptr_list.begin(),gen_ptr_list.end(),gen ) - gen_ptr_list.begin();
            this_id  = gen->pdgId();
            from_had_w = FromHadronicW( gen );
            if( gen->mother(0) ){
               mother_idx = find(gen_ptr_list.begin(),gen_ptr_list.end(),gen->mother(0) ) - gen_ptr_list.begin();
               mother_id  = gen->mother(0)->pdgId();
            }
         }

         printf(
            "[%u] %3d(%3d) | %3d(%3d) | %d\n" ,
            i,
            this_idx, this_id,
            mother_idx, mother_id,
            from_had_w
         );
         ++i ;
      }

   }
   return 0;
}

/*******************************************************************************
 *
 *  Filename    : DumpGenMap.cc
 *  Description : Dumping GenParticle topology in first 100 events of the file
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hh"

#include "TFile.h"

#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
   fwlite::Event event( TFile::Open("./test/signal.root") );
   fwlite::Handle<vector<reco::GenParticle>>  genHandle;

   unsigned i = 0 ;
   for( event.toBegin() ; !event.atEnd() && i < 100 ; ++event, ++i ){
      cout << "Event [" << i << "]" << endl;
      genHandle.getByLabel( event, "prunedGenParticles" );
      const auto& gen_list = *(genHandle);

      vector<const reco::Candidate*> gen_ptr_list ;
      for( const auto& gen : gen_list ){
         gen_ptr_list.push_back( &gen );
      }

      unsigned j = 0;
      for( const auto& gen : gen_list ){
         if( j > 120 ) { break; }
         ++j;
         if( abs(gen.pdgId()) != 24 &&
             abs(gen.pdgId()) != 6  &&
             abs(gen.pdgId()) != 13 &&
             abs(gen.pdgId()) != 11 ){ continue; }
         int mother_idx = -1;
         int mother_id  = 0;
         int daughter1_idx = -1;
         int daughter1_id  = 0;
         int daughter2_idx = -1;
         int daughter2_id  = 0;

         if( find(gen_ptr_list.begin(),gen_ptr_list.end(),gen.mother(0)) != gen_ptr_list.end() ){
            mother_idx = find(gen_ptr_list.begin(),gen_ptr_list.end(),gen.mother(0)) - gen_ptr_list.begin();
            mother_id = gen_ptr_list[mother_idx]->pdgId();
         }
         if( find(gen_ptr_list.begin(),gen_ptr_list.end(),gen.daughter(0)) != gen_ptr_list.end() ){
            daughter1_idx = find(gen_ptr_list.begin(),gen_ptr_list.end(),gen.daughter(0)) - gen_ptr_list.begin();
            daughter1_id = gen_ptr_list[daughter1_idx]->pdgId();
         }
         if( find(gen_ptr_list.begin(),gen_ptr_list.end(),gen.daughter(1)) != gen_ptr_list.end() ){
            daughter2_idx = find(gen_ptr_list.begin(),gen_ptr_list.end(),gen.daughter(1)) - gen_ptr_list.begin();
            daughter2_id  = gen_ptr_list[daughter2_idx]->pdgId();
         }

         char type[7] ={"000000"};
         if( FromLeptonicTop  (&gen) ) { type[0] = '1' ;}
         if( FromHadronicTop  (&gen) ) { type[1] = '1' ;}
         if( FromLeptonicW    (&gen) ) { type[2] = '1' ;}
         if( FromHadronicW    (&gen) ) { type[3] = '1' ;}
         if( FromLeptonicTstar(&gen) ) { type[4] = '1' ;}
         if( FromHadronicTstar(&gen) ) { type[5] = '1' ;}

         fprintf(
            stdout , "[%2u] %10d | %10.4lf %10.4lf | %3lu %3d(%3d) | %3lu %3d(%3d) %3d(%3d) | %s\n",
            j-1, gen.pdgId(),
            gen.pt(), gen.mass(),
            gen.numberOfMothers(), mother_idx, mother_id,
            gen.numberOfDaughters(), daughter1_idx, daughter1_id,  daughter2_idx, daughter2_id,
            type
         );
      }
   }
   return 0;
}

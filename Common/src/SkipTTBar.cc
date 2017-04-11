/*******************************************************************************
*
*  Filename    : SkipTTBar.cc
*  Description : Skipping event for ttbar where M_tt > 700
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/FWLite/interface/Handle.h"

bool SkipTTbar( const fwlite::EventBase& ev )
{
  fwlite::Handle<std::vector<reco::GenParticle> > genhandle;
  const reco::GenParticle* top = nullptr;
  const reco::GenParticle* atop = nullptr;
  size_t i = 0 ;
  genhandle.getByLabel( ev, "prunedGenParticles" );

  for( const auto& gen : genhandle.ref() ){
    if( i > 20 ){ break; }  // should be in first 20 entries

    if( !top && gen.pdgId() == 6 && gen.numberOfMothers() >= 2  ){
      top = &gen;
    } else if( !atop && gen.pdgId() == -6 && gen.numberOfMothers() >= 2 ){
      atop = &gen;
    }

    if( top && atop ){ break; }
    ++i;
  }

  if( !top || !atop ){ return false; }//don't skip if top or antitop is not present;
  return (top->p4() + atop->p4()).M() > 700 ;
}

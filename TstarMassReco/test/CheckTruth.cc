/*******************************************************************************
*
*  Filename    : CheckTruth.cc
*  Description : Quick Comparing the MC truth and fitting results
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "TFile.h"
#include "TLorentzVector.h"

#include <stdio.h>

using namespace std;
using namespace tstar;

int
main( int argc, char* argv[] )
{
  fwlite::Event ev( TFile::Open( "test/edm_MassReco.root" ) );
  fwlite::Handle<RecoResult> reco_handle;

  for( ev.toBegin(); !ev.atEnd(); ++ev ){
    reco_handle.getByLabel( ev, "NoTopConstrain8j2b", "HitFitResult", "HitFitCompare" );
    const RecoResult& reco_result = *reco_handle;

    printf(
      "%lf | %d%d%d%d | %d%d%d%d | %lf %lf\n",
      reco_result.TstarMass(),
      reco_result.GetParticle( muon_label ).FitMatchTruth(),
      reco_result.GetParticle( neutrino_label ).FitMatchTruth(),
      reco_result.GetParticle( lepb_label ).FitMatchTruth(),
      reco_result.GetParticle( lepg_label ).FitMatchTruth(),
      reco_result.GetParticle( hadw1_label ).FitMatchTruth(),
      reco_result.GetParticle( hadw2_label ).FitMatchTruth(),
      reco_result.GetParticle( hadb_label ).FitMatchTruth(),
      reco_result.GetParticle( hadg_label ).FitMatchTruth(),

      reco_result.HadronicJet1().Pt() - reco_result.HadronicJet1().Pt(),
      reco_result.HadronicJet2().Pt() - reco_result.HadronicJet2().Pt()
      );
  }

  return 0;
}

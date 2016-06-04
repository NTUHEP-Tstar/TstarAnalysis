/*******************************************************************************
 *
 *  Filename    : CheckTruth.cc
 *  Description : Quick Comparing the MC truth and fitting results
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/RecoResult.hh"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"

#include "TFile.h"
#include "TLorentzVector.h"

#include <stdio.h>

using namespace std;

int main(int argc, char* argv[])
{
   fwlite::Event ev( TFile::Open("test/edm_MassReco.root") );
   fwlite::Handle<RecoResult>  reco_handle;

   for( ev.toBegin() ; !ev.atEnd() ; ++ev ){
      reco_handle.getByLabel( ev , "NoTopConstrain8j2b", "HitFitResult", "HitFitCompare" );
      const RecoResult& reco_result = *reco_handle;

      printf(
         "%lf | %d%d%d%d | %d%d%d%d | %lf %lf\n",
         reco_result.TstarMass(),
         reco_result.Lepton().FitMatchTruth(),
         reco_result.Neutrino().FitMatchTruth(),
         reco_result.LeptonicBJet().FitMatchTruth(),
         reco_result.LeptonicGluon().FitMatchTruth(),
         reco_result.HadronicJet1().FitMatchTruth(),
         reco_result.HadronicJet2().FitMatchTruth(),
         reco_result.HadronicBJet().FitMatchTruth(),
         reco_result.HadronicGluon().FitMatchTruth(),

         reco_result.HadronicJet1().FittedP4().Pt() - reco_result.HadronicJet1().ObservedP4().Pt(),
         reco_result.HadronicJet2().FittedP4().Pt() - reco_result.HadronicJet2().ObservedP4().Pt()
      );
   }
   return 0;
}

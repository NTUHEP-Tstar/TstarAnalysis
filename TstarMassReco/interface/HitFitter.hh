/*******************************************************************************
 *
 *  Filename    : HitFitter.cc
 *  Description : Class for communicating with HitFit package
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef __HITFITTER_CC__
#define __HITFITTER_CC__

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "TstarAnalysis/ModifiedHitFit/interface/Lepjets_Event.h"
#include "TstarAnalysis/ModifiedHitFit/interface/Top_Fit.h"

#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"
#include "TopQuarkAnalysis/TopHitFit/interface/Top_Decaykin.h"

#include "TstarAnalysis/TstarMassReco/interface/RecoResult.hh"

#include "TLorentzVector.h"
#include <vector>

#define ISMUON true
#define ISELECTRON false

class HitFitter
{
public:
   HitFitter( const edm::ParameterSet& );
   virtual ~HitFitter ();

   void SetMET( const pat::MET* );
   void SetMuon( const pat::Muon* );
   void SetElectron( const pat::Electron* );
   void AddBTagJet(  const pat::Jet* );
   void AddLightJet( const pat::Jet* );

   void  RunPermutations();
   void  ClearAll();

   const RecoResult& BestResult() const;

private:

   // Storing objects
   const unsigned _max_jets;
   const unsigned _max_required_b_jet;
   const pat::MET*      _met;
   const pat::Muon*     _muon;
   const pat::Electron* _elec;
   std::vector<const pat::Jet*>  _btagJetList;
   std::vector<const pat::Jet*>  _lightJetList;

   std::vector<RecoResult> _results_list;

   hitfit::Top_Fit*                _top_fitter;
   hitfit::Resolution*             _met_KtResolution;    // Constant for the time being
   hitfit::EtaDepResolution*       _electronResolution;  // Read from external file
   hitfit::EtaDepResolution*       _muonResolution;      // Read from external file
   hitfit::EtaDepResolution*       _lightJetResolution;  // Read from external file
   hitfit::EtaDepResolution*       _bJetResolution;      // Read from external file

   unsigned _debug;

   //----- Translator functions  --------------------------------------------------
   void SetHitFitTemplate( hitfit::Lepjets_Event& ) const;
   void AddHitFitJet( hitfit::Lepjets_Event&, const pat::Jet* , const int) const;
   void AddResult(
      const double,
      const double,
      const hitfit::Lepjets_Event&); // Converting results to RecoResult

   //----- B Tag checking functions  ----------------------------------------------
   bool CheckBTagOrder( const std::vector<int>& ) const;
};

#endif // __HITFITTER_CC__

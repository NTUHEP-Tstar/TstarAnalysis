/*******************************************************************************
 *
 *  Filename    : RecoUtils.hh
 *  Description : Utility functions for converting CMSSW object to fitting results
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/

#ifndef __RECO_UTILS_HH__
#define __RECO_UTILS_HH__

#include "TLorentzVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "TstarAnalysis/TstarMassReco/interface/FitParticle.hh"

//------------------------------------------------------------------------------
//   Template functions
//------------------------------------------------------------------------------
extern TLorentzVector ConvertToRoot( const reco::Candidate& x );
extern TLorentzVector ConvertToRoot( const CLHEP::HepLorentzVector& x );

//------------------------------------------------------------------------------
//   Translation Functions in RecoUtils.cc
//------------------------------------------------------------------------------
extern FitParticle MakeResultJet( const pat::Jet*, int );
extern FitParticle MakeResultMET( const pat::MET* );
extern FitParticle MakeResultLepton( const reco::RecoCandidate*  );

//------------------------------------------------------------------------------
//   Decay chain crawling functions
//------------------------------------------------------------------------------
const reco::Candidate* GetDirectMother( const reco::Candidate*, int);
const reco::Candidate* GetDaughter( const reco::Candidate*, int );
extern bool FromLeptonicTop( const reco::Candidate* );
extern bool FromHadronicTop( const reco::Candidate* );
extern bool FromLeptonicW( const reco::Candidate* );
extern bool FromHadronicW( const reco::Candidate* );
extern bool FromLeptonicTstar( const reco::Candidate* );
extern bool FromHadronicTstar( const reco::Candidate* );



#endif /* end of include guard: __RECO_UTILS_HH__ */

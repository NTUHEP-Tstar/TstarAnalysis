/*******************************************************************************
*
*  Filename    : RecoUtils.hh
*  Description : Utility functions for converting CMSSW object to fitting results
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/

#ifndef TSTARANALYSIS_TSTARMASSRECO_RECOUTILS_HPP
#define TSTARANALYSIS_TSTARMASSRECO_RECOUTILS_HPP

#include "CLHEP/Vector/LorentzVector.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "TLorentzVector.h"

#include "TstarAnalysis/RootFormat/interface/FitParticle.hpp"

#include <vector>

/*******************************************************************************
*   Helper functions to extract Four momentum from non PAT objects
*******************************************************************************/
extern TLorentzVector ConvertToRoot( const reco::Candidate& x );
extern TLorentzVector ConvertToRoot( const CLHEP::HepLorentzVector& x );

/*******************************************************************************
*   Helper function for getting JES scale of MET from a list of Jets
*
*   -- Dumbed down version of algorithm (not complete!)
*      perform vector sum of jets for central
*      value, scaled up and scaled down four momentums, the error is then deprived
*      from the ration between the sum pt
*
*******************************************************************************/
void GetJESMET( const std::vector<const pat::Jet*>&, double& scaleup, double& scaledown  );

/*******************************************************************************
*   Translations functions, the creating of the storage fit particles from
*   original pat objects.
*
*   ** MakeResultJet - (int) to signify jet type in topology from fitting results
*
*   ** MakeResultMET
*       - TLorentzVector for fitted four momentum of neutrino.
*       - doubles for effects of Jet energy corrections.
*
*******************************************************************************/
extern FitParticle MakeResultJet( const pat::Jet*, int );
extern FitParticle MakeResultMET( const pat::MET*, const TLorentzVector&, const double scaleup, const double scaledown );
extern FitParticle MakeResultMuon( const pat::Muon* );
extern FitParticle MakeResultElectron( const pat::Electron* );

/*******************************************************************************
*   Decay chain crawling functions
*******************************************************************************/
const reco::Candidate* GetDirectMother( const reco::Candidate*, int );
const reco::Candidate* GetDaughter( const reco::Candidate*, int );

/*******************************************************************************
*   Decay chain crawling function for hihg level control flow
*******************************************************************************/
extern bool FromLeptonicTop( const reco::Candidate* );
extern bool FromHadronicTop( const reco::Candidate* );
extern bool FromLeptonicW( const reco::Candidate* );
extern bool FromHadronicW( const reco::Candidate* );
extern bool FromLeptonicTstar( const reco::Candidate* );
extern bool FromHadronicTstar( const reco::Candidate* );



#endif/* end of include guard: __RECO_UTILS_HH__ */

/*******************************************************************************
 *
 *  Filename    : FitParticle.cc
 *  Description : Implementations for the Fit Result particles
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/FitParticle.hh"

//------------------------------------------------------------------------------
//   Constructor and Destructor
//------------------------------------------------------------------------------
FitParticle::FitParticle():
   _fit_label(unknown_label),
   _truth_label(unknown_label),
   _b_tag(false),
   _flavour(0)
{}
FitParticle::~FitParticle() {}

//------------------------------------------------------------------------------
//   Direct Access Members
//------------------------------------------------------------------------------
TLorentzVector& FitParticle::ObservedP4()    { return _observed_p4 ; }
TLorentzVector& FitParticle::FittedP4()      { return _fitted_p4; }
TLorentzVector& FitParticle::GenP4()         { return _gen_p4; }
Particle_Label& FitParticle::TypeFromFit()   { return _fit_label; }
Particle_Label& FitParticle::TypeFromTruth() { return _truth_label; }

const TLorentzVector& FitParticle::ObservedP4()    const  { return _observed_p4; }
const TLorentzVector& FitParticle::FittedP4()      const  { return _fitted_p4; }
const TLorentzVector& FitParticle::GenP4()         const  { return _gen_p4; }
const Particle_Label& FitParticle::TypeFromFit()   const  { return _fit_label; }
const Particle_Label& FitParticle::TypeFromTruth() const  { return _truth_label; }

bool& FitParticle::IsBTagged() { return _b_tag; }
unsigned& FitParticle::TruthFlavour() { return _flavour; }


//------------------------------------------------------------------------------
//   Computation functions
//------------------------------------------------------------------------------
bool FitParticle::FitMatchTruth() const
{
   if( TypeFromTruth() == TypeFromFit() ) { return true; }
   if( TypeFromFit() == hadw1_label && TypeFromTruth() == hadw2_label ){ return true; }
   if( TypeFromFit() == hadw2_label && TypeFromTruth() == hadw1_label ){ return true; }
   return false;
}

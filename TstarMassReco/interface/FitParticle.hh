/*******************************************************************************
 *
 *  Filename    : FitParticle.hh
 *  Description : Particle Information After the fitting
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef __FIT_PARTICLE_HH__
#define __FIT_PARTICLE_HH__

#include "TLorentzVector.h"

//------------------------------------------------------------------------------
//   Label enum
//------------------------------------------------------------------------------
enum Particle_Label {
   lepton_label   = 1,
   neutrino_label = 2,
   lepb_label     = 11,
   hadb_label     = 12,
   hadw1_label    = 13,
   hadw2_label    = 14,
   higgs_label    = 15,
   lepg_label     = 16,
   hadg_label     = 17,

   unknown_label = 100
};

//------------------------------------------------------------------------------
//   Particle class declaration
//------------------------------------------------------------------------------
class FitParticle {
public:
   FitParticle();
   virtual ~FitParticle ();

   TLorentzVector& ObservedP4();
   TLorentzVector& FittedP4();
   TLorentzVector& GenP4();
   Particle_Label& TypeFromFit();
   Particle_Label& TypeFromTruth();
   const TLorentzVector& ObservedP4()    const;
   const TLorentzVector& FittedP4()      const;
   const TLorentzVector& GenP4()         const;
   const Particle_Label& TypeFromFit()   const;
   const Particle_Label& TypeFromTruth() const;

   bool& IsBTagged() ;
   unsigned& TruthFlavour();
   bool FitMatchTruth() const ;

private:
   TLorentzVector _observed_p4;
   TLorentzVector _fitted_p4;
   TLorentzVector _gen_p4;

   Particle_Label _fit_label;
   Particle_Label _truth_label;

   bool _b_tag;
   unsigned _flavour;

};

#endif /* end of include guard: __FIT_PARTICLE_HH__ */

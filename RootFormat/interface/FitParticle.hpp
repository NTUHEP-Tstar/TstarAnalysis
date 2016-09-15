/*******************************************************************************
*
*  Filename    : FitParticle.hpp
*  Description : FitParticle resulted from mass reconstruction
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_ROOTFORMAT_FITPARTICLE_HPP
#define TSTARANALYSIS_ROOTFORMAT_FITPARTICLE_HPP

#include "TLorentzVector.h"
#include <map>

// ------------------------------------------------------------------------------
//   Label enum
// ------------------------------------------------------------------------------
namespace tstar {

enum Particle_Label
{
   electron_label = 1,
   muon_label     = 2,
   neutrino_label = 5,
   lepb_label     = 11,
   hadb_label     = 12,
   hadw1_label    = 13,
   hadw2_label    = 14,
   higgs_label    = 15,
   lepg_label     = 16,
   hadg_label     = 17,

   unknown_label = 100
};

enum Momentum_Label
{
   original  = 1,
   fitted    = 2,
   gen       = 3,
   res_up    = 10,
   res_down  = 11,
   corr_up   = 20,
   corr_down = 21
};

}
// ------------------------------------------------------------------------------
//   Particle class declaration
// ------------------------------------------------------------------------------
class FitParticle
{
public:
   FitParticle();
   virtual
   ~FitParticle ();

   // Four momentum access functions,
   TLorentzVector&       P4( tstar::Momentum_Label x = tstar::original );
   const TLorentzVector& P4( tstar::Momentum_Label x = tstar::original ) const;


   tstar::Particle_Label&       TypeFromFit();
   tstar::Particle_Label&       TypeFromTruth();
   const tstar::Particle_Label& TypeFromFit()   const;
   const tstar::Particle_Label& TypeFromTruth() const;

   // Member function access
   bool&     IsBTagged();
   unsigned& TruthFlavour();
   bool      FitMatchTruth() const;

private:
   std::map<tstar::Momentum_Label, TLorentzVector> _p4map;

   tstar::Particle_Label _fit_label;
   tstar::Particle_Label _truth_label;

   bool _b_tag;
   unsigned _flavour;
};

#endif/* end of include guard: __FIT_PARTICLE_HH__ */

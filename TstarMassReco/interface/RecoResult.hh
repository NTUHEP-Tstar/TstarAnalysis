/*******************************************************************************
 *
 *  Filename    : RecoResult.hh
 *  Description : Master class for reconstruction results
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef __RECORESULTS_H__
#define __RECORESULTS_H__

#include "TLorentzVector.h"
#include "TstarAnalysis/TstarMassReco/interface/FitParticle.hh"

#include <vector>

// Forward declaring ChiSquareSolver class
class ChiSquareSolver;

class RecoResult {
public:
   RecoResult();
   virtual ~RecoResult ();

   // Granting friend access to Solver classes
   friend class ChiSquareSolver;
   friend class HitFitter;

   RecoResult& operator=( const RecoResult& );
   double TstarMass() const;
   double ChiSquare() const;

   void  AddParticle( const FitParticle& );
   const FitParticle& GetParticle( const Particle_Label& ) const;

   const FitParticle& Lepton()        const;
   const FitParticle& Neutrino()      const;
   const TLorentzVector LeptonicW()   const;
   const FitParticle& LeptonicBJet()  const;
   const TLorentzVector LeptonicTop() const;
   const FitParticle& LeptonicGluon() const;
   const TLorentzVector LeptonicTstar() const;

   const FitParticle& HadronicJet1()  const;
   const FitParticle& HadronicJet2()  const;
   const TLorentzVector HadronicW() const ;
   const FitParticle& HadronicBJet()  const;
   const TLorentzVector HadronicTop() const;
   const FitParticle& HadronicGluon() const;
   const TLorentzVector HadronicTstar() const ;

private:
   double _tstarMass;
   double _chiSquare;
   std::vector<FitParticle> _fitted_particle_list;
};

#endif /* end of include guard: __RECORESULTS_H__ */

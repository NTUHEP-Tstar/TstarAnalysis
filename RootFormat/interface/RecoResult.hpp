/*******************************************************************************
*
*  Filename    : RecoResult.hpp
*  Description : Class for storing the reconstruction results
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_ROOTFORMAT_RECORESULT_HPP
#define TSTARANALYSIS_ROOTFORMAT_RECORESULT_HPP

#include "TLorentzVector.h"
#include "TstarAnalysis/RootFormat/interface/FitParticle.hpp"

#include <vector>

class RecoResult
{
public:
   RecoResult();
   virtual
   ~RecoResult ();

   // Granting friend access to Solver classes
   friend class ChiSquareSolver;
   friend class HitFitter;

   RecoResult& operator=( const RecoResult& );
   double& TstarMass(){return _tstarMass; }
   double& ChiSquare(){return _chiSquare; }
   const double& TstarMass() const { return _tstarMass; }
   const double& ChiSquare() const { return _chiSquare; }

   double ComputeFromPaticleList( tstar::Momentum_Label x = tstar::fitted ) const;

   void               AddParticle( const FitParticle& );
   FitParticle&       GetParticle( const tstar::Particle_Label& );
   const FitParticle& GetParticle( const tstar::Particle_Label& ) const;

   std::vector<FitParticle>& ParticleList(){ return _fitted_particle_list; }
   const std::vector<FitParticle>& ParticleList() const { return _fitted_particle_list; }

   // Accessing particle four momentum only
   TLorentzVector Lepton( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector Neutrino( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector LeptonicW( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector LeptonicBJet( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector LeptonicTop( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector LeptonicGluon( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector LeptonicTstar( tstar::Momentum_Label x = tstar::fitted ) const;

   TLorentzVector HadronicJet1( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector HadronicJet2( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector HadronicW( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector HadronicBJet( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector HadronicTop( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector HadronicGluon( tstar::Momentum_Label x = tstar::fitted ) const;
   TLorentzVector HadronicTstar( tstar::Momentum_Label x = tstar::fitted ) const;

private:
   double _tstarMass;
   double _chiSquare;
   std::vector<FitParticle> _fitted_particle_list;
};

#endif/* end of include guard: __RECORESULTS_H__ */

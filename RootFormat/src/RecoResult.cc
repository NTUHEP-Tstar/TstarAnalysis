/*******************************************************************************
 *
 *  Filename    : RecoResult.cc
 *  Description : Implementation of RecoResult class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

using namespace tstar;
//------------------------------------------------------------------------------
//   Constructor and Desctructor
//------------------------------------------------------------------------------
RecoResult::RecoResult() {}
RecoResult::~RecoResult() {}
RecoResult& RecoResult::operator=( const RecoResult& x )
{
   _tstarMass = x._tstarMass;
   _chiSquare = x._chiSquare;
   _fitted_particle_list = x._fitted_particle_list;

   return *this;
}

void RecoResult::AddParticle( const FitParticle& x )
{
   _fitted_particle_list.push_back(x);
}

//------------------------------------------------------------------------------
//   Access Members
//------------------------------------------------------------------------------
static FitParticle __dummy_particle__;

const FitParticle& RecoResult::GetParticle( const Particle_Label& x ) const
{
   for( const auto& particle : _fitted_particle_list ){
      if( particle.TypeFromFit() == x ){
         return particle;
      }
   }
   return __dummy_particle__;
}

FitParticle& RecoResult::GetParticle( const Particle_Label& x )
{
   for( auto& particle : _fitted_particle_list ){
      if( particle.TypeFromFit() == x ){
         return particle;
      }
   }
   return __dummy_particle__;
}

TLorentzVector RecoResult::Lepton( Momentum_Label x ) const
{
   for( auto& particle : _fitted_particle_list ){
      if( particle.TypeFromFit() == electron_label ||
          particle.TypeFromFit() == muon_label ){
         return particle.P4(x);
      }
   }
   return TLorentzVector(0,0,0,0);
}

// Kienmatics by particle access
TLorentzVector RecoResult::Neutrino( Momentum_Label x ) const
{ return GetParticle( neutrino_label ).P4(x); }
TLorentzVector RecoResult::LeptonicBJet ( Momentum_Label x ) const
{ return GetParticle( lepb_label     ).P4(x); }
TLorentzVector RecoResult::LeptonicGluon( Momentum_Label x ) const
{ return GetParticle( lepg_label     ).P4(x); }
TLorentzVector RecoResult::HadronicJet1 ( Momentum_Label x ) const
{ return GetParticle( hadw1_label    ).P4(x); }
TLorentzVector RecoResult::HadronicJet2 ( Momentum_Label x ) const
{ return GetParticle( hadw2_label    ).P4(x); }
TLorentzVector RecoResult::HadronicBJet ( Momentum_Label x ) const
{ return GetParticle( hadb_label     ).P4(x); }
TLorentzVector RecoResult::HadronicGluon( Momentum_Label x ) const
{ return GetParticle( hadg_label     ).P4(x); }

// Extended Kinematics
TLorentzVector RecoResult::LeptonicW(Momentum_Label x) const
{ return Lepton(x) + Neutrino(x); }
TLorentzVector RecoResult::LeptonicTop(Momentum_Label x) const
{ return LeptonicW(x) + LeptonicBJet(x) ; }
TLorentzVector RecoResult::LeptonicTstar(Momentum_Label x) const
{ return LeptonicTop(x) + LeptonicGluon(x); }

TLorentzVector RecoResult::HadronicW(Momentum_Label x) const
{ return HadronicJet1(x) + HadronicJet2(x); }
TLorentzVector RecoResult::HadronicTop(Momentum_Label x) const
{ return HadronicW(x) + HadronicBJet(x); }
TLorentzVector RecoResult::HadronicTstar(Momentum_Label x) const
{ return HadronicTop(x) + HadronicGluon(x); }


double RecoResult::ComputeFromPaticleList(Momentum_Label x) const
{ return (HadronicTstar(x).M() + LeptonicTstar(x).M())/2.; }

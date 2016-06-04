/*******************************************************************************
 *
 *  Filename    : RecoResult.cc
 *  Description : Implementation of RecoResult class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/RecoResult.hh"

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
double RecoResult::TstarMass() const { return _tstarMass; }
double RecoResult::ChiSquare() const { return _chiSquare; }

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
const FitParticle& RecoResult::Lepton()        const { return GetParticle( lepton_label   ); }
const FitParticle& RecoResult::Neutrino()      const { return GetParticle( neutrino_label ); }
const FitParticle& RecoResult::LeptonicBJet()  const { return GetParticle( lepb_label     ); }
const FitParticle& RecoResult::LeptonicGluon() const { return GetParticle( lepg_label     ); }
const FitParticle& RecoResult::HadronicJet1()  const { return GetParticle( hadw1_label    ); }
const FitParticle& RecoResult::HadronicJet2()  const { return GetParticle( hadw2_label    ); }
const FitParticle& RecoResult::HadronicBJet()  const { return GetParticle( hadb_label     ); }
const FitParticle& RecoResult::HadronicGluon() const { return GetParticle( hadg_label     ); }

// Extended Kinematics
const TLorentzVector RecoResult::LeptonicW() const
{
   return Lepton().FittedP4() + Neutrino().FittedP4();
}
const TLorentzVector RecoResult::LeptonicTop() const
{
   return LeptonicW() + LeptonicBJet().FittedP4() ;
}
const TLorentzVector RecoResult::LeptonicTstar() const
{
   return LeptonicTop() + LeptonicGluon().FittedP4();
}

const TLorentzVector RecoResult::HadronicW() const
{
   return HadronicJet1().FittedP4() + HadronicJet2().FittedP4();
}
const TLorentzVector RecoResult::HadronicTop() const
{
   return HadronicW() + HadronicBJet().FittedP4();
}
const TLorentzVector RecoResult::HadronicTstar() const
{
   return HadronicTop() + HadronicGluon().FittedP4();
}

/*******************************************************************************
 *
 *  Filename    : FitParticle.cc
 *  Description : Implementations for the Fit Result particles
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/RootFormat/interface/FitParticle.hpp"
#include <iostream>
using namespace std;
using namespace tstar;
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
//   Four momentum access functions
//------------------------------------------------------------------------------
TLorentzVector& FitParticle::P4( Momentum_Label x )
{
   return _p4map[x];
}

const TLorentzVector& FitParticle::P4( Momentum_Label x ) const
{
   if( _p4map.find(x) != _p4map.end() ){
      try{
         return _p4map.at(x);
      } catch (std::exception e ){
         cerr << "Exception with _p4map.find(x) != _p4map.end() but _p4map.at(x) has error!" << endl;
         cerr << "_p4map.size() = " << _p4map.size() << endl;
         cerr << _p4map.find(x)->first << " " << _p4map.find(x)->second.Pt() << endl;
         return _p4map.at(original);
      }
   } else {
      try{
         return _p4map.at(original);
      } catch ( std::exception e ) {
         cerr << "No p4[original] found!" << endl;
         cerr << "Map size : " << _p4map.size() << endl;
         throw e;
      }
   }
}

//------------------------------------------------------------------------------
//   Direct Access Members
//------------------------------------------------------------------------------
Particle_Label& FitParticle::TypeFromFit()   { return _fit_label; }
Particle_Label& FitParticle::TypeFromTruth() { return _truth_label; }

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

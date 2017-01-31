/*******************************************************************************
*
*  Filename    : JetSelection.hpp
*  Description : Selection criteria implementation
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/JetProducer.hpp"

#include <iostream>
#include <random>

using namespace std;

/******************************************************************************/

bool
JetProducer::IsSelectedJet( const pat::Jet& jet, const bool isdata ) const
{
  // All selection should use the smear/scaled jet four momentum if it is not data
  TLorentzVector jetp4;

  if( isdata ){
    jetp4.MakeCorrected( jet.pt(), jet.eta(), jet.phi(), jet.energy() );
  } else if( IsWellMatched( jet ) ){
    jetp4 = MakeScaled( jet );
  } else {
    jetp4 = MakeSmeared( jet );
  }

  // actual jet selection
  if( jetp4.Pt() < 30. ){ return false; }
  if( fabs( jetp4.Eta() ) > 2.4 ){ return false; }

  // Jet ID Selection
  if( jet.neutralHadronEnergyFraction() > 0.99 ){ return false; }
  if( jet.neutralEmEnergyFraction()     > 0.99 ){ return false; }
  if( jet.numberOfDaughters() <= 1             ){ return false; }
  if( fabs( jetp4.Eta() ) < 2.4 ){
    if( jet.chargedHadronEnergyFraction() <= 0 ){ return false; }
    if( jet.chargedMultiplicity()         <= 0 ){ return false; }
    if( jet.chargedEmEnergyFraction()   > 0.99 ){ return false; }
  }

  // Cleaning against leptons
  for( const auto& mu : *_muonHandle ){
    const TLorentzVector mup4 = mgr::GetLorentzVector( mu, "" );  // getting default four momentum
    if( jetp4.DeltaR( mup4 ) < 0.4 ){ return false; }
  }

  for( const auto& el : *_electronHandle ){
    const TLorentzVector elp4 = mgr::GetLorentzVector( el, "" );  // getting default four momentum
    if( jetp4.DeltaR( elp4 ) < 0.4 ){ return false; }
  }

  return true;
}

/*****************************************************************************/

void
JetProducer::AddJetVariables( pat::Jet& jet, const bool is_data )
{
  // Caching jet correction uncertainty
  _jecunc->setJetPt( jet.pt() );
  _jecunc->setJetEta( jet.eta() );
  jet.addUserFloat( "jecunc", _jecunc->getUncertainty( true ) );

  // Caching resolution information
  JME::JetParameters jetparm;
  jetparm.setJetPt( jet.pt() ).setJetEta( jet.eta() ).setRho( *_rhoHandle );
  jet.addUserFloat( "respt",  _jetptres->getResolution( jetparm ) );
  jet.addUserFloat( "resphi", _jetphires->getResolution( jetparm ) );
  jet.addUserFloat( "ressf",  _jetressf->getScaleFactor( jetparm ) );

  if( is_data ){ return; }  // Early exit for data jets

  const TLorentzVector resjet =
    IsWellMatched( jet ) ? MakeScaled( jet ) : MakeSmeared( jet );
  mgr::AddLorentzVector( jet, resjet, "ResP4" );
}

/*****************************************************************************/

bool
JetProducer::IsWellMatched( const pat::Jet& jet ) const
{
  if( jet.genJet() == NULL ){ return false; } // Must have genJet

  const TLorentzVector jetp4( jet.px(), jet.py(), jet.pz(), jet.energy() );
  const TLorentzVector genp4( jet.genJet()->px(),
                              jet.genJet()->py(),
                              jet.genJet()->pz(),
                              jet.genJet()->energy() );
  const double res = GetJetPtRes( jet );

  if( jetp4.DeltaR( genp4 )         >= 0.4/2. ){ return false; }
  if( fabs( jetp4.Pt()-genp4.Pt() ) >= 3*res  ){ return false; }

  return true;
}


/******************************************************************************/

TLorentzVector
JetProducer::MakeScaled( const pat::Jet& jet ) const
{
  const TLorentzVector jetp4( jet.px(), jet.py(), jet.pz(), jet.energy() );
  const TLorentzVector genp4( jet.genJet()->px(),
                              jet.genJet()->py(),
                              jet.genJet()->pz(),
                              jet.genJet()->energy() );
  const double resscale = GetJetResScale( jet );
  const double newpt    =
    std::max( 0.0, genp4.Pt() + resscale*( jetp4.Pt()-genp4.Pt() ) );
  const double scale = newpt / jetp4.Pt();

  if( scale <= 0 ){
    std::cout << "Weird jet scaling found!" << std::endl;
  }

  return jetp4 * scale;
}

/*****************************************************************************/

// Hack for converting the jet phi to a unsigned seed with bitwise conversion
// Required for consistant jet smearing for the same jet across multiple calles
unsigned
bitconv( const float x )
{
  const void* temp = &x;
  return *( (const unsigned*)( temp ) );
}

/******************************************************************************/

TLorentzVector
JetProducer::MakeSmeared( const pat::Jet& jet ) const
{
  // Getting normal
  const double res   = GetJetPtRes( jet );
  const double ressf = GetJetResScale( jet );
  const double width = ressf > 1 ? sqrt( ressf*ressf-1 ) * res : 0;

  // Generating random number
  std::default_random_engine gen( bitconv( jet.phi() ) );
  std::normal_distribution<double> myrand( jet.pt(), width );
  const double newpt = myrand( gen );

  // Anonymouns namespace require (bug in gcc530)
  const double scale = newpt/jet.pt();
  const TLorentzVector jetp4( jet.px(), jet.py(), jet.pz(), jet.energy() );

  if( scale <= 0 || ::isnan( scale ) ){
    return jetp4;
  } else {
    return jetp4 * scale;
  }

}

/*****************************************************************************/

double
JetProducer::GetJetPtRes( const pat::Jet& jet ) const
{
  JME::JetParameters jetparm;
  jetparm.setJetPt( jet.pt() ).setJetEta( jet.eta() ).setRho( *_rhoHandle );
  return _jetptres->getResolution( jetparm );
}

/*****************************************************************************/

double
JetProducer::GetJetResScale( const pat::Jet& jet ) const
{
  JME::JetParameters jetparm;
  jetparm.setJetPt( jet.pt() ).setJetEta( jet.eta() ).setRho( *_rhoHandle );
  return _jetressf->getScaleFactor( jetparm );
}

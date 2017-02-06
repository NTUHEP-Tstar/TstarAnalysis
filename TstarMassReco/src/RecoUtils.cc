/*******************************************************************************
*
*  Filename    : RecoUtils.hh
*  Description : Implementations for Reco utility functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/RootFormat/interface/FitParticle.hpp"
#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hpp"

#include "ManagerUtils/PhysUtils/interface/BTagChecker.hpp"
#include "ManagerUtils/PhysUtils/interface/MCHelper.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"
#include "ManagerUtils/Common/interface/STLUtils.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"

#include "CLHEP/Vector/LorentzVector.h"
#include "TLorentzVector.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

/*******************************************************************************
*   Helper functions
*******************************************************************************/
static tstar::Particle_Label GetJetType( const reco::GenParticle* );
static tstar::Particle_Label GetLeptonType( const reco::GenParticle* );

/*******************************************************************************
*   Four momentum translation functions
*******************************************************************************/
TLorentzVector
ConvertToRoot( const reco::Candidate& x )
{
  return TLorentzVector( x.px(), x.py(), x.pz(), x.energy() );
}

/******************************************************************************/

TLorentzVector
ConvertToRoot( const CLHEP::HepLorentzVector& x )
{
  return TLorentzVector( x.px(), x.py(), x.pz(), x.e() );
}

/*******************************************************************************
*   GetJESMET functions
*******************************************************************************/
void
GetJESMET( const std::vector<const pat::Jet*>& jetlist, double& scaleup, double& scaledown )
{
  TLorentzVector centralp4;
  TLorentzVector scaleupp4;
  TLorentzVector scaledownp4;

  for( const auto& jet : jetlist ){
    centralp4   += mgr::GetLorentzVector( *jet, "ResP4" );
    scaleupp4   += mgr::GetLorentzVector( *jet, "ResP4" ) * ( 1+jet->userFloat( "jecunc" ) );
    scaledownp4 += mgr::GetLorentzVector( *jet, "ResP4" ) * ( 1-jet->userFloat( "jecunc" ) );
  }

  scaleup   = scaleupp4.Pt() / centralp4.Pt();
  scaledown = scaledownp4.Pt() / centralp4.Pt();
}


/*******************************************************************************
*   Particle creation functions
*******************************************************************************/
FitParticle
MakeResultJet( const pat::Jet* jet, int x )
{
  static mgr::BTagChecker bcheck( "check", mgr::CMSSWSrc() / "TstarAnalysis/EventWeight/data/CSVv2_ichep.csv" );

  FitParticle ans;
  ans.P4( tstar::original ) = mgr::GetLorentzVector( *jet, "ResP4" );
  // Making smeared jet momentums
  ans.P4( tstar::res_up   )  = ans.P4( tstar::original ) * ( 1. + jet->userFloat( "respt" ) );
  ans.P4( tstar::res_down )  = ans.P4( tstar::original ) * ( 1. - jet->userFloat( "respt" ) );
  ans.P4( tstar::corr_up  )  = ans.P4( tstar::original ) * ( 1. + jet->userFloat( "jecunc" ) );
  ans.P4( tstar::corr_down ) = ans.P4( tstar::original ) * ( 1. - jet->userFloat( "jecunc" ) );
  ans.TypeFromFit()          = (tstar::Particle_Label)( x );
  ans.IsBTagged()            = ( bcheck.PassMedium( *jet ) );

  if( jet->genParton() ){
    ans.P4( tstar::gen ) = ConvertToRoot( *( jet->genParton() ) );
    ans.TypeFromTruth()  = GetJetType( jet->genParton() );
    ans.TruthFlavour()   = jet->hadronFlavour();
  } else {
    ans.P4( tstar::gen ) = TLorentzVector( 0, 0, 0, 0 );
    ans.TypeFromTruth()  = tstar::unknown_label;
    ans.TruthFlavour()   = 0;
  }

  return ans;
}

/******************************************************************************/

FitParticle
MakeResultMET(
  const pat::MET*       met,
  const TLorentzVector& fittedP4,
  const double          scaleup,
  const double          scaledown )
{
  FitParticle ans;

  const double resup = met->shiftedPt( pat::MET::JetEnUp ) / met->pt();
  const double resdn = met->shiftedPt( pat::MET::JetEnDown ) / met->pt();

  ans.P4( tstar::fitted    ) = fittedP4;
  ans.P4( tstar::res_up    ) = fittedP4 * resup;
  ans.P4( tstar::res_down  ) = fittedP4 * resdn;
  ans.P4( tstar::corr_up   ) = fittedP4 * scaleup;
  ans.P4( tstar::corr_down ) = fittedP4 * scaledown;
  ans.P4( tstar::original  ) = mgr::GetLorentzVector( *met, "" );
  ans.TypeFromFit()          = tstar::neutrino_label;
  ans.TypeFromTruth()        = tstar::unknown_label;
  ans.IsBTagged()            = false;
  ans.TruthFlavour()         = 0;
  return ans;
}

/******************************************************************************/

FitParticle
MakeResultMuon( const pat::Muon* lepton )
{
  FitParticle ans;

  ans.P4( tstar::original ) = mgr::GetLorentzVector( *lepton, "" );
  ans.TypeFromFit()         = tstar::muon_label;

  if( lepton->genLepton() ){
    const reco::GenParticle* gen = lepton->genLepton();
    ans.P4( tstar::gen ) = ConvertToRoot( *gen );
    ans.TypeFromTruth()  = GetLeptonType( gen );
  } else {
    ans.P4( tstar::gen ) = TLorentzVector( 0, 0, 0, 0 );
    ans.TypeFromTruth()  = tstar::unknown_label;
  }
  ans.IsBTagged()    = false;
  ans.TruthFlavour() = 0;

  return ans;
}

/******************************************************************************/

FitParticle
MakeResultElectron( const pat::Electron* lepton )
{
  FitParticle ans;

  ans.P4( tstar::original ) = mgr::GetLorentzVector( *lepton, "" );
  ans.TypeFromFit()         = tstar::electron_label;

  if( lepton->genLepton() ){
    const reco::GenParticle* gen = lepton->genLepton();
    ans.P4( tstar::gen ) = ConvertToRoot( *gen );
    ans.TypeFromTruth()  = GetLeptonType( gen );
  } else {
    ans.P4( tstar::gen ) = TLorentzVector( 0, 0, 0, 0 );
    ans.TypeFromTruth()  = tstar::unknown_label;
  }
  ans.IsBTagged()    = false;
  ans.TruthFlavour() = 0;

  return ans;
}

/*******************************************************************************
*   Topology Tracing functions
*******************************************************************************/
#define TSTAR_ID     9000005

tstar::Particle_Label
GetJetType( const reco::GenParticle* x )
{
  using namespace tstar;

  if( FromHadronicW( x ) ){
    return hadw1_label;
  } else if( FromHadronicTop( x ) ){
    if( abs( x->pdgId() ) == mgr::PID::BOTTOM_QUARK_ID ){
      return hadb_label;
    } else {
      return unknown_label;
    }
  } else if( FromHadronicTstar( x ) ){
    if( abs( x->pdgId() ) == mgr::PID::GLUON_ID ){
      return hadg_label;
    } else {
      return unknown_label;
    }
  } else if( FromLeptonicTop( x ) ){
    if( abs( x->pdgId() ) == mgr::PID::BOTTOM_QUARK_ID ){
      return lepb_label;
    } else {
      return unknown_label;
    }
  } else if( FromLeptonicTstar( x ) ){
    if( abs( x->pdgId() ) == mgr::PID::GLUON_ID ){
      return lepg_label;
    } else {
      return unknown_label;
    }
  } else {
    return unknown_label;
  }
  return unknown_label;
}

/******************************************************************************/

tstar::Particle_Label
GetLeptonType( const reco::GenParticle* x )
{
  using namespace tstar;
  if( FromLeptonicW( x ) ){
    if( abs( x->pdgId() ) == mgr::PID::ELEC_ID ){
      return electron_label;
    } else if( abs( x->pdgId() ) == mgr::PID::MUON_ID ){
      return muon_label;
    } else {
      return unknown_label;
    }
  } else {
    return unknown_label;
  }
}

/******************************************************************************/

bool
IsLeptonicW( const reco::Candidate* x )
{
  if( abs( x->pdgId() ) != mgr::PID::W_BOSON_ID ){ return false; }
  if( x->numberOfDaughters() != 2 ){ return false; }
  if( abs( x->daughter( 0 )->pdgId() ) == mgr::PID::ELEC_ID ||
      abs( x->daughter( 0 )->pdgId() ) == mgr::PID::MUON_ID ){ return true; }
  if( abs( x->daughter( 1 )->pdgId() ) == mgr::PID::ELEC_ID ||
      abs( x->daughter( 1 )->pdgId() ) == mgr::PID::MUON_ID ){ return true; }
  return false;
}

/******************************************************************************/

bool
IsHadronicW( const reco::Candidate* x )
{
  if( abs( x->pdgId() ) != mgr::PID::W_BOSON_ID ){ return false; }
  if( x->numberOfDaughters() != 2 ){ return false; }
  if( abs( x->daughter( 0 )->pdgId() ) >= 1 &&
      abs( x->daughter( 0 )->pdgId() ) <= 6 &&
      abs( x->daughter( 1 )->pdgId() ) >= 1 &&
      abs( x->daughter( 1 )->pdgId() ) <= 6 ){ return true; }
  return false;
}

/******************************************************************************/

bool
FromLeptonicTop( const reco::Candidate* x )
{
  const reco::Candidate* mother_top = mgr::GetDirectMother( x, mgr::PID::TOP_QUARK_ID );
  if( mother_top == NULL ){ return false; }
  const reco::Candidate* decay_w = mgr::GetDaughter( mother_top, mgr::PID::W_BOSON_ID );
  if( decay_w == NULL ){ return false; }
  if( IsLeptonicW( decay_w ) ){ return true; }
  return false;
}

/******************************************************************************/

bool
FromHadronicTop( const reco::Candidate* x )
{
  const reco::Candidate* mother_top = mgr::GetDirectMother( x, mgr::PID::TOP_QUARK_ID );
  if( mother_top == NULL ){ return false; }
  const reco::Candidate* decay_w = mgr::GetDaughter( mother_top, mgr::PID::W_BOSON_ID );
  if( decay_w == NULL ){ return false; }
  if( IsHadronicW( decay_w ) ){ return true; }
  return false;
}

/******************************************************************************/

bool
FromLeptonicW( const reco::Candidate* x )
{
  const reco::Candidate* mother_w = mgr::GetDirectMother( x, mgr::PID::W_BOSON_ID );
  if( mother_w == NULL ){ return false; }
  if( IsLeptonicW( mother_w ) ){ return true; }
  return false;
}

/******************************************************************************/

bool
FromHadronicW( const reco::Candidate* x )
{
  const reco::Candidate* mother_w = mgr::GetDirectMother( x, mgr::PID::W_BOSON_ID );
  if( mother_w == NULL ){ return false; }
  if( IsHadronicW( mother_w ) ){ return true; }
  return false;
}

/******************************************************************************/

bool
FromLeptonicTstar( const reco::Candidate* x )
{
  const reco::Candidate* mother_tstar = mgr::GetDirectMother( x, TSTAR_ID );
  if( mother_tstar == NULL ){ return false; }
  const reco::Candidate* decay_w = mgr::GetDaughter( mother_tstar, mgr::PID::W_BOSON_ID );
  if( decay_w == NULL ){ return false; }
  if( IsLeptonicW( decay_w ) ){ return true; }
  return false;
}

/******************************************************************************/

bool
FromHadronicTstar( const reco::Candidate* x )
{
  const reco::Candidate* mother_tstar = mgr::GetDirectMother( x, TSTAR_ID );
  if( mother_tstar == NULL ){ return false; }
  const reco::Candidate* decay_w = mgr::GetDaughter( mother_tstar, mgr::PID::W_BOSON_ID );
  if( decay_w == NULL ){ return false; }
  if( IsHadronicW( decay_w ) ){ return true; }
  return false;
}

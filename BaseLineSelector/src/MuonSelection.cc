/*******************************************************************************
*
*  Filename    : MuonSelection.cc
*  Description : Implementation of muon selection and functions;
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedVars.hpp"
#include "ManagerUtils/PhysUtils/interface/TriggerMatching.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/MuonProducer.hpp"
#include "TstarAnalysis/Common/interface/IDCache.hpp"

/*******************************************************************************
*   Object selection criteria
*******************************************************************************/
bool
MuonProducer::GetPrimaryVertex()
{
  for( const auto& vertex : *_vertexHandle ){
    if( vertex.isFake() ){ continue; }
    if( vertex.ndof() <   4. ){ continue; }
    if( vertex.z()    >= 24. ){ continue; }
    if( vertex.position().Rho() >= 2.0 ){ continue; }
    _primary_vertex = vertex;
    return true;
  }

  return false;
}

/******************************************************************************/

bool
MuonProducer::IsSelectedMuon( const pat::Muon&  mu,
                              const edm::Event& iEvent ) const
{
  if( !mu.isTightMuon( _primary_vertex ) ){ return false; }
  if( mu.pt()             < 30.          ){ return false; }
  if( fabs( mu.eta() )    > 2.4          ){ return false; }
  if( mgr::MuPfIso( mu )  > mgr::MUPFISO_TIGHT ){ return false; }
  // Trigger selection moved to later, since it requires intensive study
  return true;
}

/******************************************************************************/

bool
MuonProducer::IsVetoMuon( const pat::Muon& mu, const edm::Event& ) const
{
  if( !mu.isLooseMuon() ){ return false; }
  if( mu.pt() < 15.0         ){ return false; }
  if( fabs( mu.eta() ) > 2.4 ){ return false; }
  if( mgr::MuPfIso( mu ) > mgr::MUPFISO_LOOSE ){ return false; }
  return true;
}

/*******************************************************************************
*   Variable caching
*******************************************************************************/
void
MuonProducer::AddMuonVariables( pat::Muon& mu,  const edm::Event& iEvent ) const
{
  // Additional ID values
  int state = 0;
  if( mu.isSoftMuon( _primary_vertex ) ){ state |= MUSOFTFLAG; }
  if( mu.isLooseMuon() ){ state |= MULOOSEFLAG; }
  if( mu.isMediumMuon() ){ state |= MUMEDIUMFLAG; }
  if( mu.isTightMuon( _primary_vertex ) ){ state |= MUTIGHTFLAG; }
  if( mu.isHighPtMuon( _primary_vertex ) ){ state |= MUHIGHTPTFLAG; }

  mu.addUserInt( MUIDVAR, state   );

  // See https://github.com/cmsb2g/B2GAnaFW/blob/master/src/MuonUserData.cc
  const double miniIso = mgr::PFMiniIsolation(
    _packedHandle,
    dynamic_cast<const reco::Candidate*>( &mu ),
    0.05,
    0.2,
    10.,
    false );
  mu.addUserFloat( "miniIso", miniIso );

}

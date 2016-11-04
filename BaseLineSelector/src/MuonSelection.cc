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
   if( mu.pt()        < 30.               ){ return false; }
   if( fabs( mu.eta() ) > 2.1             ){ return false; }
   if( MuPfIso( mu )  > MUPFISO_TIGHT     ){ return false; }
   // Trigger selection moved to later, since it requires intensive study
   return true;
}

/******************************************************************************/

bool
MuonProducer::IsVetoMuon( const pat::Muon& mu, const edm::Event& ) const
{
   if( !mu.isLooseMuon()      ){ return false; }
   if( mu.pt() < 15.0         ){ return false; }
   if( fabs( mu.eta() ) > 2.4 ){ return false; }
   if( MuPfIso( mu ) > MUPFISO_LOOSE ){ return false; }
   return true;
}

/*******************************************************************************
*   Variable caching
*******************************************************************************/
void
MuonProducer::AddMuonVariables( pat::Muon& mu,  const edm::Event& iEvent ) const
{
   // Additional ID values
   const bool isSoftMuon   = mu.isSoftMuon( _primary_vertex );
   const bool isLooseMuon  = mu.isLooseMuon();
   const bool isMediumMuon = mu.isMediumMuon();
   const bool isTightMuon  = mu.isTightMuon( _primary_vertex );
   const bool isHighPtMuon = mu.isHighPtMuon( _primary_vertex );

   mu.addUserInt( "isSoftMuon",   isSoftMuon   );
   mu.addUserInt( "isLooseMuon",  isLooseMuon  );
   mu.addUserInt( "isMediumMuon", isMediumMuon );
   mu.addUserInt( "isTightMuon",  isTightMuon  );
   mu.addUserInt( "isHighPtMuon", isHighPtMuon );

   // See https://github.com/cmsb2g/B2GAnaFW/blob/master/src/MuonUserData.cc
   const double miniIso = PFMiniIsolation(
      _packedHandle,
      dynamic_cast<const reco::Candidate*>( &mu ),
      0.05,
      0.2,
      10.,
      false );
   mu.addUserFloat( "miniIso", miniIso );

}

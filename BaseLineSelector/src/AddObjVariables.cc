/*******************************************************************************
 *
 *  Filename    : AddObjVaraibles.cc
 *  Description : Caching more complicated analysis objects into edm objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/BaseLineSelector/interface/ObjectCache.hh"

//------------------------------------------------------------------------------
//   Muon Variables
//------------------------------------------------------------------------------
void AddMuonVariables(
   pat::Muon& mu,
   const reco::Vertex& pv,
   const edm::Handle<pat::PackedCandidateCollection>& packedHandle )
{
   // Additional ID values
   const bool isSoftMuon   = mu.isSoftMuon(pv);
   const bool isLooseMuon  = mu.isLooseMuon();
   const bool isMediumMuon = mu.isMediumMuon();
   const bool isTightMuon  = mu.isTightMuon(pv);
   const bool isHighPtMuon = mu.isHighPtMuon(pv);

   mu.addUserInt("isSoftMuon"  , isSoftMuon   );
   mu.addUserInt("isLooseMuon" , isLooseMuon  );
   mu.addUserInt("isMediumMuon", isMediumMuon );
   mu.addUserInt("isTightMuon" , isTightMuon  );
   mu.addUserInt("isHighPtMuon", isHighPtMuon );

   // See https://github.com/cmsb2g/B2GAnaFW/blob/master/src/MuonUserData.cc
   double miniIso = GetPFMiniIsolation(
         packedHandle ,
         dynamic_cast<const reco::Candidate*>(&mu),
         0.05,
         0.2,
         10.,
         false );
   mu.addUserFloat( "miniIso" , miniIso );
}

//------------------------------------------------------------------------------
//   Electron Variables
//------------------------------------------------------------------------------
extern void AddElectronVariables(
   pat::Electron& el,
   const edm::Handle<pat::PackedCandidateCollection>& packedHandle )
{
   // See https://github.com/cmsb2g/B2GAnaFW/blob/master/src/MuonUserData.cc
   double miniIso = GetPFMiniIsolation(
         packedHandle ,
         dynamic_cast<const reco::Candidate*>(&el),
         0.05,
         0.2,
         10.,
         false );
   el.addUserFloat( "miniIso" , miniIso );
}

//------------------------------------------------------------------------------
//   Jet Variables
//------------------------------------------------------------------------------
extern void AddJetVariables( pat::Jet& )
{

}

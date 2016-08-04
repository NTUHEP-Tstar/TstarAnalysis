/*******************************************************************************
 *
 *  Filename    : AddObjVaraibles.cc
 *  Description : Caching more complicated analysis objects into edm objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/BaseLineSelector/interface/ObjectCache.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedVars.hpp"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

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
   double miniIso = PFMiniIsolation(
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
void AddElectronVariables(
   pat::Electron& el,
   const edm::Handle<pat::PackedCandidateCollection>& packedHandle )
{
   // See https://github.com/cmsb2g/B2GAnaFW/blob/master/src/MuonUserData.cc
   double miniIso = PFMiniIsolation(
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
void AddJetVariables(
   pat::Jet& jet,
   JetCorrectionUncertainty& jecunc, // Cannot be const!
   const JME::JetResolution& jetptres,
   const JME::JetResolution& jetphires,
   const JME::JetResolutionScaleFactor& jetressf
)
{
   // Caching jet correction uncertainty
   jecunc.setJetPt( jet.pt() );
   jecunc.setJetEta( jet.eta() );
   jet.addUserFloat( "jecunc", jecunc.getUncertainty(true));

   // Caching resolution information
   JME::JetParameters jetparm;
   jetparm.setJetPt( jet.pt() ).setJetEta( jet.eta() );
   jet.addUserFloat( "respt" , jetptres.getResolution(jetparm)  );
   jet.addUserFloat( "resphi", jetphires.getResolution(jetparm) );
   jet.addUserFloat( "ressf" , jetressf.getScaleFactor(jetparm) );

}

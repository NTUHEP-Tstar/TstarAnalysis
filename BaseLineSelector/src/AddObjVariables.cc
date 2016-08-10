/*******************************************************************************
 *
 *  Filename    : AddObjVaraibles.cc
 *  Description : Caching more complicated analysis objects into edm objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/BaseLineSelector/interface/ObjectCache.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedVars.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "TRandom3.h"
#include "TLorentzVector.h"
#include <cmath>
#include <algorithm>

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
bool             IsWellMatched( const pat::Jet& );
TLorentzVector   MakeScaled ( const pat::Jet& );
TLorentzVector   MakeSmeared( const pat::Jet& );

void AddJetVariables(
   pat::Jet& jet,
   const double    rho,
   JetCorrectionUncertainty& jecunc, // Cannot be const!
   const JME::JetResolution& jetptres,
   const JME::JetResolution& jetphires,
   const JME::JetResolutionScaleFactor& jetressf,
   const bool is_data
)
{
   // Caching jet correction uncertainty
   jecunc.setJetPt( jet.pt() );
   jecunc.setJetEta( jet.eta() );
   jet.addUserFloat( "jecunc", jecunc.getUncertainty(true));

   // Caching resolution information
   JME::JetParameters jetparm;
   jetparm.setJetPt( jet.pt() ).setJetEta( jet.eta() ).setRho(rho);
   jet.addUserFloat( "respt" , jetptres.getResolution(jetparm)  );
   jet.addUserFloat( "resphi", jetphires.getResolution(jetparm) );
   jet.addUserFloat( "ressf" , jetressf.getScaleFactor(jetparm) );

   TLorentzVector resjet;
   if( !is_data && IsWellMatched(jet) ){
      resjet = MakeScaled( jet );
   } else if( !is_data ) {
      resjet = MakeSmeared( jet );
   } else {
      resjet.SetPtEtaPhiE( jet.pt(), jet.eta(), jet.phi(), jet.energy() );
   }
   AddLorentzVector( jet, resjet, "ResP4" );
}


bool IsWellMatched( const pat::Jet& jet )
{
   if( jet.genJet() == NULL ) { return false; } // Must have gneJet
   const TLorentzVector jetp4( jet.px(), jet.py(), jet.pz(), jet.energy() );
   const TLorentzVector genp4( jet.genJet()->px(), jet.genJet()->py(), jet.genJet()->pz(), jet.genJet()->energy() );
   const double res = jet.userFloat("respt");

   if( jetp4.DeltaR(genp4)         > 0.4/2. ) { return false; }
   if( fabs(jetp4.Pt()-genp4.Pt()) > 3*res  ) { return false; }
   return true;
}

TLorentzVector MakeScaled( const pat::Jet& jet )
{
   const TLorentzVector jetp4( jet.px(), jet.py(), jet.pz(), jet.energy() );
   const TLorentzVector genp4( jet.genJet()->px(), jet.genJet()->py(), jet.genJet()->pz(), jet.genJet()->energy() );
   const double resscale = jet.userFloat("ressf");
   const double newpt = std::max(0.0, genp4.Pt() + resscale*(jetp4.Pt()-genp4.Pt() ));
   const double scale = newpt / jetp4.Pt();
   return jetp4 * scale;
}

TLorentzVector MakeSmeared( const pat::Jet& jet )
{
   static TRandom3 ran;
   const double res   = jet.userFloat("respt");
   const double ressf = jet.userFloat("ressf");
   const double width = sqrt( ressf*ressf -1 ) * res;

   const TLorentzVector jetp4( jet.px(), jet.py(), jet.pz(), jet.energy() );

   return jetp4 * ran.Gaus( 1 , width/jet.pt() ) ;
}

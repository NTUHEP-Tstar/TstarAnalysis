/*******************************************************************************
*
*  Filename    : ElectronSelection.cc
*  Description : Function related to object caching in selection
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedVars.hpp"
#include "ManagerUtils/PhysUtils/interface/TriggerMatching.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/ElectronProducer.hpp"

/*******************************************************************************
*   Selection Criterias
*******************************************************************************/
bool
ElectronProducer::IsSelectedElectron(
   pat::Electron&                 el,
   const edm::Ptr<pat::Electron>& elPtr,
   const edm::Event&              ev
   ) const
{
   if( !( ( *_looseMapHandle )[elPtr] ) ){ return false; }
   if( el.pt() < 53. ){ return false; }
   if( fabs( el.eta() ) > 2.1 ){ return false; }
   if( fabs( el.eta() ) > 1.44 && fabs( el.eta() ) < 1.57 ){ return false; }

   if( _reqtrigger != "" && !HasTriggerMatch(
          el,
          *_trigobjHandle,
          _reqtrigger,
          ev.triggerNames( *_hltHandle )
          ) ){ return false; }

   // Only caching for particles that have passed selection
   el.addUserInt( "passVeto",   ( *_vetoMapHandle   )[elPtr] );
   el.addUserInt( "passLoose",  ( *_looseMapHandle  )[elPtr] );
   el.addUserInt( "passMedium", ( *_mediumMapHandle )[elPtr] );
   el.addUserInt( "passTight",  ( *_tightMapHandle  )[elPtr] );
   return true;
}

/******************************************************************************/

bool
ElectronProducer::IsVetoElectron(
   const pat::Electron&           el,
   const edm::Ptr<pat::Electron>& elPtr,
   const edm::Event&              ev
   ) const
{
   if( !( ( *_vetoMapHandle )[elPtr] ) ){ return false; }
   if( el.pt() < 15. ){ return false; }
   if( fabs( el.eta() ) > 2.4 ){ return false; }
   if( fabs( el.eta() ) > 1.44 && fabs( el.eta() ) < 1.57 ){ return false; }
   return true;
}


/*******************************************************************************
*   Selection Criterias
*******************************************************************************/
void
ElectronProducer::AddElectronVariables(
   pat::Electron&    el,
   const edm::Event& iEvent
   ) const
{

   double miniIso = PFMiniIsolation(
      _packedHandle,
      dynamic_cast<const reco::Candidate*>( &el ),
      0.05,
      0.2,
      10.,
      false
      );
   el.addUserFloat( "miniIso", miniIso );

   if( _reqtrigger == "" ){ return; }// easrly exit if not required

   TLorentzVector trigp4 = TriggerMatchP4(
      el,
      *_trigobjHandle,
      _reqtrigger,
      iEvent.triggerNames( *_hltHandle )
      );

   AddLorentzVector( el, trigp4, "TrigP4" );

}

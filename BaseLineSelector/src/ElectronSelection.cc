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
#include "TstarAnalysis/Common/interface/IDCache.hpp"

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
  // Typical cuts
  if( !( ( *_tightMapHandle )[elPtr] ) ){ return false; }
  if( el.pt() < 30. ){ return false; }
  if( fabs( el.eta() ) > 2.1 ){ return false; }
  if( fabs( el.eta() ) > 1.44 && fabs( el.eta() ) < 1.57 ){ return false; }

  // Impact parameter cuts, using standard cuts
  if( fabs( el.eta() ) < 1.45  ){ //
    if( fabs( el.gsfTrack()->dxy( _vertexHandle->front().position() ) ) > 0.05 ){ return false; }
    if( fabs( el.gsfTrack()->dz( _vertexHandle->front().position() ) ) > 0.10 ){ return false; }
  } else {
    if( fabs( el.gsfTrack()->dxy( _vertexHandle->front().position() ) ) > 0.10 ){ return false; }
    if( fabs( el.gsfTrack()->dz( _vertexHandle->front().position() ) ) > 0.20 ){ return false; }
  }

  // Trigger object selection moved to later selection and not baseline

  // Only caching for particles that have passed selection
  int state = 0;
  if( ( *_vetoMapHandle   )[elPtr] ){ state |= ELEVETOFLAG;   }
  if( ( *_looseMapHandle  )[elPtr] ){ state |= ELELOOSEFLAG;  }
  if( ( *_mediumMapHandle )[elPtr] ){ state |= ELEMEDIUMFLAG; }
  if( ( *_tightMapHandle  )[elPtr] ){ state |= ELETIGHTFLAG;  }
  if( ( *_heepMapHandle   )[elPtr] ){ state |= ELEHEEPFLAG;   }
  if( ( *_hltMapHandle    )[elPtr] ){ state |= ELEHLTFLAG;    }
  el.addUserInt( ELEIDVAR, state );
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
  if( !( ( *_looseMapHandle )[elPtr] ) ){ return false; }
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

  double miniIso = mgr::PFMiniIsolation(
    _packedHandle,
    dynamic_cast<const reco::Candidate*>( &el ),
    0.05,
    0.2,
    10.,
    false
    );
  el.addUserFloat( "miniIso", miniIso );

}

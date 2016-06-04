/*******************************************************************************
 *
 *  Filename    : ObjectCache
 *  Description : Helper functions for computing Object caching
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  For implementations see the various files in the src directory
 *
*******************************************************************************/
#ifndef __OBJECTCACHE_HH__
#define __OBJECTCACHE_HH__

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

//------------------------------------------------------------------------------
//   Main Object part, see src/AddObjVariables.cc
//------------------------------------------------------------------------------
extern void AddMuonVariables(
   pat::Muon& ,
   const reco::Vertex&,
   const edm::Handle<pat::PackedCandidateCollection>&
);

extern void AddElectronVariables(
   pat::Electron&,
   const edm::Handle<pat::PackedCandidateCollection>&
);

extern void AddJetVariables( pat::Jet& );

//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
extern double GetPFMiniIsolation(
   const edm::Handle<pat::PackedCandidateCollection>& pfcands,
   const reco::Candidate* ptcl,
   const double r_iso_min,
   const double r_iso_max,
   const double kt_scale,
   const bool charged_only ); // in src/PFIsolation.cc


#endif /* end of include guard: __OBJECTCACHE_HH__ */

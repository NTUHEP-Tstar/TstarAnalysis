/*******************************************************************************
 *
 *  Filename    : ObjectCache
 *  Description : Helper functions for computing Object caching
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  For implementations see the various files in the src directory
 *
*******************************************************************************/
#ifndef TSTARANALYSIS_BASELINESELECTOR_OBJECTCACHE_HPP
#define TSTARANALYSIS_BASELINESELECTOR_OBJECTCACHE_HPP

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"

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

extern void AddJetVariables(
   pat::Jet& ,
   JetCorrectionUncertainty& jecunc, // Cannot be const!
   const JME::JetResolution& jetptres,
   const JME::JetResolution& jetphires,
   const JME::JetResolutionScaleFactor& jetressf
);



#endif /* end of include guard: __OBJECTCACHE_HH__ */

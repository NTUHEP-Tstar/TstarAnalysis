/*******************************************************************************
*
*  Filename    : TypeDef.hpp
*  Description : Quick Names for cleaner code
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_BASELINESELECTOR_TYPEDEF_HPP
#define TSTARANALYSIS_BASELINESELECTOR_TYPEDEF_HPP

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

namespace tstar {

typedef std::vector<pat::Jet>      JetList;
typedef std::vector<pat::Muon>     MuonList;
typedef std::vector<pat::Electron> ElectronList;

typedef pat::PackedCandidateCollection  PackedCandList;

typedef reco::VertexCollection  VertexList ;

typedef edm::TriggerResults                    TriggerResults;
typedef pat::TriggerObjectStandAlone           TriggerObj;
typedef pat::TriggerObjectStandAloneCollection TriggerObjList;

}


#endif/* end of include guard: TSTARANALYSIS_BASELINESELECTOR_TYPEDEF_HPP */

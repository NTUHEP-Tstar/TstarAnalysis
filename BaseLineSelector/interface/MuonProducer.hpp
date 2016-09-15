/*******************************************************************************
*
*  Filename    : MuonProducer.hpp
*  Description : Defining plugin for muon production
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*
*******************************************************************************/
#ifndef TSTARANALYSIS_BASELINESELECTOR_MUONPRODUCER_HPP
#define TSTARANALYSIS_BASELINESELECTOR_MUONPRODUCER_HPP

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include <memory>

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include <vector>

typedef std::vector<pat::Muon> MuonList;

// ------------------------------------------------------------------------------
//   Class Definition
// ------------------------------------------------------------------------------
class MuonProducer : public edm::stream::EDFilter<>
{
public:
   explicit
   MuonProducer( const edm::ParameterSet& );
   ~MuonProducer();
   static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
   virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

   // Data Members
   const edm::EDGetToken _rhosrc;
   const edm::EDGetToken _vertexsrc;
   const edm::EDGetToken _packedsrc;
   const edm::EDGetToken _muonsrc;
   const edm::EDGetToken _hltsrc;
   const edm::EDGetToken _triggerobjsrc;

   reco::Vertex _primary_vertex;
   edm::Handle<double> _rhoHandle;
   edm::Handle<reco::VertexCollection> _vertexHandle;
   edm::Handle<pat::PackedCandidateCollection> _packedHandle;
   edm::Handle<std::vector<pat::Muon>> _muonHandle;
   edm::Handle<edm::TriggerResults> _hltHandle;
   edm::Handle<pat::TriggerObjectStandAloneCollection> _triggerObjectHandle;
   const std::string _reqtrigger;

   // Helper private functions, see src/MuonSelction.cc
   bool GetPrimaryVertex();
   bool IsSelectedMuon( const pat::Muon&, const edm::Event& ) const;
   bool IsVetoMuon( const pat::Muon&, const edm::Event& ) const;
   void AddMuonVariables( pat::Muon&, const edm::Event& )   const;
};


#endif/* end of include guard: TSTARANALYSIS_BASELINESELECTOR_MUONPRODUCER_HPP */

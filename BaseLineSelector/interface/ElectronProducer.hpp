/*******************************************************************************
*
*  Filename    : ElectronProducer.cc
*  Description : Defining plugin for electron selection and caching
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_BASELINESELECTOR_ELECTRONPRODUCER_HPP
#define TSTARANALYSIS_BASELINESELECTOR_ELECTRONPRODUCER_HPP

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include <memory>

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include <vector>


// ------------------------------------------------------------------------------
//   Class Definition
// ------------------------------------------------------------------------------
class ElectronProducer : public edm::stream::EDFilter<>
{
public:
   explicit
   ElectronProducer( const edm::ParameterSet& );
   ~ElectronProducer();
   static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
   virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

   // Data Members
   const edm::EDGetToken _electronsrc;
   const edm::EDGetToken _vetoMapToken;
   const edm::EDGetToken _looseMapToken;
   const edm::EDGetToken _mediumMapToken;
   const edm::EDGetToken _tightMapToken;
   const edm::EDGetToken _packedsrc;

   edm::Handle<std::vector<pat::Electron>> _electronHandle;
   edm::Handle<pat::PackedCandidateCollection> _packedHandle;
   edm::Handle<edm::ValueMap<bool> > _vetoMapHandle;
   edm::Handle<edm::ValueMap<bool> > _looseMapHandle;
   edm::Handle<edm::ValueMap<bool> > _mediumMapHandle;
   edm::Handle<edm::ValueMap<bool> > _tightMapHandle;

   // Helper functions: see src/ElectronSelection.cc
   bool IsSelectedElectron(
      pat::Electron&,
      const edm::Ptr<pat::Electron>&,
      const edm::Event&
      ) const;
   bool IsVetoElectron(
      const pat::Electron&,
      const edm::Ptr<pat::Electron>&,
      const edm::Event&
      ) const;
   void AddElectronVariables(
      pat::Electron&,
      const edm::Event&
      ) const;

};


#endif/* end of include guard: TSTARANALYSIS_BASELINESELECTOR_ELECTRONPRODUCER_HPP */

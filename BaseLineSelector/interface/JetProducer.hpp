/*******************************************************************************
*
*  Filename    : JetProducer.hpp
*  Description : Defining Plugin for Jet selection and Caching
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_BASELINESELECTOR_JETPRODUCER_HPP
#define TSTARANALYSIS_BASELINESELECTOR_JETPRODUCER_HPP

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include <memory>

#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include <vector>

class JetProducer : public edm::stream::EDFilter<>
{
public:
   explicit
   JetProducer( const edm::ParameterSet& );
   ~JetProducer();
   static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
   virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

   // EDM interaction handles
   const edm::EDGetToken _jetsrc;
   const edm::EDGetToken _muonsrc;
   const edm::EDGetToken _electronsrc;
   const edm::EDGetToken _rhosrc;
   edm::Handle<std::vector<pat::Jet> > _jetHandle;
   edm::Handle<std::vector<pat::Muon> > _muonHandle;
   edm::Handle<std::vector<pat::Electron> > _electronHandle;
   edm::Handle<double> _rhoHandle;

   // Jet energy resolution and correction handler
   edm::ESHandle<JetCorrectorParametersCollection> _jetcorHandle;
   std::unique_ptr<JetCorrectionUncertainty> _jecunc;
   std::unique_ptr<JME::JetResolution> _jetptres;
   std::unique_ptr<JME::JetResolution> _jetphires;
   std::unique_ptr<JME::JetResolutionScaleFactor> _jetressf;

   // Helper variables, see src/JetSelection.cc
   bool           IsSelectedJet( const pat::Jet&, const bool isdata ) const;
   void           AddJetVariables( pat::Jet&, const bool isdata );// cannot be constant!
   bool           IsWellMatched( const pat::Jet& ) const;
   TLorentzVector MakeScaled( const pat::Jet& ) const;
   TLorentzVector MakeSmeared( const pat::Jet& ) const;
   double         GetJetPtRes( const pat::Jet& ) const;
   double         GetJetResScale( const pat::Jet& ) const;
};


#endif/* end of include guard: TSTARANALYSIS_BASELINESELECTOR_JETPRODUCER_HPP */

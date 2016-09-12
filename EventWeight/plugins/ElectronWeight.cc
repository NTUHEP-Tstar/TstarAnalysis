/*******************************************************************************
*
*  Filename    : ElectronWeight
*  Description : Adding weighting according to electron status
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/

// system include files
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// user include files
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "ManagerUtils/EDMUtils/interface/TFileUtils.hpp"

#include "TAxis.h"
#include "TFile.h"
#include "TH2D.h"

// ------------------------------------------------------------------------------
//   Class Definition
// ------------------------------------------------------------------------------
class ElectronWeight : public edm::EDProducer {
public:
   explicit ElectronWeight(const edm::ParameterSet &);
   ~ElectronWeight();

private:
   virtual void produce(edm::Event &, const edm::EventSetup &) override;

// Getting objects from vector of sums
   const edm::EDGetToken _elecsrc;
   edm::Handle<std::vector<pat::Electron> > _elechandle;

// Vector representing weights
   const TH2D *_cutweight;
   double getweight(const TH2D *, double pt, double eta) const;
};
using namespace edm;
using namespace std;

// ------------------------------------------------------------------------------
//   Constructor and destructor
// ------------------------------------------------------------------------------
ElectronWeight::ElectronWeight(const edm::ParameterSet &iConfig) :
   _elecsrc(consumes<std::vector<pat::Electron> >(
               iConfig.getParameter<InputTag>("elecsrc"))),
   _cutweight((const TH2D *)GetCloneFromFile(
                 iConfig.getParameter<FileInPath>("cutfile").fullPath(),
                 "EGamma_SF2D")) {
   produces<double>("ElectronWeight").setBranchAlias("ElectronWeight");
}

ElectronWeight::~ElectronWeight() {
   delete _cutweight;
}

// ------------------------------------------------------------------------------
//   Main Control flow
// ------------------------------------------------------------------------------
void ElectronWeight::produce(edm::Event &iEvent,
                             const edm::EventSetup &iSetup) {
   if (iEvent.isRealData())
      return; // Skipping over data files

   iEvent.getByToken(_elecsrc, _elechandle);

   auto_ptr<double> elecweightptr(new double(1.));
   double &elecweight = *elecweightptr;

   if (_elechandle->size()) {
      const double pt = _elechandle->at(0).pt();
      const double eta = _elechandle->at(0).eta();

      elecweight = getweight(_cutweight, pt, eta);
   }

   iEvent.put(elecweightptr, "ElectronWeight");
}

double ElectronWeight::getweight(const TH2D *hist, double pt,
                                 double eta) const {
   if (!hist)
      return 1.;

   // Re-evaluating to avoid overflow
   eta = min(eta, hist->GetXaxis()->GetXmax() - 0.01);
   eta = max(eta, hist->GetXaxis()->GetXmin() + 0.01);
   pt = min(pt, hist->GetYaxis()->GetXmax() - 0.01);

   return hist->GetBinContent(hist->FindFixBin(eta, pt));
}

DEFINE_FWK_MODULE(ElectronWeight);

/*******************************************************************************
*
*  Filename    : BtagWeight.cc
*  Description : Producer for pile up weights
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
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TstarAnalysis/Common/interface/BTagChecker.hpp"

// ------------------------------------------------------------------------------
//   Class Definition
// ------------------------------------------------------------------------------
class BtagWeight : public edm::EDProducer {
public:
  explicit BtagWeight(const edm::ParameterSet &);
  ~BtagWeight();

private:
  virtual void produce(edm::Event &, const edm::EventSetup &) override;

  // Getting objects from vector of sums
  const edm::EDGetToken _jetsrc;
  edm::Handle<std::vector<pat::Jet>> _jethandle;

  BTagChecker _btagcheck;
};
using namespace edm;
using namespace std;

// ------------------------------------------------------------------------------
//   Constructor and destructor
// ------------------------------------------------------------------------------
BtagWeight::BtagWeight(const edm::ParameterSet &iConfig)
    : _jetsrc(consumes<std::vector<pat::Jet>>(
          iConfig.getParameter<edm::InputTag>("jetsrc"))),
      _btagcheck("bcheck",
                 iConfig.getParameter<edm::FileInPath>("btagfile").fullPath()) {
  produces<double>("BtagWeight");
  produces<double>("BtagWeightup");
  produces<double>("BtagWeightdown");
}

BtagWeight::~BtagWeight() {}

// ------------------------------------------------------------------------------
//   Main Control flow
// ------------------------------------------------------------------------------
void BtagWeight::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {
  if (iEvent.isRealData())
    return; // skipping if is data event

  std::auto_ptr<double> btagweightptr(new double(1.));
  std::auto_ptr<double> btagweightupptr(new double(1.));
  std::auto_ptr<double> btagweightdownptr(new double(1.));
  double &btagweight = *btagweightptr;
  double &btagweightup = *btagweightupptr;
  double &btagweightdown = *btagweightdownptr;

  iEvent.getByToken(_jetsrc, _jethandle);

  for (const auto &jet : *_jethandle) {
    if (_btagcheck.PassMedium(jet)) {
      btagweight *= _btagcheck.GetMediumScaleFactor(jet);
      btagweightup *= _btagcheck.GetMediumScaleFactorUp(jet);
      btagweightdown *= _btagcheck.GetMediumScaleFactorDown(jet);
    }
  }
  iEvent.put(btagweightptr, "BtagWeight");
  iEvent.put(btagweightupptr, "BtagWeightup");
  iEvent.put(btagweightdownptr, "BtagWeightdown");
}

DEFINE_FWK_MODULE(BtagWeight);

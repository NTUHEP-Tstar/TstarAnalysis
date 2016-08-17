/*******************************************************************************
 *
 *  Filename    : AddEventWeight.cc
 *  Description : A Producer to add event weights into an Event
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
// system include files
#include <memory>
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/MergeableCounter.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "TstarAnalysis/EventWeight/interface/PileUpWeighter.hpp"

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class EventWeighter : public edm::one::EDProducer<edm::one::WatchRuns,edm::EndRunProducer>
{
public:
   explicit EventWeighter(const edm::ParameterSet&);
   ~EventWeighter();

private:
   virtual void produce(edm::Event &, const edm::EventSetup&) override;
   virtual void beginRun( const edm::Run&,const edm::EventSetup&) override;
   virtual void endRun  ( const edm::Run&,const edm::EventSetup&) override;
   virtual void endRunProduce(edm::Run& , const edm::EventSetup&) override;

   const edm::EDGetToken  _lhesrc;
   const edm::EDGetToken  _pusrc ;
   edm::Handle<LHEEventProduct>                _lhehandle;
   edm::Handle<std::vector<PileupSummaryInfo>> _puhandle;

   const PileUpWeighter _puweighter;
   double _weightsum;
};
using namespace edm;
using namespace std;

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
EventWeighter::EventWeighter(const edm::ParameterSet& iConfig):
   _lhesrc( consumes<LHEEventProduct>               (iConfig.getParameter<edm::InputTag>("lhesrc"))),
   _pusrc ( consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pusrc"))),
   _puweighter( edm::FileInPath("TstarAnalysis/EventWeight/data/pileupweights.csv").fullPath() )
{
   // A tally over weight sum
   produces<double, edm::InRun>("WeightSum").setBranchAlias("WeightSum");

   produces<double>("EventWeight") .setBranchAlias("EventWeight");
   produces<double>("PileupWeight").setBranchAlias("PileupWeight");
}

EventWeighter::~EventWeighter(){}

//------------------------------------------------------------------------------
//   Main Control flow
//------------------------------------------------------------------------------
void EventWeighter::beginRun( const edm::Run& , const edm::EventSetup& )
{
   _weightsum = 0;
}

void EventWeighter::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   std::auto_ptr<double> eventweightptr ( new double (1.) );
   std::auto_ptr<double> pileupweightptr( new double (1.) );
   double& eventweight  = *eventweightptr;
   double& pileupweight = *pileupweightptr;

   // Only Weighting if is MC dataset
   if( !iEvent.isRealData() ){
      // Getting relavent objects
      iEvent.getByToken( _lhesrc , _lhehandle );
      iEvent.getByToken( _pusrc  , _puhandle  );
      // Getting sign of weight (appling to eventweight only!)
      if( _lhehandle.isValid() && _lhehandle->hepeup().XWGTUP < 0 ){ eventweight *= -1.; }
      if( _puhandle.isValid() ){
         const unsigned pu = _puhandle->at(0).getPU_NumInteractions();
         pileupweight = _puweighter.GetWeight( pu );
      }
      // Summing all weights together:
      eventweight *= pileupweight;

      iEvent.put( eventweightptr  , "EventWeight" );
      iEvent.put( pileupweightptr , "PileupWeight");

   }

   _weightsum += eventweight;
}

void EventWeighter::endRun( const edm::Run&, const edm::EventSetup& )
{}

   void EventWeighter::endRunProduce( edm::Run& iRun , const EventSetup& iSetup)
   {
      auto_ptr<double> weightsum( new double(_weightsum) );
      iRun.put( weightsum, "WeightSum");
   }



   DEFINE_FWK_MODULE(EventWeighter);

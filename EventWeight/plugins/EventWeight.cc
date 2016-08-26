/*******************************************************************************
 *
 *  Filename    : EventWeight.cc
 *  Description : Summing of all event weights introduced in file
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
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

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class EventWeight : public edm::one::EDProducer<edm::one::WatchRuns,edm::EndRunProducer>
{
public:
   explicit EventWeight(const edm::ParameterSet&);
   ~EventWeight();

private:
   virtual void produce(edm::Event &, const edm::EventSetup&) override;
   virtual void beginRun( const edm::Run&,const edm::EventSetup&) override;
   virtual void endRun  ( const edm::Run&,const edm::EventSetup&) override;
   virtual void endRunProduce(edm::Run& , const edm::EventSetup&) override;

   const edm::EDGetToken  _elecw_src;
   const edm::EDGetToken  _puw_src;
   const edm::EDGetToken  _lhesrc;
   edm::Handle<LHEEventProduct> _lheHandle;
   edm::Handle<double>   _elecwhandle;
   edm::Handle<double>   _puwhandle;

   bool   _isdata;
   double _weightsum;
};
using namespace edm;
using namespace std;

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
EventWeight::EventWeight(const edm::ParameterSet& iConfig):
   _elecw_src( consumes<double>(iConfig.getParameter<edm::InputTag>("elecwsrc"))),
   _puw_src  ( consumes<double>(iConfig.getParameter<edm::InputTag>("puwsrc"))),
   _lhesrc( consumes<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lhesrc")) ),
   _isdata( false )
{
   produces<double>("EventWeight").setBranchAlias("EventWeight");
   produces<double, edm::InRun>("WeightSum").setBranchAlias("WeightSum");
}

EventWeight::~EventWeight(){}

//------------------------------------------------------------------------------
//   Main Control flow
//------------------------------------------------------------------------------
void EventWeight::beginRun( const edm::Run& , const edm::EventSetup& )
{
   _weightsum = 0;
}

void EventWeight::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   if( iEvent.isRealData() ) { _isdata = true; return; }
   _isdata = false;

   auto_ptr<double> weightptr ( new double(1.) );
   double& eventweight = *weightptr;

   // Multilpying by weights, skipping over if doesn't exists
   try{
      iEvent.getByToken( _elecw_src , _elecwhandle );
      eventweight *= *_elecwhandle;
   } catch( std::exception ) {}

   iEvent.getByToken( _puw_src   , _puwhandle   );
   eventweight *= *_puwhandle ;

   try {
      iEvent.getByToken( _lhesrc    , _lheHandle );
      if( _lheHandle->hepeup().XWGTUP <= 0 ){
         eventweight *= -1.;
      }
   } catch( std::exception ) {}

   iEvent.put( weightptr , "EventWeight" );
   _weightsum += eventweight;
}

void EventWeight::endRun( const edm::Run&, const edm::EventSetup& )
{
}

void EventWeight::endRunProduce( edm::Run& iRun , const EventSetup& iSetup)
{
   if( _isdata ) { return ; }
   auto_ptr<double> ptr( new double(_weightsum) );
   iRun.put( ptr, "WeightSum" );
}

DEFINE_FWK_MODULE(EventWeight);

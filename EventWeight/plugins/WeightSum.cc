/*******************************************************************************
*
*  Filename    : WeightSum.cc
*  Description : Top Pt reweighting algorithm
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  See https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
*  For details
*
*******************************************************************************/
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/EDMUtils/interface/Counter.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class WeightSum :
   public edm::one::EDProducer<edm::one::WatchRuns, edm::EndRunProducer>
{
public:
   explicit
   WeightSum( const edm::ParameterSet& );
   ~WeightSum();

private:
   virtual void beginRun( const edm::Run&, const edm::EventSetup& ) override;
   virtual void endRun( const edm::Run&, const edm::EventSetup& ) override;
   virtual void endRunProduce( edm::Run&, const edm::EventSetup& ) override;
   virtual void produce( edm::Event&, const edm::EventSetup& ) override;

   // Getting objects from vector of sums
   const edm::EDGetToken _weightsrc;
   edm::Handle<double> _weighthandle;

   bool   _isdata; // required for per run production
   double _weightsum;

};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
WeightSum::WeightSum( const edm::ParameterSet& iConfig ) :
   _weightsrc( GETTOKEN( iConfig, double, "weightsrc" ) )
{
   produces<mgr::Counter, edm::InRun>("WeightSum");
}

WeightSum::~WeightSum(){}
/*******************************************************************************
*   Main control flow
*******************************************************************************/
void WeightSum::beginRun( const edm::Run&, const edm::EventSetup& )
{
   _weightsum = 0 ;
}

void
WeightSum::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   if( iEvent.isRealData() ){ _isdata= true; return; }// skipping if is data event
   _isdata = false;
   iEvent.getByToken( _weightsrc, _weighthandle );
   _weightsum += *_weighthandle;
}

/******************************************************************************/

void WeightSum::endRun( const edm::Run& , const edm::EventSetup& ) {}

/******************************************************************************/

void WeightSum::endRunProduce( edm::Run& iRun, const edm::EventSetup& )
{
   if( _isdata ) { return ; }
   auto_ptr<mgr::Counter> sumptr( new mgr::Counter(_weightsum));
   iRun.put( sumptr, "WeightSum" );
}

/******************************************************************************/

DEFINE_FWK_MODULE( WeightSum );

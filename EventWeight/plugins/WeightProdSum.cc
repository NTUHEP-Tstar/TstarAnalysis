/*******************************************************************************
*
*  Filename    : WeightProdSum.cc
*  Description : Top Pt reweighting algorithm
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Storing the product of a list of doubles sources as a single weight,
*  also summes all weight and store it in run level.
*
*******************************************************************************/
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "ManagerUtils/EDMUtils/interface/Counter.hpp"
#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class WeightProdSum :
   public edm::one::EDProducer<edm::one::WatchRuns, edm::EndRunProducer>
{
public:
   explicit
   WeightProdSum( const edm::ParameterSet& );
   ~WeightProdSum();

private:
   virtual void beginRun( const edm::Run&, const edm::EventSetup& ) override;
   virtual void endRun( const edm::Run&, const edm::EventSetup& ) override;
   virtual void endRunProduce( edm::Run&, const edm::EventSetup& ) override;
   virtual void produce( edm::Event&, const edm::EventSetup& ) override;

   // Getting objects from vector of sums
   std::vector<edm::EDGetToken> _weightsrclist;
   edm::Handle<double>          _weighthandle;

   double _weightsum;

};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
WeightProdSum::WeightProdSum( const edm::ParameterSet& iConfig )
{
   vector<edm::InputTag> taglist = iConfig.getParameter<vector<edm::InputTag>>("weightlist");
   for( const auto& tag : taglist ){
      _weightsrclist.push_back( consumes<double>(tag) );
   }

   produces<double>( "WeightProd" );
   produces<mgr::Counter, edm::InRun>( "WeightSum" );
}

WeightProdSum::~WeightProdSum(){}
/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
WeightProdSum::beginRun( const edm::Run&, const edm::EventSetup& )
{
   _weightsum = 0;
}

void
WeightProdSum::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   if( iEvent.isRealData() ){
      _weightsum += 1; // Always ++ for data
      return;          // Do not attempt to read weights
   } // skipping if is data event

   auto_ptr<double> weight( new double(1.0) );

   for( const auto& src : _weightsrclist ){
      // No error detection, throw exception if weight is not found.
      iEvent.getByToken( src, _weighthandle );
      *weight *= *_weighthandle;
   }
   _weightsum += *weight;

   iEvent.put( weight, "WeightProd" );
}

/******************************************************************************/

void
WeightProdSum::endRun( const edm::Run&, const edm::EventSetup& ){}

/******************************************************************************/

void
WeightProdSum::endRunProduce( edm::Run& iRun, const edm::EventSetup& )
{
   // Weight sum should be number of events for data.
   auto_ptr<mgr::Counter> sumptr( new mgr::Counter( _weightsum ) );
   iRun.put( sumptr, "WeightSum" );
}

/******************************************************************************/


DEFINE_FWK_MODULE( WeightProdSum );

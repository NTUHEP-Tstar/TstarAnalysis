/*******************************************************************************
*
*  Filename    : EventWeight.cc
*  Description : Summing of all event weights introduced in file
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
// user include files
#include "DataFormats/Common/interface/MergeableCounter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"

// ------------------------------------------------------------------------------
//   Class Definition
// ------------------------------------------------------------------------------
class EventWeight :
   public edm::one::EDProducer<edm::one::WatchRuns, edm::EndRunProducer>
{
public:
   explicit
   EventWeight( const edm::ParameterSet& );
   ~EventWeight();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& ) override;
   virtual void beginRun( const edm::Run&, const edm::EventSetup& ) override;
   virtual void endRun( const edm::Run&, const edm::EventSetup& ) override;
   virtual void endRunProduce( edm::Run&, const edm::EventSetup& ) override;

   const edm::EDGetToken _elecwsrc;
   const edm::EDGetToken _muonwsrc;
   const edm::EDGetToken _puwsrc;
   const edm::EDGetToken _btagsrc;
   const edm::EDGetToken _lhesrc;
   edm::Handle<LHEEventProduct> _lheHandle;
   edm::Handle<double> _elecwhandle;
   edm::Handle<double> _muonwhandle;
   edm::Handle<double> _puwhandle;
   edm::Handle<double> _btaghandle;

   bool _isdata;
   double _weightsum;
};
using namespace edm;
using namespace std;

// ------------------------------------------------------------------------------
//   Constructor and destructor
// ------------------------------------------------------------------------------
EventWeight::EventWeight( const edm::ParameterSet& iConfig ) :
   _elecwsrc( GETTOKEN( iConfig, double,  "elecwsrc" ) ),
   _muonwsrc( GETTOKEN( iConfig, double, "muonwsrc" ) ),
   _puwsrc( GETTOKEN( iConfig, double, "puwsrc" ) ),
   _btagsrc( GETTOKEN( iConfig, double, "btagsrc" ) ),
   _lhesrc( GETTOKEN( iConfig, LHEEventProduct, "lhesrc" ) ),
   _isdata( false )
{
   produces<double>(             "EventWeight" );

   produces<double, edm::InRun>( "WeightSum" );
}

EventWeight::~EventWeight(){}

// ------------------------------------------------------------------------------
//   Main Control flow
// ------------------------------------------------------------------------------
void
EventWeight::beginRun( const edm::Run&, const edm::EventSetup& )
{
   _weightsum = 0;
}

/******************************************************************************/

void
EventWeight::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   if( iEvent.isRealData() ){ _isdata = true;  return; }// Don't do anything for data
   _isdata = false;


   auto_ptr<double> weightptr( new double(1.) );
   double& eventweight = *weightptr;

   // Multilpying by electron weights, skipping over if doesn't exist
   try {
      iEvent.getByToken( _elecwsrc, _elecwhandle );
      eventweight *= *_elecwhandle;
   } catch( std::exception ){
   }

   // Multiplying by muon weights, skipping over if doesn't exist
   try {
      iEvent.getByToken( _muonwsrc, _muonwhandle );
      eventweight *= *_muonwhandle;
   } catch( std::exception ){
   }

   // Pileup weight should definitely exist
   iEvent.getByToken( _puwsrc, _puwhandle   );
   eventweight *= *_puwhandle;

   // B tag weights should definitely exist
   iEvent.getByToken( _btagsrc, _btaghandle );
   eventweight *= *_btaghandle;

   // Try to get LHE information
   try {
      iEvent.getByToken( _lhesrc, _lheHandle );
      if( _lheHandle->hepeup().XWGTUP <= 0 ){
         eventweight *= -1.;
      }
   } catch( std::exception ){
   }

   iEvent.put( weightptr, "EventWeight" );
   _weightsum += eventweight;
}

/******************************************************************************/

void
EventWeight::endRun( const edm::Run&, const edm::EventSetup& )
{
}

/******************************************************************************/

void
EventWeight::endRunProduce( edm::Run& iRun, const EventSetup& iSetup )
{
   if( _isdata ){ return; }
   auto_ptr<double> ptr( new double(_weightsum) );
   iRun.put( ptr, "WeightSum" );
}

/******************************************************************************/

DEFINE_FWK_MODULE( EventWeight );

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
class EventWeight : public edm::EDProducer
{
public:
   explicit
   EventWeight( const edm::ParameterSet& );
   ~EventWeight();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& ) override;

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

};
using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor
*******************************************************************************/
EventWeight::EventWeight( const edm::ParameterSet& iConfig ) :
   _elecwsrc( GETTOKEN( iConfig, double,  "elecwsrc" ) ),
   _muonwsrc( GETTOKEN( iConfig, double,  "muonwsrc" ) ),
   _puwsrc( GETTOKEN( iConfig, double,  "puwsrc"   ) ),
   _btagsrc( GETTOKEN( iConfig, double,  "btagsrc"  ) ),
   _lhesrc( GETTOKEN( iConfig, LHEEventProduct, "lhesrc" ) )
{
   produces<double>( "EventWeight" );
}

EventWeight::~EventWeight(){}

/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
EventWeight::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   if( iEvent.isRealData() ){ return; }// Don't do anything for data

   auto_ptr<double> weightptr( new double(1.) );
   double& eventweight = *weightptr;

   iEvent.getByToken( _elecwsrc, _elecwhandle );
   iEvent.getByToken( _muonwsrc, _muonwhandle );
   iEvent.getByToken( _puwsrc,   _puwhandle   );
   iEvent.getByToken( _btagsrc,  _btaghandle );
   iEvent.getByToken( _lhesrc,   _lheHandle );

   // weights that should definitely exists
   eventweight *= *_puwhandle;
   eventweight *= *_btaghandle;

   if( _elecwhandle.isValid() ){ eventweight *= *_elecwhandle; }
   if( _muonwhandle.isValid() ){ eventweight *= *_muonwhandle; }
   if( _lheHandle.isValid() && _lheHandle->hepeup().XWGTUP <= 0 ){
      eventweight *= -1.;
   }

   // Will not be caching Scaled up event weight
   iEvent.put( weightptr, "EventWeight" );
}

/******************************************************************************/

DEFINE_FWK_MODULE( EventWeight );

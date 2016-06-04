/*******************************************************************************
 *
 *  Filename    : TestPlugin.cc
 *  Description : Tstar Mass reconstruction algorithms
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include <vector>
#include <iostream>

typedef std::vector<pat::Jet>      JetList;

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class TestPlugin : public edm::EDProducer {
public:
   TestPlugin(const edm::ParameterSet& );
   virtual ~TestPlugin();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& );

   const edm::EDGetToken  _jetsrc;
   edm::Handle<JetList>   _jetHandle;

};

//------------------------------------------------------------------------------
//   Constructor and Desctructor
//------------------------------------------------------------------------------
TestPlugin::TestPlugin( const edm::ParameterSet& iConfig ):
   _jetsrc(  consumes<JetList>     (iConfig.getParameter<edm::InputTag>("jetsrc"))  )
{
   produces<std::vector<edm::Ptr<pat::Jet>>>("ptrtest").setBranchAlias("ptrtest");
}

TestPlugin::~TestPlugin()
{}


//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void TestPlugin::produce( edm::Event& iEvent, const edm::EventSetup& )
{
   iEvent.getByToken( _jetsrc  , _jetHandle  );
   std::auto_ptr<std::vector<edm::Ptr<pat::Jet>>>     ptr_list( new std::vector<edm::Ptr<pat::Jet>> );

   for( unsigned i = 0 ; i < _jetHandle->size() ; ++i ){
      ptr_list->push_back( edm::Ptr<pat::Jet>(_jetHandle , i) );
   }

   iEvent.put( ptr_list , "ptrtest" );
}

DEFINE_FWK_MODULE(TestPlugin);

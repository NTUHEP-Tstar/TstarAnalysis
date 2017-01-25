/*******************************************************************************
*
*  Filename    : GenWeight.cc
*  Description : Getting the sign of the central weight
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
// user include files
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"

/*******************************************************************************
*   Class definition
*******************************************************************************/
class GenWeight : public edm::EDProducer
{
public:
  explicit
  GenWeight( const edm::ParameterSet& );
  ~GenWeight();

private:
  virtual void produce( edm::Event&, const edm::EventSetup& ) override;

  const edm::EDGetToken _lhesrc;
  edm::Handle<LHEEventProduct> _lheHandle;
};
using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor
*******************************************************************************/
GenWeight::GenWeight( const edm::ParameterSet& iConfig ) :
  _lhesrc( GETTOKEN( iConfig, LHEEventProduct, "lhesrc" ) )
{
  produces<double>( "GenWeight" );
}

GenWeight::~GenWeight(){}

/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
GenWeight::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
  if( iEvent.isRealData() ){ return; } // Don't do anything for data

  auto_ptr<double> weightptr( new double(1.) );
  iEvent.getByToken( _lhesrc, _lheHandle );
  if( _lheHandle.isValid()  && _lheHandle->originalXWGTUP() < 0 ){
    *weightptr = -1;
  }

  iEvent.put( weightptr, "GenWeight" );
}

/******************************************************************************/

DEFINE_FWK_MODULE( GenWeight );

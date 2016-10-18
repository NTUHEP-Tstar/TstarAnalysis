/*******************************************************************************
*
*  Filename    : TopPtWeight.cc
*  Description : Top Pt reweighting algorithm
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  See https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting
*  For details
*
*******************************************************************************/
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class TopPtWeight : public edm::EDProducer
{
public:
   explicit
   TopPtWeight( const edm::ParameterSet& );
   ~TopPtWeight();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& ) override;

   // Getting objects from vector of sums
   const edm::EDGetToken _gensrc;
   edm::Handle<std::vector<reco::GenParticle> > _genhandle;

   const double _a;
   const double _b;
   const double _minpt;
   const double _maxpt;

   double gettopweight( double pt ) const;
};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
TopPtWeight::TopPtWeight( const edm::ParameterSet& iConfig ) :
   _gensrc( GETTOKEN( iConfig, std::vector<reco::GenParticle>, "gensrc" ) ),
   _a( iConfig.getParameter<double>( "a" ) ),
   _b( iConfig.getParameter<double>( "b" ) ),
   _minpt( iConfig.getParameter<double>("minpt") ),
   _maxpt( iConfig.getParameter<double>("maxpt") )
{
   produces<double>( "TopPtWeight" );
}

TopPtWeight::~TopPtWeight(){}

/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
TopPtWeight::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   if( iEvent.isRealData() ){ return; }// skipping if is data event

   std::auto_ptr<double> weight( new double(1.) );
   double topweight     = 1;
   double antitopweight = 1;

   unsigned i = 0;
   iEvent.getByToken( _gensrc, _genhandle );

   for( const auto& gen : *_genhandle ){
      if( i > 20 ){ break; }// should be in first 20 entries

      if( gen.pdgId() == 6 && gen.numberOfMothers() >= 2 && topweight == 1 ){
         topweight = gettopweight( gen.pt() );
      } else if( gen.pdgId() == -6 && gen.numberOfMothers() >= 2 && antitopweight == 1 ){
         antitopweight = gettopweight( gen.pt() );
      }

      if( topweight != 1 && antitopweight != 1 ){
         *weight = sqrt( topweight * antitopweight );
         break;
      }
      ++i;
   }

   iEvent.put( weight, "TopPtWeight" );
}


/******************************************************************************/

double
TopPtWeight::gettopweight( double pt ) const
{
   pt = std::max( _minpt, pt );
   pt = std::min( _maxpt, pt );
   return exp( _a + _b * pt );
}

/******************************************************************************/

DEFINE_FWK_MODULE( TopPtWeight );

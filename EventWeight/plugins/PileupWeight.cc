/*******************************************************************************
*
*  Filename    : PileupWeight.cc
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
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class PileupWeight : public edm::EDProducer
{
public:
   explicit
   PileupWeight( const edm::ParameterSet& );
   ~PileupWeight();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& ) override;

   // Getting objects from vector of sums
   const edm::EDGetToken _pusrc;
   edm::Handle<std::vector<PileupSummaryInfo> > _puhandle;

   // Vector representing weights
   const std::vector<double> _puweights;
   std::vector<double> readweight( const std::string& );// for aiding with contruction
   double              getweight( const unsigned ) const;
};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
PileupWeight::PileupWeight( const edm::ParameterSet& iConfig ) :
   _pusrc( GETTOKEN( iConfig, std::vector<PileupSummaryInfo>, "pusrc" ) ),
   _puweights( readweight( GETFILEPATH( iConfig, "pileupfile" ) ) )
{
   produces<double>( "PileupWeight" );
}

PileupWeight::~PileupWeight(){}

/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
PileupWeight::produce( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   if( iEvent.isRealData() ){ return; }// skipping if is data event

   std::auto_ptr<double> pileupweightptr( new double(1.) );
   double& pileupweight = *pileupweightptr;

   iEvent.getByToken( _pusrc, _puhandle );
   const unsigned pu = _puhandle->at( 0 ).getPU_NumInteractions();
   if( pu < _puweights.size() ){
      pileupweight = _puweights.at( pu );
   }
   iEvent.put( pileupweightptr, "PileupWeight" );
}


/******************************************************************************/

vector<double>
PileupWeight::readweight( const string& filename )
{
   vector<double> ans;
   double weight;
   FILE* weightfile = fopen( filename.c_str(), "r" );

   while( fscanf( weightfile, "%lf", &weight ) != EOF ){
      ans.push_back( weight );
   }

   return ans;
}

/******************************************************************************/

double
PileupWeight::getweight( const unsigned x ) const
{
   if( x < _puweights.size() ){ return _puweights.at( x ); } else { return 1.; }
}

/******************************************************************************/

DEFINE_FWK_MODULE( PileupWeight );

/*******************************************************************************
 *
 *  Filename    : PileupWeight.cc
 *  Description : Producer for pile up weights
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
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"


//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class PileupWeight : public edm::EDProducer
{
public:
   explicit PileupWeight(const edm::ParameterSet&);
   ~PileupWeight();

private:
   virtual void produce(edm::Event &, const edm::EventSetup&) override;

   // Getting objects from vector of sums
   const edm::EDGetToken  _pusrc ;
   edm::Handle<std::vector<PileupSummaryInfo>> _puhandle;

   // Vector representing weights
   const std::vector<double>  _puweights;
   const std::vector<double> readweightfromfile( const std::string& ); // for aiding with contruction
   double getweight( const unsigned ) const ;
};
using namespace edm;
using namespace std;

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
PileupWeight::PileupWeight(const edm::ParameterSet& iConfig):
   _pusrc ( consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pusrc"))),
   _puweights( readweightfromfile( iConfig.getParameter<edm::FileInPath>("pileupfile").fullPath() ))
{
   produces<double>("PileupWeight").setBranchAlias("PileupWeight");
}

PileupWeight::~PileupWeight(){}

//------------------------------------------------------------------------------
//   Main Control flow
//------------------------------------------------------------------------------
void PileupWeight::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   std::auto_ptr<double> pileupweightptr( new double (1.) );
   double& pileupweight = *pileupweightptr;

   // Only Weighting if is MC dataset
   if( !iEvent.isRealData() ){
      iEvent.getByToken( _pusrc  , _puhandle );
      const unsigned pu = _puhandle->at(0).getPU_NumInteractions();
      if( pu < _puweights.size() ){
         pileupweight = _puweights.at( pu );
      }
      iEvent.put( pileupweightptr , "PileupWeight");
   }
}

// Helper function for initializing vector from file
const vector<double> PileupWeight::readweightfromfile( const string& filename )
{
   vector<double> ans ;
   double         weight;
   FILE* weightfile = fopen( filename.c_str() , "r" );
   while( fscanf(weightfile,"%lf", &weight ) != EOF ){
      ans.push_back( weight );
   }
   return ans;
}

double PileupWeight::getweight( const unsigned x ) const
{
   if( x < _puweights.size() ) { return _puweights.at(x); }
   else { return 1.; }
}

DEFINE_FWK_MODULE(PileupWeight);

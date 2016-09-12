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
   const std::vector<double> _puweights;
   const std::vector<double> _puweights71260;
   const std::vector<double> _puweights62000;
   std::vector<double> readweightfromfile( const std::string& ); // for aiding with contruction
   double getweight( const unsigned ) const ;
};
using namespace edm;
using namespace std;

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
PileupWeight::PileupWeight(const edm::ParameterSet& iConfig):
   _pusrc ( consumes<std::vector<PileupSummaryInfo>>(iConfig.getParameter<edm::InputTag>("pusrc"))),
   _puweights( readweightfromfile( iConfig.getParameter<edm::FileInPath>("pileupfile").fullPath() )),
   _puweights71260( readweightfromfile( iConfig.getParameter<edm::FileInPath>("pileupfile71260").fullPath() )),
   _puweights62000( readweightfromfile( iConfig.getParameter<edm::FileInPath>("pileupfile62000").fullPath() ))
{
   produces<double>("PileupWeight");
   produces<double>("PileupWeight72160");
   produces<double>("PileupWeight62000");

}

PileupWeight::~PileupWeight(){}

//------------------------------------------------------------------------------
//   Main Control flow
//------------------------------------------------------------------------------
void PileupWeight::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   if(iEvent.isRealData()) { return; } // skipping if is data event

   std::auto_ptr<double> pileupweightptr( new double (1.) );
   std::auto_ptr<double> pileupweightptr71260( new double (1.) );
   std::auto_ptr<double> pileupweightptr62000( new double (1.) );
   double& pileupweight      = *pileupweightptr;
   double& pileupweight71260 = *pileupweightptr71260;
   double& pileupweight62000 = *pileupweightptr62000;

   iEvent.getByToken( _pusrc  , _puhandle );
   const unsigned pu = _puhandle->at(0).getPU_NumInteractions();
   if( pu < _puweights.size() ){
      pileupweight = _puweights.at( pu );
      pileupweight71260 = _puweights71260.at(pu);
      pileupweight62000 = _puweights62000.at(pu);
   }
   iEvent.put( pileupweightptr , "PileupWeight" );
   iEvent.put( pileupweightptr71260 , "PileupWeight72160" );
   iEvent.put( pileupweightptr62000 , "PileupWeight62000" );
}

// Helper function for initializing vector from file
vector<double> PileupWeight::readweightfromfile( const string& filename )
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

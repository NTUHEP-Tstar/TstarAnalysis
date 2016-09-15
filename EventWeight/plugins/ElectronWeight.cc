/*******************************************************************************
*
*  Filename    : ElectronWeight
*  Description : Adding weighting according to electron status
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Main reference :
*
*******************************************************************************/

// system include files
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
// user include files
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/EDMUtils/interface/TFileUtils.hpp"
#include "ManagerUtils/Maths/interface/Parameter.hpp"

#include "TAxis.h"
#include "TFile.h"
#include "TH2D.h"


/*******************************************************************************
*   Class definition
*******************************************************************************/
class ElectronWeight : public edm::EDProducer
{
public:
   explicit
   ElectronWeight( const edm::ParameterSet& );
   ~ElectronWeight();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& ) override;

   // Getting objects from vector of sums
   const edm::EDGetToken _elecsrc;
   edm::Handle<std::vector<pat::Electron> > _elechandle;

   // Vector representing weights
   const TH2D* _cutweight;
   const TH2D* _gsfweight;
   double getweight( const TH2D*, double pt, double eta ) const;
   double getweighterr( const TH2D*, double pt, double eta ) const;
};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
ElectronWeight::ElectronWeight( const edm::ParameterSet & iConfig ) :
   _elecsrc( GETTOKEN( iConfig, std::vector<pat::Electron>, "elecsrc" ) ),
   _cutweight( (const TH2D*)GetCloneFromFile( GETFILEPATH( iConfig, "cutfile" ), "EGamma_SF2D" ) ),
   _gsfweight( (const TH2D*)GetCloneFromFile( GETFILEPATH( iConfig, "gsffile" ), "EGamma_SF2D" ) )
{
   produces<double>( "ElectronWeight" );
   produces<double>( "ElectronCutWeight" );
   produces<double>( "ElectronGsfWeight" );

   produces<double>( "ElectronWeightup" );
   produces<double>( "ElectronWeightdown" );
}

ElectronWeight::~ElectronWeight()
{
   delete _cutweight;
}

/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
ElectronWeight::produce( edm::Event& iEvent, const edm::EventSetup & iSetup )
{
   if( iEvent.isRealData() ){ return; }// Skipping over data files

   iEvent.getByToken( _elecsrc, _elechandle );

   if( _elechandle->size() == 0 ){ return; }// Skipping over muon signal

   auto_ptr<double> elecweightptr( new double(1) );
   auto_ptr<double> cutweightptr( new double(1) );
   auto_ptr<double> gsfweightptr( new double(1) );
   auto_ptr<double> weightupptr( new double(1) );
   auto_ptr<double> weightdownptr( new double(1) );

   double& elecweight = *elecweightptr;
   double& cutweight  = *cutweightptr;
   double& gsfweight  = *gsfweightptr;
   double& weightup   = *weightupptr;
   double& weightdown = *weightdownptr;

   const double pt  = _elechandle->at( 0 ).pt();
   const double eta = _elechandle->at( 0 ).eta();
   cutweight = getweight( _cutweight, pt, eta );
   gsfweight = getweight( _gsfweight, pt, eta );
   const double cuterr = getweighterr( _cutweight, pt, eta );
   const double gsferr = getweighterr( _gsfweight, pt, eta );

   const Parameter cutparm( cutweight, cuterr, cuterr );
   const Parameter gsfparm( gsfweight, gsferr, gsferr );
   const Parameter totalparam = cutparm * gsfparm;

   elecweight = totalparam.CentralValue();
   weightup   = totalparam.CentralValue() + totalparam.AbsUpperError();
   weightdown = totalparam.CentralValue() - totalparam.AbsUpperError();

   iEvent.put( elecweightptr, "ElectronWeight" );
   iEvent.put( cutweightptr,  "ElectronCutWeight" );
   iEvent.put( gsfweightptr,  "ElectronGsfWeight" );
   iEvent.put( weightupptr,   "ElectronWeightup" );
   iEvent.put( weightdownptr, "ElectronWeightdown" );
}

/******************************************************************************/

double
ElectronWeight::getweight( const TH2D* hist, double pt, double eta ) const
{
   if( !hist ){ return 1.; }

   // Re-evaluating to avoid overflow
   eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
   eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
   pt  = min( pt, hist->GetYaxis()->GetXmax() - 0.01 );
   return hist->GetBinContent( hist->FindFixBin( eta, pt ) );
}

double
ElectronWeight::getweighterr( const TH2D* hist, double pt, double eta ) const
{
   if( !hist ){ return 0.; }

   // Re-evaluating to avoid overflow
   eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
   eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
   pt  = min( pt, hist->GetYaxis()->GetXmax() - 0.01 );
   return hist->GetBinError( hist->FindFixBin( eta, pt ) );
}

DEFINE_FWK_MODULE( ElectronWeight );

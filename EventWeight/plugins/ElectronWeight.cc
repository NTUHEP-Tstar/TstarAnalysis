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
#include "TEfficiency.h"


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
   const TH2D* _gsfweight;
   const TH2D* _cutweight;
   const TEfficiency* _trgweight;

   double getweight( const TH2D*, double pt, double eta ) const;
   double getweighterr( const TH2D*, double pt, double eta ) const;

   Parameter getweightParam( const TEfficiency*, double pt, double eta )const ;
};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
ElectronWeight::ElectronWeight( const edm::ParameterSet & iConfig ) :
   _elecsrc( GETTOKEN( iConfig, std::vector<pat::Electron>, "elecsrc" ) ),
   _gsfweight( (const TH2D*)GETFILEOBJ( iConfig, "gsffile", "gsfhist" ) ),
   _cutweight( (const TH2D*)GETFILEOBJ( iConfig, "cutfile", "cuthist" ) ),
   _trgweight( (const TEfficiency*)GETFILEOBJ( iConfig, "trgfile", "trghist" ) )
{
   produces<double>( "ElectronWeight" );
   produces<double>( "ElectronGsfWeight" );
   produces<double>( "ElectronCutWeight" );
   produces<double>( "ElectronTriggerWeight" );

   produces<double>( "ElectronWeightup" );
   produces<double>( "ElectronWeightdown" );
}

ElectronWeight::~ElectronWeight()
{
   delete _gsfweight;
   delete _cutweight;
   delete _trgweight;
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
   auto_ptr<double> gsfweightptr( new double(1) );
   auto_ptr<double> cutweightptr( new double(1) );
   auto_ptr<double> trgweightptr( new double(1) );
   auto_ptr<double> weightupptr( new double(1) );
   auto_ptr<double> weightdownptr( new double(1) );

   double& elecweight = *elecweightptr;
   double& gsfweight  = *gsfweightptr;
   double& cutweight  = *cutweightptr;
   double& trgweight  = *trgweightptr;
   double& weightup   = *weightupptr;
   double& weightdown = *weightdownptr;

   const double pt  = _elechandle->at( 0 ).pt();
   const double eta = _elechandle->at( 0 ).eta();
   gsfweight = getweight( _gsfweight, pt, eta );
   cutweight = getweight( _cutweight, pt, eta );
   const double gsferr = getweighterr( _gsfweight, pt, eta );
   const double cuterr = getweighterr( _cutweight, pt, eta );

   const Parameter cutparm( cutweight, cuterr, cuterr );
   const Parameter gsfparm( gsfweight, gsferr, gsferr );
   const Parameter trgparm = getweightParam( _trgweight, pt, eta);
   trgweight = trgparm.CentralValue();
   const Parameter totalparam = cutparm * gsfparm * trgparm ;

   elecweight = totalparam.CentralValue();
   weightup   = totalparam.CentralValue() + totalparam.AbsUpperError();
   weightdown = totalparam.CentralValue() - totalparam.AbsLowerError();

   iEvent.put( elecweightptr, "ElectronWeight" );
   iEvent.put( gsfweightptr,  "ElectronGsfWeight" );
   iEvent.put( cutweightptr,  "ElectronCutWeight" );
   iEvent.put( trgweightptr,  "ElectronTriggerWeight" );
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

Parameter
ElectronWeight::getweightParam( const TEfficiency* eff, double pt, double eta )const
{
   if( !eff ) { return Parameter(1,0,0); }

   eta = min( eta, _trgweight->GetTotalHistogram()->GetXaxis()->GetXmax() - 0.01 );
   eta = max( eta, _trgweight->GetTotalHistogram()->GetXaxis()->GetXmin() + 0.01 );
   pt  = min( pt,  _trgweight->GetTotalHistogram()->GetYaxis()->GetXmax() - 0.01 );

   const int binidx = eff->FindFixBin( eta, pt );

   return Parameter(
      eff->GetEfficiency( binidx ),
      eff->GetEfficiencyErrorUp(binidx ),
      eff->GetEfficiencyErrorLow( binidx )
   );
}


DEFINE_FWK_MODULE( ElectronWeight );

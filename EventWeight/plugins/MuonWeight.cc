/*******************************************************************************
*
*  Filename    : MuonWeight
*  Description : Adding weighting according to muon status
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  Main reference :
*  https://twiki.cern.ch/twiki/bin/view/CMS/MuonWorkInProgressAndPagResults
*
*******************************************************************************/

// system include files
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
// user include files
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/EDMUtils/interface/TFileUtils.hpp"
#include "ManagerUtils/Maths/interface/Parameter.hpp"

#include "TAxis.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TH2D.h"


/*******************************************************************************
*   Class definition
*******************************************************************************/
class MuonWeight : public edm::EDProducer
{
public:
   explicit
   MuonWeight( const edm::ParameterSet& );
   ~MuonWeight();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& ) override;

   // Getting objects from vector of sums
   const edm::EDGetToken _muonsrc;
   edm::Handle<std::vector<pat::Muon> > _muonhandle;

   // Vector representing weights
   const TH2D* _idweight;
   const TH2D* _isoweight;
   const TEfficiency* _trgweight;
   double    getweight( const TH2D*, double pt, double eta ) const;
   double    getweighterr( const TH2D*, double pt, double eta ) const;
   Parameter getweightParam( const TEfficiency*, double pt, double eta ) const;
};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
MuonWeight::MuonWeight( const edm::ParameterSet & iConfig ) :
   _muonsrc( GETTOKEN( iConfig, std::vector<pat::Muon>, "muonsrc" ) ),
   _idweight( (const TH2D*)GETFILEOBJ( iConfig, "idfile", "idhist" ) ),
   _isoweight( (const TH2D*)GETFILEOBJ( iConfig, "isofile", "isohist" ) ),
   _trgweight( (const TEfficiency*)GETFILEOBJ( iConfig, "trgfile", "trghist" ) )
{
   produces<double>( "MuonWeight" );
   produces<double>( "MuonTriggerWeight" );
   produces<double>( "MuonIDWeight" );
   produces<double>( "MuonIsoWeight" );

   // These weight errors are all statistical
   produces<double>( "MuonWeightup" );
   produces<double>( "MuonWeightdown" );
}

MuonWeight::~MuonWeight()
{
}

/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
MuonWeight::produce( edm::Event& iEvent, const edm::EventSetup & iSetup )
{
   if( iEvent.isRealData() ){ return; }// Skipping over data files

   iEvent.getByToken( _muonsrc, _muonhandle );

   if( _muonhandle->size() == 0 ){ return; }// Skipping events that doen't contain muons

   auto_ptr<double> muonweightptr( new double(1) );
   auto_ptr<double> muontrgweightptr( new double(1) );
   auto_ptr<double> muonidweightptr( new double(1) );
   auto_ptr<double> muonisoweightptr( new double(1) );
   auto_ptr<double> muonweightupptr( new double(1) );
   auto_ptr<double> muonweightdownptr( new double(1) );

   const double pt  = _muonhandle->at( 0 ).pt();
   const double eta = _muonhandle->at( 0 ).eta();

   const Parameter trgweight = getweightParam( _trgweight, pt, eta );
   const Parameter idweight(
      getweight( _idweight, pt, eta ),
      getweighterr( _idweight, pt, eta ),
      getweighterr( _idweight, pt, eta )
      );
   const Parameter isoweight(
      getweight( _isoweight, pt, eta ),
      getweighterr( _isoweight, pt, eta ),
      getweighterr( _isoweight, pt, eta )
      );

   const Parameter totalweight = trgweight * idweight * isoweight;

   *muonweightptr     = totalweight.CentralValue();
   *muonweightupptr   = totalweight.CentralValue() + totalweight.AbsUpperError();
   *muonweightdownptr = totalweight.CentralValue() - totalweight.AbsLowerError();
   *muontrgweightptr  = trgweight.CentralValue();
   *muonidweightptr   = idweight.CentralValue();
   *muonisoweightptr  = isoweight.CentralValue();

   iEvent.put( muonweightptr,     "MuonWeight" );
   iEvent.put( muontrgweightptr,  "MuonTriggerWeight" );
   iEvent.put( muonidweightptr,   "MuonIDWeight" );
   iEvent.put( muonisoweightptr,  "MuonIsoWeight" );
   iEvent.put( muonweightupptr,   "MuonWeightup" );
   iEvent.put( muonweightdownptr, "MuonWeightdown" );

}

/******************************************************************************/

double
MuonWeight::getweight( const TH2D* hist, double pt, double eta ) const
{
   if( !hist ){ return 1.; }

   // Re-evaluating to avoid overflow
   eta = fabs( eta );
   eta = min( eta, hist->GetYaxis()->GetXmax() - 0.01 );
   eta = max( eta, hist->GetYaxis()->GetXmin() + 0.01 );
   pt  = min( pt, hist->GetXaxis()->GetXmax() - 0.01 );
   return hist->GetBinContent( hist->FindFixBin( pt, eta ) );
}

double
MuonWeight::getweighterr( const TH2D* hist, double pt, double eta ) const
{
   if( !hist ){ return 0.; }

   // Re-evaluating to avoid overflow
   eta = fabs( eta );
   eta = min( eta, hist->GetYaxis()->GetXmax() - 0.01 );
   eta = max( eta, hist->GetYaxis()->GetXmin() + 0.01 );
   pt  = min( pt, hist->GetXaxis()->GetXmax() - 0.01 );
   return hist->GetBinError( hist->FindFixBin( pt, eta ) );
}

Parameter
MuonWeight::getweightParam( const TEfficiency* eff, double pt, double eta ) const
{
   if( !eff ){ return Parameter( 1, 0, 0 ); }

   eta = fabs( eta );
   eta = min( eta, eff->GetTotalHistogram()->GetXaxis()->GetXmax() - 0.01 );
   eta = max( eta, eff->GetTotalHistogram()->GetXaxis()->GetXmin() + 0.01 );
   pt  = min( pt, eff->GetTotalHistogram()->GetYaxis()->GetXmax() - 0.01 );

   const int binidx = eff->FindFixBin( eta, pt );

   return Parameter(
      eff->GetEfficiency( binidx ),
      eff->GetEfficiencyErrorUp( binidx ),
      eff->GetEfficiencyErrorLow( binidx )
      );
}


DEFINE_FWK_MODULE( MuonWeight );

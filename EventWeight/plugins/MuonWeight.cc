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

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/EDMUtils/interface/TFileUtils.hpp"
#include "ManagerUtils/Maths/interface/Parameter.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

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

  struct WeightObj
  {
    std::string    weightname;
    std::string    objecttype;
    const TObject* weightobj;
  };
  const std::vector<MuonWeight::WeightObj> _weightobjlist;

  std::vector<MuonWeight::WeightObj> GetWeightList( const edm::ParameterSet&, const std::string& ) const;
  std::string                        MakeWeightName( const MuonWeight::WeightObj&  ) const;
  mgr::Parameter                     GetWeight( const MuonWeight::WeightObj&, const double pt, const double eta ) const;

  double         getweight( const TH2D*, double pt, double eta ) const;
  double         getweighterr( const TH2D*, double pt, double eta ) const;
  mgr::Parameter getweightParam( const TEfficiency*, double pt, double eta ) const;
};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
MuonWeight::MuonWeight( const edm::ParameterSet & iConfig ) :
  _muonsrc( GETTOKEN( iConfig, std::vector<pat::Muon>, "muonsrc" ) ),
  _weightobjlist( GetWeightList( iConfig, "weightlist" ) )
{
  produces<double>( "MuonWeight" );
  produces<double>( "MuonWeightup" );
  produces<double>( "MuonWeightdown" );

  for( const auto& weightobj : _weightobjlist ){
    produces<double>( MakeWeightName( weightobj ) );
  }
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

  mgr::Parameter totalweight( 1, 0.00001, 0.00001 );

  if( _muonhandle->size() == 1 ){// MuonWeight is stuck at 1 otherwise
    const double pt  = _muonhandle->at( 0 ).pt();
    const double eta = _muonhandle->at( 0 ).eta();

    for( const auto& weightobj : _weightobjlist ){
      const mgr::Parameter weightparm = GetWeight( weightobj, pt, eta );

      totalweight *= weightparm;
      // cout << totalweight.CentralValue()
      //      << " " << totalweight.AbsUpperError()
      //      << " " << totalweight.AbsLowerError() << endl;

      auto_ptr<double> ptr( new double( weightparm.CentralValue() ) );
      iEvent.put( ptr, MakeWeightName( weightobj ) );
    }

    // cout << totalweight.CentralValue() << endl << endl;
  }

  auto_ptr<double> weightptr( new double( totalweight.CentralValue() ) );
  auto_ptr<double> weightupptr( new double( totalweight.CentralValue()+totalweight.AbsUpperError() ) );
  auto_ptr<double> weightdownptr( new double( totalweight.CentralValue()-totalweight.AbsLowerError() ) );

  iEvent.put( weightptr,     "MuonWeight" );
  iEvent.put( weightupptr,   "MuonWeightup" );
  iEvent.put( weightdownptr, "MuonWeightdown" );
}

/******************************************************************************/

double
MuonWeight::getweight( const TH2D* hist, double pt, double eta ) const
{
  if( !hist ){ return 1.; }

  // Re-evaluating to avoid overflow
  eta = fabs( eta );
  eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
  eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
  pt  = min( pt, hist->GetYaxis()->GetXmax() - 0.01 );

  // cout << eta << " " << pt << " " << hist->FindFixBin( pt, eta ) << " "
  //      << hist->GetBinContent( hist->FindFixBin( pt, eta ) ) << endl;

  return hist->GetBinContent( hist->FindFixBin( eta, pt ) );
}

double
MuonWeight::getweighterr( const TH2D* hist, double pt, double eta ) const
{
  if( !hist ){ return 0.; }

  // Re-evaluating to avoid overflow
  eta = fabs( eta );
  eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
  eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
  pt  = min( pt, hist->GetYaxis()->GetXmax() - 0.01 );
  return hist->GetBinError( hist->FindFixBin( eta, pt ) );
}

mgr::Parameter
MuonWeight::getweightParam( const TEfficiency* eff, double pt, double eta ) const
{
  if( !eff ){ return mgr::Parameter( 1, 0, 0 ); }

  eta = fabs( eta );
  eta = min( eta, eff->GetTotalHistogram()->GetXaxis()->GetXmax() - 0.01 );
  eta = max( eta, eff->GetTotalHistogram()->GetXaxis()->GetXmin() + 0.01 );
  pt  = min( pt, eff->GetTotalHistogram()->GetYaxis()->GetXmax() - 0.01 );

  const int binidx = eff->FindFixBin( eta, pt );

  return mgr::Parameter(
    eff->GetEfficiency( binidx ),
    eff->GetEfficiencyErrorUp( binidx ),
    eff->GetEfficiencyErrorLow( binidx )
    );
}

/*******************************************************************************
*   Objects for getting objects
*******************************************************************************/
std::vector<MuonWeight::WeightObj>
MuonWeight::GetWeightList(
  const edm::ParameterSet& iConfig,
  const std::string&       tag ) const
{
  vector<MuonWeight::WeightObj> ans;

  for( const auto& pset : iConfig.getParameter<std::vector<edm::ParameterSet> >( tag ) ){
    MuonWeight::WeightObj obj;
    obj.weightname = pset.getParameter<std::string>( "weightname" );
    obj.objecttype = pset.getParameter<std::string>( "objecttype" );
    obj.weightobj  = GETFILEOBJ( pset, "file", "fileobj" );
    ans.push_back( obj );
  }

  return ans;
}

/******************************************************************************/

std::string
MuonWeight::MakeWeightName( const MuonWeight::WeightObj& x ) const
{
  return "Muon" + x.weightname;
}

/******************************************************************************/

mgr::Parameter
MuonWeight::GetWeight( const MuonWeight::WeightObj& x, const double pt, const double eta ) const
{
  if( x.objecttype == "TH2D" ){
    return mgr::Parameter(
      getweight( (const TH2D*)x.weightobj, pt, eta ),
      getweighterr( (const TH2D*)x.weightobj, pt, eta ),
      getweighterr( (const TH2D*)x.weightobj, pt, eta )
      );
  } else if( x.objecttype == "TEfficiency" ){
    return getweightParam(
      (const TEfficiency*)x.weightobj, pt, eta
      );
  } else {
    throw std::invalid_argument( "Unknown type of weight object!" );
    return mgr::Parameter( 0, 0, 0 );
  }

}

DEFINE_FWK_MODULE( MuonWeight );

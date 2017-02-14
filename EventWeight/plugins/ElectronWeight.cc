/*******************************************************************************
*
*  Filename    : ElectronWeight
*  Description : Adding weighting according to electron status
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
#include "TEfficiency.h"
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
  struct WeightObj
  {
    std::string    weightname;
    std::string    objecttype;
    const TObject* weightobj;
  };
  const std::vector<WeightObj> _weightobjlist;


  std::vector<ElectronWeight::WeightObj> GetWeightList( const edm::ParameterSet&, const std::string& tag ) const;
  std::string                            MakeWeightName( const ElectronWeight::WeightObj& ) const;
  mgr::Parameter                         GetWeight( const ElectronWeight::WeightObj&, const double pt, const double eta ) const;

  // raw functions for root object interaction
  double getweight( const TH2D*, double pt, double eta ) const;
  double getweighterr( const TH2D*, double pt, double eta ) const;

  mgr::Parameter getweightParam( const TEfficiency*, double pt, double eta ) const;
};

using namespace edm;
using namespace std;

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
ElectronWeight::ElectronWeight( const edm::ParameterSet & iConfig ) :
  _elecsrc( GETTOKEN( iConfig, std::vector<pat::Electron>, "elecsrc" ) ),
  _weightobjlist( GetWeightList( iConfig, "weightlist" ) )
{
  produces<double>( "ElectronWeight" );
  produces<double>( "ElectronWeightup" );
  produces<double>( "ElectronWeightdown" );

  for( const auto& weightobj : _weightobjlist ){
    produces<double>( MakeWeightName( weightobj ) );
  }
}

ElectronWeight::~ElectronWeight()
{
  for( auto& weightobj : _weightobjlist ){
    delete weightobj.weightobj;
  }
}

/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
ElectronWeight::produce( edm::Event& iEvent, const edm::EventSetup & iSetup )
{
  if( iEvent.isRealData() ){ return; }// Skipping over data files

  iEvent.getByToken( _elecsrc, _elechandle );


  mgr::Parameter totalweight( 1, 0.00001, 0.00001 );

  if( _elechandle->size() ){// Weight will remain 1.0 otherwise
    const double pt  = _elechandle->at( 0 ).pt();
    const double eta = _elechandle->at( 0 ).eta();

    for( const auto& weightobj : _weightobjlist ){
      const mgr::Parameter weightparm = GetWeight( weightobj, pt, eta );
      totalweight *= weightparm;

      auto_ptr<double> ptr( new double( weightparm.CentralValue() ) );
      iEvent.put( ptr, MakeWeightName( weightobj ) );
    }
  }

  auto_ptr<double> weightptr( new double( totalweight.CentralValue() ) );
  auto_ptr<double> weightupptr( new double( totalweight.CentralValue()+totalweight.AbsUpperError() ) );
  auto_ptr<double> weightdownptr( new double( totalweight.CentralValue()-totalweight.AbsLowerError() ) );

  iEvent.put( weightptr,     "ElectronWeight" );
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

  // cout << eta << " " << pt << " " << hist->FindFixBin( pt, eta ) << " "
  //      << hist->GetBinContent( hist->FindFixBin( pt, eta ) ) << endl;

  return hist->GetBinContent( hist->FindFixBin( eta, pt ) );
}

/******************************************************************************/

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

/******************************************************************************/

mgr::Parameter
ElectronWeight::getweightParam( const TEfficiency* eff, double pt, double eta ) const
{
  if( !eff ){ return mgr::Parameter( 1, 0, 0 ); }

  eta = min( eta, eff->GetTotalHistogram()->GetXaxis()->GetXmax() - 0.01 );
  eta = max( eta, eff->GetTotalHistogram()->GetXaxis()->GetXmin() + 0.01 );
  pt  = min( pt,  eff->GetTotalHistogram()->GetYaxis()->GetXmax() - 0.01 );

  const int binidx = eff->FindFixBin( eta, pt );

  return mgr::Parameter(
    eff->GetEfficiency( binidx ),
    eff->GetEfficiencyErrorUp( binidx ),
    eff->GetEfficiencyErrorLow( binidx )
    );
}

/*******************************************************************************
*   Helper function for getting weight configuration from input parameters
*******************************************************************************/
vector<ElectronWeight::WeightObj>
ElectronWeight::GetWeightList(
  const edm::ParameterSet& iConfig,
  const std::string&       tag ) const
{
  std::vector<ElectronWeight::WeightObj> ans;

  for( const auto& pset : iConfig.getParameter<std::vector<edm::ParameterSet> >( tag ) ){
    ElectronWeight::WeightObj obj;
    obj.weightname = pset.getParameter<std::string>( "weightname" );
    obj.objecttype = pset.getParameter<std::string>( "objecttype" );
    obj.weightobj  = GETFILEOBJ( pset, "file", "fileobj" );
    ans.push_back( obj );
  }

  return ans;
}

std::string
ElectronWeight::MakeWeightName( const ElectronWeight::WeightObj& x ) const
{
  return "Electron" + x.weightname;
}

mgr::Parameter
ElectronWeight::GetWeight(
  const ElectronWeight::WeightObj& x,
  const double                     pt,
  const double                     eta
  ) const
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


DEFINE_FWK_MODULE( ElectronWeight );

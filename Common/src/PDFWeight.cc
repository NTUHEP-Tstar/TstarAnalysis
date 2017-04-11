/*******************************************************************************
*
*  Filename    : PDFWeight.cc
*  Description : Implementations for calculation PDF Weights
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
* Access methods:
* https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideDataFormatGeneratorInterface
*
* One sigma error for single pdf set:
* https://nnpdf.hepforge.org/html/tutorial.html
*
* Parsing powered by boost property_tree and rapidxml parser
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"

#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include "TFile.h"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

using namespace std;
namespace bpt = boost::property_tree;


/*******************************************************************************
*   GetPdfIdGrouping - Per run event, no generator case handling here
*******************************************************************************/
vector<vector<unsigned> >
GetPdfIdGrouping( const std::string& filename )
{
  stringstream xmlstream;
  bpt::ptree xmltree;

  fwlite::Run run( TFile::Open( filename.c_str() ) );
  fwlite::Handle<LHERunInfoProduct> runinfo;

  run.toBegin();
  try {
    runinfo.getByLabel( run, "externalLHEProducer" );

    for( auto info = runinfo->headers_begin(); info != runinfo->headers_end(); ++info ){
      if( info->tag() == "initrwgt" ){
        for( const auto& line : info->lines() ){
          xmlstream << line;
        }
      }
    }
  } catch( std::exception ){
    // Do nothing, use empty string for parsing
  }

  // Creating string stream

  read_xml( xmlstream, xmltree );

  // Parsing contents
  vector<vector<unsigned> > ans;

  for( const auto& groupinfo : xmltree ){
    vector<unsigned> idgroup;

    for( const auto& weightinfo : groupinfo.second ){
      if( weightinfo.first == "weight"  ){
        idgroup.push_back( stoi( weightinfo.second.get<string>( "<xmlattr>.id" ) ) );
      }
    }

    ans.push_back( idgroup );
  }

  return ans;
}


vector<vector<unsigned> >
GetPdfIdGrouping( const mgr::SampleMgr& sample )
{
  return GetPdfIdGrouping( sample.GlobbedFileList().front() );
}

/*******************************************************************************
*   GetPdfWeightError - Generator case handling here
*******************************************************************************/

double
GetPdfWeightError( const fwlite::EventBase& ev, const vector<vector<unsigned> >& pdfidgroup )
{
  if( pdfidgroup.empty() ){ return 0; } // no pdf information

  vector<unsigned> idlist;
  fwlite::Handle<LHEEventProduct> evthandle;
  evthandle.getByLabel( ev, "externalLHEProducer" );

  if( pdfidgroup.front().front() == 1 ){ // Madgraph
    idlist = GetMadGraphIdList( ev, pdfidgroup );
    return GetPdfWeightError( ev, idlist, evthandle.ref().weights().front().wgt );
  } else { // Powheg, amcatnlo
    idlist = GetPowHegIdList( ev, pdfidgroup );
    return GetPdfWeightError( ev, idlist, evthandle.ref().originalXWGTUP() );
  }
}

/******************************************************************************/
double
GetScaleWeightError( const fwlite::EventBase& ev, const vector<vector<unsigned> >& pdfidgroup )
{
  return (GetScaleWeightUpError(ev,pdfidgroup)+GetScaleWeightDownError(ev,pdfidgroup))/2;
}

/******************************************************************************/

double
GetScaleWeightUpError( const fwlite::EventBase& ev, const vector<vector<unsigned> >& pdfidgroup )
{
  if( pdfidgroup.empty() ){ return 0; } // no pdf information

  vector<unsigned> idlist;
  fwlite::Handle<LHEEventProduct> evthandle;
  evthandle.getByLabel( ev, "externalLHEProducer" );

  if( pdfidgroup.front().front() == 1 ){ // Madgraph
    idlist = GetMadGraphScaleIdList( ev );
    return GetScaleWeightUpError( ev, idlist, evthandle.ref().weights().front().wgt );
  } else { // Powheg, amcatnlo
    idlist = GetPowHegScaleIdList( ev );
    return GetScaleWeightUpError( ev, idlist, evthandle.ref().originalXWGTUP() );
  }
  return 0;

}

/******************************************************************************/

double
GetScaleWeightDownError( const fwlite::EventBase& ev, const vector<vector<unsigned> >& pdfidgroup )
{
  if( pdfidgroup.empty() ){ return 0; } // no pdf information

  vector<unsigned> idlist;
  fwlite::Handle<LHEEventProduct> evthandle;
  evthandle.getByLabel( ev, "externalLHEProducer" );

  if( pdfidgroup.front().front() == 1 ){ // Madgraph
    idlist = GetMadGraphScaleIdList( ev );
    return GetScaleWeightDownError( ev, idlist, evthandle.ref().weights().front().wgt );
  } else { // Powheg, amcatnlo
    idlist = GetPowHegScaleIdList( ev );
    return GetScaleWeightDownError( ev, idlist, evthandle.ref().originalXWGTUP() );
  }
  return 0;

}

/******************************************************************************/

vector<unsigned>
GetMadGraphIdList(
  const fwlite::EventBase&         ev,
  const vector<vector<unsigned> >& pdfidgroup
  )
{
  fwlite::Handle<LHEEventProduct> evthandle;
  evthandle.getByLabel( ev, "externalLHEProducer" );

  unsigned targetid = -1;

  for( const auto& weightinfo : evthandle.ref().weights() ){
    const unsigned thisid   = stoi( weightinfo.id );
    if( thisid == 111 ){
      targetid = thisid;
      break;
    }
  }

  for( const auto& idgroup : pdfidgroup ){
    if( find( idgroup.begin(), idgroup.end(), targetid ) != idgroup.end() ){
      return idgroup;
    }
  }

  return vector<unsigned>(); // returning blank if not found
}

vector<unsigned>
GetMadGraphScaleIdList( const fwlite::EventBase& ev )
{
  return {1, 2, 3, 4, 5, 7, 9}; // skipping where on scales up and the other scales down
}

/******************************************************************************/

vector<unsigned>
GetPowHegScaleIdList( const fwlite::EventBase& ev )
{
  return {1001, 1002, 1003, 1004, 1005, 1007, 1008, 1009};
}

vector<unsigned>
GetPowHegIdList(
  const fwlite::EventBase&         ev,
  const vector<vector<unsigned> >& pdfidgroup
  )
{
  vector<unsigned> ans;

  for( const auto& id : pdfidgroup.at(1) ){
    if( int(id/1000) == int(pdfidgroup.at(1).front() / 1000) ){
      ans.push_back( id );
    }
  }

  return ans;
}


/******************************************************************************/

double
GetPdfWeightError(
  const fwlite::EventBase& ev,
  const vector<unsigned>&  idlist,
  const double             centralweight
  )
{
  fwlite::Handle<LHEEventProduct> evthandle;
  evthandle.getByLabel( ev, "externalLHEProducer" );

  if( idlist.size() == 0 ){ return 0; }

  double numofweights = idlist.size();
  double sumofweights = 0;
  double sumofsquares = 0;

  for( const auto& weightinfo : evthandle.ref().weights() ){
    const unsigned thisid   = stoi( weightinfo.id );
    const double thisweight = weightinfo.wgt / centralweight;
    if( find( idlist.begin(), idlist.end(), thisid ) != idlist.end() ){
      sumofweights += thisweight;
      sumofsquares += thisweight * thisweight;
    }
  }

  sumofsquares /= numofweights;
  sumofweights /= numofweights;
  return sqrt( sumofsquares - sumofweights*sumofweights );
}

/******************************************************************************/

double
GetScaleWeightUpError(
  const fwlite::EventBase& ev,
  const vector<unsigned>&  idlist,
  const double             centralweight
  )
{
  fwlite::Handle<LHEEventProduct> evthandle;
  evthandle.getByLabel( ev, "externalLHEProducer" );

  if( idlist.empty() ){ return 0; }

  double maxweight = 1;

  for( const auto& weightinfo : evthandle.ref().weights() ){
    const unsigned thisid   = stoi( weightinfo.id );
    const double thisweight = weightinfo.wgt / centralweight;
    if( find( idlist.begin(), idlist.end(), thisid ) != idlist.end() ){
      maxweight = std::max( maxweight, thisweight );
    }
  }

  return maxweight - 1 ;
}

/******************************************************************************/

double
GetScaleWeightDownError(
  const fwlite::EventBase& ev,
  const vector<unsigned>&  idlist,
  const double             centralweight
  )
{
  fwlite::Handle<LHEEventProduct> evthandle;
  evthandle.getByLabel( ev, "externalLHEProducer" );

  if( idlist.empty() ){ return 0; }

  double minweight = 1;

  for( const auto& weightinfo : evthandle.ref().weights() ){
    const unsigned thisid   = stoi( weightinfo.id );
    const double thisweight = weightinfo.wgt / centralweight;
    if( find( idlist.begin(), idlist.end(), thisid ) != idlist.end() ){
      minweight = std::min( minweight, thisweight );
    }
  }

  return 1-minweight ;
}

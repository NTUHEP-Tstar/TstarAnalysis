/*******************************************************************************
*
*  Filename    : GetEventWeight.hpp
*  Description : Utility functions for extracting event weight
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  The functions listed here are specialized for the event weights
*  produced in the class defined in plugins/EventWeighter.cc
*  In the case that the product wasn't found. it will always return 1.0
*
*******************************************************************************/
#ifndef TSTARANALYSIS_EVENTWEIGHT_GETEVENTWEIGHT_HPP
#define TSTARANALYSIS_EVENTWEIGHT_GETEVENTWEIGHT_HPP

#include "DataFormats/FWLite/interface/EventBase.h"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include <vector>

extern double GetEventWeight( const fwlite::EventBase& );

extern double GetWeightSign( const fwlite::EventBase& );

extern double GetPileupWeight( const fwlite::EventBase& );
extern double GetPileupWeightBestFit( const fwlite::EventBase& );
extern double GetPileupWeightXsecup( const fwlite::EventBase& );
extern double GetPileupWeightXsecdown( const fwlite::EventBase& );

extern double GetBtagWeight( const fwlite::EventBase& );
extern double GetBtagWeightUp( const fwlite::EventBase& );
extern double GetBtagWeightDown( const fwlite::EventBase& );

extern double GetElectronWeight( const fwlite::EventBase& );
extern double GetElectronCutWeight( const fwlite::EventBase& );
extern double GetElectronTrackWeight( const fwlite::EventBase& );
extern double GetElectronTriggerWeight( const fwlite::EventBase& );
extern double GetElectronWeightUp( const fwlite::EventBase& );
extern double GetElectronWeightDown( const fwlite::EventBase& );

extern double GetMuonWeight( const fwlite::EventBase& );
extern double GetMuonWeightUp( const fwlite::EventBase& );
extern double GetMuonWeightDown( const fwlite::EventBase& );
extern double GetMuonTriggerWeight( const fwlite::EventBase& );
extern double GetMuonIDWeight( const fwlite::EventBase& );
extern double GetMuonIsoWeight( const fwlite::EventBase& );

// Getting the raw, unnormalized double associated with single event
extern double GetEventTopPtWeight( const fwlite::EventBase& );

// Getting normalized version
extern double GetSampleEventTopPtWeight( const mgr::SampleMgr&, const fwlite::EventBase& );

// Whether or not to skip event for ttbar
extern bool SkipTTbar( const fwlite::EventBase& );

/*******************************************************************************
*   PDF weights could not be cached!
*   This is because you cannot access per-run information (PDF ID LIST) before
*   per-event information (PDF event weights) in the full CMSSW Framework
*
*   ** GetPdfIdGrouping - parse the pdf id information found in per-run information
*   and store the results as a group of integers
*
*   ** GetPdfWeightError - passing in a single event and the grouping information
*   stored previously to obtain a one sigma error (as indicated with the code snippet
*   found here:
*   https://nnpdf.hepforge.org/html/tutorial.html
*
*   These functions are implemented in src/PDFWeight.cc
*******************************************************************************/
extern std::vector<std::vector<unsigned> > GetPdfIdGrouping( const std::string& filename );

extern std::vector<std::vector<unsigned> > GetPdfIdGrouping( const mgr::SampleMgr& );

extern double GetPdfWeightError(
  const fwlite::EventBase&                   ev,
  const std::vector<std::vector<unsigned> >& pdfidgroup
  );

extern std::vector<unsigned> GetMadGraphIdList(
  const fwlite::EventBase&                   ev,
  const std::vector<std::vector<unsigned> >& pdfidgroup
  );

extern std::vector<unsigned> GetPowHegIdList(
  const fwlite::EventBase&                   ev,
  const std::vector<std::vector<unsigned> >& pdfidgroup
  );

extern double GetPdfWeightError(
  const fwlite::EventBase&     ev,
  const std::vector<unsigned>& idlist,
  const double                 centralweight
  );

/*******************************************************************************
*
*   Scale variation  - a variation of PDF uncertainty grouping
*
*   * GetScaleWeightError - Get the weight error due to scaleing
*
*   * Get<Generator>ScaleList - returng the scale
*
*******************************************************************************/
extern double GetScaleWeightError(
  const fwlite::EventBase&                   ev,
  const std::vector<std::vector<unsigned> >& pdfidgroup
  );

extern double GetScaleWeightUpError(
  const fwlite::EventBase&                   ev,
  const std::vector<std::vector<unsigned> >& pdfidgroup
  );

extern double GetScaleWeightDownError(
  const fwlite::EventBase&                   ev,
  const std::vector<std::vector<unsigned> >& pdfidgroup
  );

extern std::vector<unsigned> GetMadGraphScaleIdList( const fwlite::EventBase& ev );

extern std::vector<unsigned> GetPowHegScaleIdList( const fwlite::EventBase& ev );

extern double GetScaleWeightUpError(
  const fwlite::EventBase&     ev,
  const std::vector<unsigned>& idlist,
  const double                 centralweight
  );

extern double GetScaleWeightDownError(
  const fwlite::EventBase&     ev,
  const std::vector<unsigned>& idlist,
  const double                 centralweight
  );


#endif/* end of include guard: TSTARANALYSIS_EVENTWEIGHT_GETEVENTWEIGHT_HPP */

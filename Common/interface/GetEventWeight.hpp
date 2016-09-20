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

// Getting master event (product of everything)
extern double GetEventWeight( const fwlite::EventBase& );

extern double GetPileupWeight( const fwlite::EventBase& );
extern double GetPileupWeight71260( const fwlite::EventBase& );
extern double GetPileupWeight62000( const fwlite::EventBase& );

extern double GetBtagWeight( const fwlite::EventBase& );
extern double GetBtagWeightUp( const fwlite::EventBase& );
extern double GetBtagWeightDown( const fwlite::EventBase& );

extern double GetElectronWeight( const fwlite::EventBase& );
extern double GetElectronCutWeight( const fwlite::EventBase& );
extern double GetElectronMediumCutWeight( const fwlite::EventBase& );
extern double GetElectronTightCutWeight( const fwlite::EventBase& );
extern double GetElectronTrackWeight( const fwlite::EventBase& );
extern double GetElectronWeightUp( const fwlite::EventBase& );
extern double GetElectronWeightDown( const fwlite::EventBase& );

extern double GetEventTopPtWeight( const fwlite::EventBase& );

// special function for getting sample top pt weight
extern double SetSampleTopPtWeight( mgr::SampleMgr& );
extern double GetSampleTopPtWeight( const mgr::SampleMgr& );
extern double GetSampleEventTopPtWeight( const mgr::SampleMgr& , const fwlite::EventBase& );

#endif/* end of include guard: TSTARANALYSIS_EVENTWEIGHT_GETEVENTWEIGHT_HPP */

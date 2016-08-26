/*******************************************************************************
 *
 *  Filename    : GetEventWeight.hpp
 *  Description : Utility functions for extracting event weight
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  The functions listed here are specialized for the event weights
 *  produced in the class defined in plugins/EventWieghter.cc
*******************************************************************************/
#ifndef TSTARANALYSIS_EVENTWEIGHT_GETEVENTWEIGHT_HPP
#define TSTARANALYSIS_EVENTWEIGHT_GETEVENTWEIGHT_HPP

#include "DataFormats/FWLite/interface/EventBase.h"

// Getting master event (product of everything)
extern double GetEventWeight ( const fwlite::EventBase& );

extern double GetPileupWeight( const fwlite::EventBase& );

extern double GetElectronWeight     ( const fwlite::EventBase& );
extern double GetElectronCutWeight  ( const fwlite::EventBase& );
extern double GetElectronTrackWeight( const fwlite::EventBase& );

#endif /* end of include guard: TSTARANALYSIS_EVENTWEIGHT_GETEVENTWEIGHT_HPP */
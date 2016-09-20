/*******************************************************************************
*
*  Filename    : ComputeSelectionEff.hpp
*  Description : Computing selection efficiencies of sample
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  This assumes that you are using the results produced by the producers
*  Defined in the process of RunSequence/cmsrun/
*
*******************************************************************************/
#ifndef TSTARANALYSIS_EVENTWEIGHT_COMPUTESELECTIONEFF_HPP
#define TSTARANALYSIS_EVENTWEIGHT_COMPUTESELECTIONEFF_HPP

#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"

/*******************************************************************************
*   GetOriginalEventCount - access the weights sums stored per run
*******************************************************************************/
double SetOriginalEventCount( mgr::SampleMgr& );
double SetSelectedEventCount( mgr::SampleMgr& );
double GetOriginalEventCount( const mgr::SampleMgr& );
double GetSelectedEventCount( const mgr::SampleMgr& );

/*******************************************************************************
*   ** ComputeSelectionEff - Using the results of the previous functions
*                            to compute selection efficiencies
*******************************************************************************/
const Parameter& ComputeSelectionEff( mgr::SampleMgr& );

/*******************************************************************************
*   ** Getting the sample weight to apply to all events
*******************************************************************************/
double GetSampleWeight( const mgr::SampleMgr& );

#endif/* end of include guard: TSTARANALYSIS_EVENTWEIGHT_COMPUTESELECTIONEFF_HPP */

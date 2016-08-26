/*******************************************************************************
 *
 *  Filename    : ComputeSelectionEff.hpp
 *  Description : Computing selection efficiencies of sample
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  This assumes that you are using a sum of positive and negative events
 *  store in each run, as well as the
 *
*******************************************************************************/
#ifndef TSTARANALYSIS_EVENTWEIGHT_COMPUTESELECTIONEFF_HPP
#define TSTARANALYSIS_EVENTWEIGHT_COMPUTESELECTIONEFF_HPP

#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "ManagerUtils/Maths/interface/Parameter.hpp"

double    GetOriginalEventCount( const mgr::SampleMgr& );
double    GetSelectedEventCount( const mgr::SampleMgr& );
const Parameter& ComputeSelectionEff( mgr::SampleMgr& );

#endif /* end of include guard: TSTARANALYSIS_EVENTWEIGHT_COMPUTESELECTIONEFF_HPP */

/*******************************************************************************
*
*  Filename    : InitSample.hpp
*  Description : Prototyping for functions initializating sample settings
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMMON_INITSAMPLESTATIC_HPP
#define TSTARANALYSIS_COMMON_INITSAMPLESTATIC_HPP

#include "TstarAnalysis/Common/interface/TstarNamer.hpp"

#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
/*******************************************************************************
*   InitSampleStatic - Static variable initialization
*******************************************************************************/
extern void InitSampleStatic( const TstarNamer& );

/*******************************************************************************
*   InitSample - Loading variables that should be store by workflow
*******************************************************************************/
extern void InitSample( mgr::SampleMgr&  );

#endif/* end of include guard: TSTARANALYSIS_COMMON_INITSAMPLESTATIC_HPP */

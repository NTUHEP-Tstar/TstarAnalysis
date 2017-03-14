/*******************************************************************************
*
*  Filename    : MakeMCCompare.hpp
*  Description : Plotting control flow for MC Comparison
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_MAKEMCCOMPARE_HPP
#define TSTARANALYSIS_COMPAREDATAMC_MAKEMCCOMPARE_HPP

#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"
#include <vector>

extern void MakeMCCompare( std::vector<SampleErrHistMgr*>& samplelist );

#endif /* end of include guard:  */

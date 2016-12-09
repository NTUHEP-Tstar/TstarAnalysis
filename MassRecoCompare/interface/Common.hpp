/*******************************************************************************
*
*  Filename    : Bin_Common.hpp
*  Description : Common functions and global variables to for binaries
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_TSTARMASSRECO_BINCOMMON_HPP
#define TSTARANALYSIS_TSTARMASSRECO_BINCOMMON_HPP

#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/CompareHistMgr.hpp"


/*******************************************************************************
*   Global variables
*******************************************************************************/
extern TstarNamer reconamer;

/*******************************************************************************
*   Plotting functions
*******************************************************************************/
extern void ComparePlot( const std::string&, const std::vector<CompareHistMgr*> );
extern void MatchPlot( CompareHistMgr* );


#endif/* end of include guard: TSTARANALYSIS_TSTARMASSRECO_BINCOMMON_HPP */

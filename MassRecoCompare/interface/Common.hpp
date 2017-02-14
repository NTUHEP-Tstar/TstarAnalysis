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
#include "TstarAnalysis/MassRecoCompare/interface/KeysCompMgr.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/KeysErrMgr.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include <string>

/*******************************************************************************
*   Global variables, see Common.cc
*******************************************************************************/
extern TstarNamer reconamer;
extern const Color_t Color_Sequence[5];

/*******************************************************************************
*   Modules retrieval functions, see Common.cc
*******************************************************************************/
extern std::string ProcessName();
extern std::string LabelName( const std::string& module );

/*******************************************************************************
*   Common functions and extern variables
*******************************************************************************/
extern const std::vector<tstar::Particle_Label> fitlabellist;
extern const std::vector<tstar::Particle_Label> truthlabellist;
extern unsigned NumCorrectAssign( const RecoResult& );

/*******************************************************************************
*   Method comparison plotting functions, see ComparePlot.cc
*******************************************************************************/
extern void ComparePlot( const std::string&, const std::vector<CompareHistMgr*> );
extern void MatchPlot( CompareHistMgr* );
extern void MatchPlot1D( CompareHistMgr* );
extern void MatchPlot2D( CompareHistMgr* );
extern void MatchMassPlot( CompareHistMgr* );

/*******************************************************************************
*   Keys Comparison plotting functions, see KeysPlot.cc
*******************************************************************************/
extern void MakeKeysPlots( KeysCompMgr* );

/*******************************************************************************
*   Keys Comparison for errors
*******************************************************************************/
extern void MakeKeysErrPlots( KeysErrMgr* );

#endif/* end of include guard: TSTARANALYSIS_TSTARMASSRECO_BINCOMMON_HPP */

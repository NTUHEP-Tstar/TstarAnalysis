/*******************************************************************************
*
*  Filename    : MakeTable.hpp
*  Description : Functions for making the latex summary tables.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_MAKETABLE_HPP
#define TSTARANALYSIS_COMPAREDATAMC_MAKETABLE_HPP

#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleTableMgr.hpp"
#include <fstream>
#include <vector>


/*******************************************************************************
*   Main control flows to be called by main functions
*
*   ** InitSampleForTables - Caching required variables for samples
*
*   ** SummaryComplete - Every sample is treated individually.
*
*   ** SummarySignal - list every signal sample
*
*   ** SummaryBKGBriefBreif - Background is bunched up into groups
*
*   ** SummaryMCLumi - print the MC sample as a list of equivalent luminosities.
*
*******************************************************************************/
extern void SummaryComplete(
   const std::vector<SampleTableMgr>& siglist,
   const std::vector<SampleTableMgr>& bkglist,
   const SampleTableMgr&              data
   );

extern void SummaryBrief(
   const std::vector<SampleTableMgr>& siglist,
   const std::vector<SampleTableMgr>& bkglist,
   const SampleTableMgr&              data
);

extern void SummaryMCLumi(
   const std::vector<SampleTableMgr>& siglist,
   const std::vector<SampleTableMgr>& bkglist
   );

/*******************************************************************************
*   Initialization Helper functions
*
*   ** InitGroupString - Recrawling group configuration json file and adding
*                        string caches to sample mgr list
*   ** InitSampleString - Actually defining what sample stringto cache
*
*******************************************************************************/
extern mgr::Parameter GetWeightError( const mgr::SampleMgr& );
extern std::string  AddMathBrace( const std::string& );
extern std::string  XSecStr( const mgr::Parameter& );

/*******************************************************************************
*   Helper function for SummaryComplete
*   * OpenSelecFile   - open files and print latex table format header s
*   * PrintSampleLine - pritn line for single sample
*   * PrintCount      - Print only yield part ( for mc summary and data )
*******************************************************************************/
extern std::string SampleLine( const mgr::SampleMgr& );
extern std::string CountLine( const std::string&, const mgr::Parameter& );

/*******************************************************************************
*   Helper functions for simple summary files
*******************************************************************************/
extern std::string  SimpleLine( const SampleTableMgr& );
extern std::string  SimpleCount( const std::string&, const mgr::Parameter& );

/*******************************************************************************
*   Helper functions for Lumi files
*******************************************************************************/
extern std::string  LumiLine( const mgr::SampleMgr& );

#endif/* end of include guard: TSTARANALYSIS_COMPAREDATAMC_MAKETABLE_HPP */

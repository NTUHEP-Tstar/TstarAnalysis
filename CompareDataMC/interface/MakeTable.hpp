/*******************************************************************************
*
*  Filename    : MakeTable.hpp
*  Description : Functions for making the latex summary tables.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_MAKETABLE_HPP
#define TSTARANALYSIS_COMPAREDATAMC_MAKETABLE_HPP

#include "ManagerUtils/Maths/interface/ParameterFormat.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/SampleTableMgr.hpp"
#include <cstdio>
#include <cstdlib>
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
extern Parameter GetWeightError( const mgr::SampleMgr& );

/*******************************************************************************
*   Common Helper function
*
*   * Print latex \hline to file
*
*   * Print latex \end{tabular} and close file
*
*******************************************************************************/
extern void PrintHline( FILE* );
extern void CloseTableFile( FILE* );

/*******************************************************************************
*   Helper function for SummaryComplete
*   * OpenSelecFile   - open files and print latex table format header s
*   * PrintSampleLine - pritn line for single sample
*   * PrintCount      - Print only yield part ( for mc summary and data )
*******************************************************************************/
extern FILE* OpenSelecFile( const std::string& );
extern void  PrintSampleLine( FILE*, const mgr::SampleMgr& );
extern void  PrintCount( FILE*, const std::string&, const Parameter& );

/*******************************************************************************
*   Helper functions for simple summary files
*******************************************************************************/
extern FILE* OpenSimpleFile( const std::string& );
extern void  PrintSimpleLine( FILE*, const SampleTableMgr& );
extern void  PrintSimpleCount( FILE*, const std::string&, const Parameter& );

/*******************************************************************************
*   Helper functions for Lumi files
*******************************************************************************/
extern FILE* OpenLumiFile( const std::string& );
extern void  PrintLumiLine( FILE*, const mgr::SampleMgr& );

#endif/* end of include guard: TSTARANALYSIS_COMPAREDATAMC_MAKETABLE_HPP */

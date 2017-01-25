/*******************************************************************************
*
*  Filename    : MakeErrHist.hpp
*  Description : Function for production graphs with correct error representation
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_MAKEERRHIST_HPP
#define TSTARANALYSIS_COMPAREDATAMC_MAKEERRHIST_HPP

#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"

#include "THStack.h"

extern void PlotErrCompare(
   const std::string&                    listname,
   const std::vector<SampleErrHistMgr*>& samplelist,
   const std::string&  histname,
   const ErrorSource&  err
   );


extern void MakeFullComparePlot(
   SampleErrHistMgr* data,
   std::vector<SampleErrHistMgr*>& bkg,
   SampleErrHistMgr* sig,
   const std::string& label = ""
);

extern void Normalize(
   SampleErrHistMgr*               data,
   std::vector<SampleErrHistMgr*>& bg
   );


/*******************************************************************************
*   Helper functions
*******************************************************************************/
extern mgr::Parameter ExpectedYield(
   const std::vector<SampleErrHistMgr*>& samplelist );

extern TH1D* MakeSumHistogram(
   const std::vector<SampleErrHistMgr*>& samplelist,
   const std::string&                    histname
   );

extern void SetBkgColor( std::vector<SampleErrHistMgr*>& );

extern THStack* MakeBkgStack(
   const std::vector<SampleErrHistMgr*>& samplelist,
   const std::string&                    histname
);

extern TH1D* MakeBkgError(
   const std::vector<SampleErrHistMgr*>& samplelist,
   const std::string&    histname
);

#endif/* end of include guard: TSTARANALySIS_COMPAREDATAMC_MAKEERRHIST_HPP */

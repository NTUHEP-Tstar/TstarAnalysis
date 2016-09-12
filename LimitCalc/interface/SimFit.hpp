/*******************************************************************************
 *
 *  Filename    : SimFit.hpp
 *  Description : Forward declaration of all functions related with the SimFit
                  Method
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef TSTARANALYSIS_LIMITCALC_SIMFIT
#define TSTARANALYSIS_LIMITCALC_SIMFIT

#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "RooAddPdf.h"
#include "RooFitResult.h"
#include <string>

//------------------------------------------------------------------------------
//   Object naming conventions
//------------------------------------------------------------------------------
extern std::string
SimFitBGPdfName( const std::string& datasetname, const std::string& signalname );

extern std::string
SimFitCombinePdfName ( const std::string& datasetname, const std::string& signalname );

extern std::string
StitchSimFitBgPdfname( const std::string& signalname );


//------------------------------------------------------------------------------
//   Fitting functions
//------------------------------------------------------------------------------
extern RooFitResult* SimFitSingle (
   SampleRooFitMgr*    data,
   SampleRooFitMgr*    sig,
   const std::string&  dataset,
   RooFitResult*    bgconstrain
);

extern RooAddPdf* MakeFullSimFit (
   SampleRooFitMgr*   data,
   SampleRooFitMgr*   sig,
   RooFitResult*      bgconstrain
);

//------------------------------------------------------------------------------
//   Main control flow to be called by main function
//------------------------------------------------------------------------------
extern void MakeSimFit(
   SampleRooFitMgr* data,
   SampleRooFitMgr* mc,
   std::vector<SampleRooFitMgr*>& sig_list
);

//------------------------------------------------------------------------------
//   Control flow subroutines
//------------------------------------------------------------------------------
extern void MakeSimFitPlot(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   const std::string& datasetname = ""
);

extern void MakeSimFitCardFile(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig
);

//------------------------------------------------------------------------------
//   Secondary helper functions
//------------------------------------------------------------------------------
extern Parameter GetBgNormError(
   SampleRooFitMgr* data,
   const std::string& datasetname,
   const std::string& signalname
);


#endif /* end of include guard: TSTARANALYSIS_LIMITCALC_SIMFIT */

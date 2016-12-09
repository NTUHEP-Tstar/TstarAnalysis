/*******************************************************************************
*
*  Filename    : MakeKeysPdf.hpp
*  Description : Forward declaration for all functions related to keys pdfs
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*     For implementation see src/RooFit_MakeKeysPdf.cc
*
*******************************************************************************/
#ifndef TSTARANALYSIS_LIMITCALC_MAKEKEYSPDF_HPP
#define TSTARANALYSIS_LIMITCALC_MAKEKEYSPDF_HPP

#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include "RooAddPdf.h"
#include "RooKeysPdf.h"
#include <string>

/*******************************************************************************
*   Object naming conventions
*   ** KeyPdfName - Defining naming convention between dataset and resulting keys pdf
*
*   ** StitchKeyPdfName - name of stitched Keys pdf
*******************************************************************************/
extern std::string KeyPdfName( const std::string& dataset );
extern std::string KeyNormName( const std::string& dataset );
extern std::string JoinPdfName( const std::string& err );
extern std::string JoinNormName( const std::string& err );
extern const std::string StitchKeyPdfName;
extern const std::string StitchKeyNormName;


/*******************************************************************************
*   Fitting functions
*******************************************************************************/
// Making a single keys from a designated dataset in a manager
extern RooKeysPdf* MakeSingleKeysPdf(
   SampleRooFitMgr*   sample,
   const std::string& datasetname
   );

// Stitching all RooKeysPdf together
extern RooAbsPdf* MakeFullKeysPdf( SampleRooFitMgr* sample );


#endif/* end of include guard: TSTARANALYSIS_LIMITCALC_MAKEKEYSPDF_HPP */

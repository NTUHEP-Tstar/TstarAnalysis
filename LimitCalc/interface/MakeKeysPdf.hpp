/*******************************************************************************
 *
 *  Filename    : MakeKeysPdf.hpp
 *  Description : Forward declaration for all functions related to keys pdfs
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *     For main implementation see src/RooFit_MakeKeysPdf.cc
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "RooKeysPdf.h"
#include "RooAddPdf.h"
#include <string>

//------------------------------------------------------------------------------
//   Object Naming conventions
//------------------------------------------------------------------------------

// Defining naming convention between dataset and resulting keys pdf
extern std::string KeyPdfName( const std::string& dataset );
// Defining the name of Master pdf
extern const std::string StitchKeyPdfName;

//------------------------------------------------------------------------------
//   Fitting control flow
//------------------------------------------------------------------------------

// Making a single keys from a designated dataset in a manager
extern RooKeysPdf* MakeSingleKeysPdf(
   SampleRooFitMgr*    sample,
   const std::string& datasetname
);


// Stitching all RooKeysPdf together
extern RooAddPdf*  MakeFullKeysPdf(
   SampleRooFitMgr* sample
);

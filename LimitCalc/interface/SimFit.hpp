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

/*******************************************************************************
*   Object naming conventions
*******************************************************************************/
extern std::string
SimFitBGPdfName( const std::string& datasetname, const std::string& signalname );

extern std::string
SimFitCombinePdfName ( const std::string& datasetname, const std::string& signalname );

extern std::string
StitchSimFitBgPdfname( const std::string& signalname );


/*******************************************************************************
*
*   Fitting functions :
*   ** SimFitSingle - attempt to fit the data->DataSet(setname) RooDataSet
*                     object to the background + signal model. The
*                     signal->DataSet("") constructed PDF object will always
*                     be used for the fitting
*
*   ** MakeFullSimFit - Run the SimFitSingle function on all available DataSet
*                       objects stored in the data object.
*
*******************************************************************************/
extern RooFitResult* SimFitSingle (
   SampleRooFitMgr*    data,
   SampleRooFitMgr*    sig,
   const std::string&  setname,
   RooFitResult*    bgconstrain
);

extern RooAddPdf* MakeFullSimFit (
   SampleRooFitMgr*   data,
   SampleRooFitMgr*   sig,
   RooFitResult*      bgconstrain
);

/*******************************************************************************
*   Main Control flow to be called be binary main functions
*******************************************************************************/
extern void MakeSimFit(
   SampleRooFitMgr* data,
   SampleRooFitMgr* mc,
   std::vector<SampleRooFitMgr*>& sig_list
);

/*******************************************************************************
*
*    Helper subroutine control flows
*    ** MakeSimFitPlot - generates the plots of the fitting results for the
*                        a given dataset tag
*
*    ** MakeSimFitCardFile - created the card file to be passed to higgs combine
*
*    ** GetBgNormError - Calculate the normalization error from bg fitting results
*                        Of different datasets.
*
*    ** GetSigNormError - Calculating the normalization error for signal dataset entries
*
*******************************************************************************/
extern void MakeSimFitPlot(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   const std::string& datasetname = ""
);

extern void MakeSimFitCardFile(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig
);

extern Parameter GetBgNormError(
   SampleRooFitMgr* data,
   const std::string& uperrorpdf,
   const std::string& downerrorpdf,
   const std::string& signalname
);

extern Parameter GetSigNormError(
   SampleRooFitMgr* sig,
   const std::string& uperrorset,
   const std::string& downerrorset
);


#endif /* end of include guard: TSTARANALYSIS_LIMITCALC_SIMFIT */

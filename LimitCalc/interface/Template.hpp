/*******************************************************************************
 *
 *  Filename    : Template.hpp
 *  Description : Functions for Aiding with Template method
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *     Main implementations in  src/RooFit_MCTemplate.cc
*******************************************************************************/
#ifndef TSTARANALYSIS_LIMITCALC_TEMPLATE_HPP
#define TSTARANALYSIS_LIMITCALC_TEMPLATE_HPP

#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include <vector>
#include <string>

//------------------------------------------------------------------------------
//   Object naming convention
//------------------------------------------------------------------------------
extern std::string TemplatePdfName( const std::string& datasetname );
extern const std::string StitchTemplatePdfName;

//------------------------------------------------------------------------------
//   Fitting Results
//------------------------------------------------------------------------------

extern RooFitResult* FitBackgroundTemplate(
   SampleRooFitMgr* bkg,
   const std::string& dataset
);

// Fitting very dataset and making joint result
extern RooAddPdf* MakeFullTemplate(
   SampleRooFitMgr* bkg
);

//------------------------------------------------------------------------------
//   Main control flow to be called by main function
//------------------------------------------------------------------------------
extern void MakeTemplate(
   SampleRooFitMgr*    data,
   SampleRooFitMgr*    mc,
   std::vector<SampleRooFitMgr*>& signallist
);

extern void  MakeTemplatePlot(
   SampleRooFitMgr*   data    ,
   SampleRooFitMgr*   mc      ,
   SampleRooFitMgr*   signal  ,
   const bool         use_data
);

extern void  MakeTemplateCardFile(
   SampleRooFitMgr*   data,
   SampleRooFitMgr*   mc  ,
   SampleRooFitMgr*   signal
);


#endif /* end of include guard: TSTARANALYSIS_LIMITCALC_TEMPLATE_HPP */

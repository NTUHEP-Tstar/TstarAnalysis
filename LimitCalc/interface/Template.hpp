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
#include <string>
#include <vector>

/*******************************************************************************
*   Object naming conventions
*******************************************************************************/
extern std::string TemplatePdfName( const std::string& datasetname );
extern const std::string StitchTemplatePdfName;


/*******************************************************************************
*
*   Fitting results functions
*   ** FitBackgroundTemplate - fitting a single dataset to the designated
*                              background functions, returns fitting results
*
*   ** MakeFullTemplate - fit all dataset available in the object and stictch
*                         the results together
*
*******************************************************************************/
extern RooFitResult* FitBackgroundTemplate(
  SampleRooFitMgr*   bkg,
  const std::string& dataset
  );

extern RooAbsPdf* MakeFullTemplate( SampleRooFitMgr* bkg );

/*******************************************************************************
*   Main constrol flow to be called by main function
*******************************************************************************/
extern void MakeTemplate(
  SampleRooFitMgr*               data,
  SampleRooFitMgr*               mc,
  std::vector<SampleRooFitMgr*>& signallist
  );

/*******************************************************************************
*
*   Helper control flow functions
*   ** Make Template Plot - show plot presenting the fitting results
*                           is the use_data tag is set to be true, the MC fitting
*                           results will display along side the data dataset
*                           rather than the MC
*
*   ** MakeTemplateCardFile - generate card file to pass to the higgs combine package
*******************************************************************************/
extern void MakeTemplatePlot(
  SampleRooFitMgr* data,
  SampleRooFitMgr* mc,
  SampleRooFitMgr* signal,
  const bool       use_data
  );

extern void MakeTemplateCardFile(
  SampleRooFitMgr* data,
  SampleRooFitMgr* mc,
  SampleRooFitMgr* signal
  );

#endif/* end of include guard: TSTARANALYSIS_LIMITCALC_TEMPLATE_HPP */

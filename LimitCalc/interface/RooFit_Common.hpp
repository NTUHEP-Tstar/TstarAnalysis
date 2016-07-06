/*******************************************************************************
 *
 *  Filename    : MakePDF.hh
 *  Description : Declaring common pdfs in SampleRooFitMgr objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/

#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "RooKeysPdf.h"
#include "RooGenericPdf.h"
#include "RooFitResult.h"

#include <string>
#include <cstdlib>

//------------------------------------------------------------------------------
//   Static variables
//------------------------------------------------------------------------------
extern const std::string ws_name;

// see src/RooFit_Common.cc for implementations
extern RooKeysPdf* MakeKeysPdf(SampleRooFitMgr*);

extern RooGenericPdf* MakeFermi(SampleRooFitMgr*,const std::string& = "fermi");
extern RooGenericPdf* MakeExo(SampleRooFitMgr*,const std::string& = "exo" );

extern void SaveRooWorkSpace(
   RooDataSet*                        data,
   const std::vector<RooAbsPdf*>&     bkg_pdf_list,
   const std::vector<RooAbsPdf*>&     sig_pdf_list,
   const std::vector<RooFitResult*>&  fit_results_list
);

extern FILE* MakeCardCommon(
   RooDataSet* data,
   RooAbsPdf*  bkg,
   RooAbsPdf*  sig,
   const std::string& card_tag
);

extern void PrintNuisanceFloats(
   FILE*,
   const std::string& nuisance_name,
   const std::string& nuisance_type,
   const Parameter& sig_nuisance, // Leave (0,0,0) if skip
   const Parameter& bkg_nuisance
);

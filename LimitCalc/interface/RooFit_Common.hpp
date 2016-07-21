/*******************************************************************************
 *
 *  Filename    : MakePDF.hh
 *  Description : Declaring common pdfs in SampleRooFitMgr objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/

#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "TstarAnalysis/Common/interface/InitSampleStatic.hpp"
#include "TstarAnalysis/Common/interface/NameParse.hpp"
#include "RooKeysPdf.h"
#include "RooGenericPdf.h"
#include "RooLognormal.h"
#include "RooLandau.h"
#include "RooAddPdf.h"
#include "RooFitResult.h"
#include "RooMCStudy.h"

#include <string>
#include <vector>
#include <cstdlib>

//------------------------------------------------------------------------------
//   Static variables  see src/Common_Init.cc
//------------------------------------------------------------------------------
extern std::string       dataset_alias;
extern TstarNamer        limit_namer;


//------------------------------------------------------------------------------
//   Limit calculation main control flow and helper functions
//------------------------------------------------------------------------------

// in src/RooFit_SimFit.cc
extern void MakeSimFit(
   SampleRooFitMgr* data,
   std::vector<SampleRooFitMgr*>& sig_list
);
namespace smft
{
RooFitResult* FitPDFs (
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   const std::string& extratag = ""
);

void MakeValidationPlot(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig,
   RooFitResult*    fiterr,
   const std::string& extratag=""
);
void MakeCardFile(
   SampleRooFitMgr* data,
   SampleRooFitMgr* sig
);
};

// in src/RooFit_MCTemplate.cc
extern void MakeTemplate(
   SampleRooFitMgr*,
   SampleRooFitMgr*,
   std::vector<SampleRooFitMgr*>&
);
namespace tmplt
{
RooFitResult* MakeBGFromMC( SampleRooFitMgr* );
void  MakeTemplatePlot(
   SampleRooFitMgr*,
   SampleRooFitMgr*,
   SampleRooFitMgr*,
   RooFitResult*,
   const bool
);
void  MakeCardFile(
   SampleRooFitMgr*,
   SampleRooFitMgr*,
   SampleRooFitMgr*
);
};

// in src/RooFit_Bias.cc
extern void MakeBias(
   SampleRooFitMgr*,
   SampleRooFitMgr*,
   std::vector<SampleRooFitMgr*>&
);
namespace bias
{
void MakePsuedoData(SampleRooFitMgr*,const unsigned);
}

// in src/RooFit_SimFit_Val.cc
extern void RunValidate( SampleRooFitMgr*, std::vector<SampleRooFitMgr*>& );
namespace val
{
struct PullResult; // Forward declaring only
PullResult RunSingle( SampleRooFitMgr*, SampleRooFitMgr* );
Parameter  DrawPull( RooMCStudy*, RooRealVar*, const int, const std::string& );
void       PlotMassCompare( TGraph*, const std::string& );
}

//------------------------------------------------------------------------------
//   PDF Definitions, see Common_PDF.cc
//------------------------------------------------------------------------------
extern RooKeysPdf*    MakeKeysPdf(SampleRooFitMgr*);
extern RooGenericPdf* MakeFermi(SampleRooFitMgr*,const std::string& = "fermi");
extern RooGenericPdf* MakeExo(SampleRooFitMgr*,const std::string& = "exo" );
extern RooLognormal*  MakeLognorm( SampleRooFitMgr*, const std::string& = "longnorm");
extern RooLandau*     MakeLandau( SampleRooFitMgr*, const std::string& ="landau");
extern RooGenericPdf* MakeTrial( SampleRooFitMgr*, const std::string& = "trial" );

//------------------------------------------------------------------------------
//   Initalizing functions, see Common_Init.cc
//------------------------------------------------------------------------------
extern void InitRooFitSettings( const TstarNamer& );
extern void InitSingle( SampleRooFitMgr*&, const std::string& );
extern void InitDataAndSignal( SampleRooFitMgr*&, std::vector<SampleRooFitMgr*>& );
extern void InitMC( SampleRooFitMgr*& );

//------------------------------------------------------------------------------
//   Saving Functions, see Common_Save.cc
//------------------------------------------------------------------------------
extern void SetDataAlias( const std::string& );

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

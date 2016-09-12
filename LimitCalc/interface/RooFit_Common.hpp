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
//   PDF stitching functions
//------------------------------------------------------------------------------
extern RooAddPdf* MakeStichPdf(
   SampleRooFitMgr* sample ,
   const std::string& stitchname,
   const std::vector<std::string>& pdfnamelist
);


//------------------------------------------------------------------------------
//   Limit calculation main control flow and helper functions
//------------------------------------------------------------------------------

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
extern void RunValGenFit( SampleRooFitMgr*, SampleRooFitMgr*, SampleRooFitMgr* );
extern void PlotGenFit( const std::vector<std::string>& );
namespace val
{
std::string SigStrengthTag();

struct PullResult; // List of all the parameters relavent
PullResult PlotSingleGenFit( const std::string& masstag );
Parameter  MakePullPlot( RooDataSet&, const std::string& masstag, const std::string& tag );
Parameter  MakeNormPlot( RooDataSet&, const std::string& masstag, const std::string& tag );
void       MakePullComparePlot( TGraph*, const std::string& tag );
}

//------------------------------------------------------------------------------
//   Initalizing functions, see Common_Init.cc
//------------------------------------------------------------------------------
extern void InitRooFitSettings( const TstarNamer& );
extern void InitSingle( SampleRooFitMgr*&, const std::string& );
extern void InitDataAndSignal( SampleRooFitMgr*&, std::vector<SampleRooFitMgr*>& );
extern void InitMC( SampleRooFitMgr*& );


//------------------------------------------------------------------------------
//   Saving functions and card file helpers
//------------------------------------------------------------------------------
extern void SaveRooWorkSpace(
   RooDataSet*                        data,
   const std::vector<RooAbsPdf*>&     bkg_pdf_list,
   const std::vector<RooAbsPdf*>&     sig_pdf_list
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

extern void PrintFloatParam(
   FILE*,
   const RooRealVar*
);

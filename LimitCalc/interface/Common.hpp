/*******************************************************************************
*
*  Filename    : MakePDF.hh
*  Description : Declaring common pdfs in SampleRooFitMgr objects
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/Common/interface/NameParse.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include "RooAddPdf.h"
#include <cstdlib>
#include <string>
#include <vector>

/*******************************************************************************
*   Common variables, defined in src/Common_Init.cc
*******************************************************************************/
extern TstarNamer limit_namer;

/*******************************************************************************
*   Object initializing functions ( called after parsing user input )
*   - see src/Common/init
*
*   ** InitRooFitSettings - read the settings in the master json file initialized
*                           the static variables.
*
*   ** InitSingle - Initialize single pointer by input json file.
*
*   ** InitDataAndSignal - Initialize data and signal list. The signal list is
*                          defined, by the "Signal List" entry in the master json file
*
*   ** InitMC - Initialize MC manager. The channels described is found in Background
*               in the master json file.
*
*******************************************************************************/
extern void InitRooFitSettings( const TstarNamer& );

extern void InitSingle( SampleRooFitMgr*&, const std::string& );

extern void InitDataAndSignal( SampleRooFitMgr*&, std::vector<SampleRooFitMgr*>& );

extern void InitMC( SampleRooFitMgr*& );



/*******************************************************************************
*   PDF stitching functions
*   Gets all of the corresponding PDFs in list and stitches them into a
*   RooAddPdf object with the alias name of <stitchname>, note the for n legal
*   pdfs names given, it will generate n-1 coefficients.
*   - see src/Common_PdfStitch.cc
*******************************************************************************/
extern RooAddPdf* MakeStichPdf(
   SampleRooFitMgr*                sample,
   const std::string&              stitchname,
   const std::vector<std::string>& pdfnamelist
   );



/*******************************************************************************
*
*   Object saving functions - src/Common_Save.cc
*
*   ** SaveRooWorkSpace - saves datasets in data, and list of pdfs to a
*                         RooWorkspace file
*
*   ** MakeCardCommon - Opens a card files with all the common header parts
*                       printed:
*                       - number of channels
*                       - RooObject names
*                       - raw expected yields ( without parameters )
*
*   ** PrintNuisanceFloats - Prints the single line representing a normalization
*                            nuissance parameters, the first parameter passed
*                            will be for the signal part, the second background.
*                            If you pass a null parameter (0,0,0), the ignore string
*                            "--" will be printed .
*
*   ** PrintFloatParam - Print a RooRealVar as a Gaussian nuissance parameter, error is based
*                        on the average error of the variables.
*
*   ** PrintFlatParam - Print a RooRealVar as flat nuissance parameters. Range based on
*                       variable min,max values
*
*******************************************************************************/
extern void SaveRooWorkSpace(
   RooDataSet*                    data,
   const std::vector<RooAbsPdf*>& bkg_pdf_list,
   const std::vector<RooAbsPdf*>& sig_pdf_list
   );

extern FILE* MakeCardCommon(
   RooDataSet*        data,
   RooAbsPdf*         bkg,
   RooAbsPdf*         sig,
   const std::string& card_tag
   );

extern void PrintNuisanceFloats(
   FILE*,
   const std::string& nuisance_name,
   const std::string& nuisance_type,
   const Parameter&   sig_nuisance,// Leave (0,0,0) if skip
   const Parameter&   bkg_nuisance
   );

extern void PrintFloatParam(
   FILE*,
   const RooRealVar*
   );

extern void PrintFlatParam(
   FILE*,
   const RooRealVar*
   );

// ------------------------------------------------------------------------------
//   Limit calculation main control flow and helper functions
// ------------------------------------------------------------------------------

// in src/RooFit_Bias.cc
extern void MakeBias(
   SampleRooFitMgr*,
   SampleRooFitMgr*,
   std::vector<SampleRooFitMgr*>&
   );
namespace bias
{
void MakePsuedoData( SampleRooFitMgr*, const unsigned );
}

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

#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooAbsReal.h"
#include "RooAddPdf.h"

#include <boost/program_options.hpp>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

/*******************************************************************************
*   Common variables, defined in src/Common_Init.cc
*******************************************************************************/
extern TstarNamer limnamer;

extern const std::vector<std::string> uncsource;

extern boost::program_options::options_description LimitOptions();
extern boost::program_options::options_description MassOptions();
extern boost::program_options::options_description PsuedoExpOptions();
extern boost::program_options::options_description ExtraCutOptions();
extern boost::program_options::options_description HiggsCombineOptions();
extern boost::program_options::options_description ExtraLimitOptions();

extern mgr::OptNamer::PARSE_RESULTS CheckPsuedoExpOptions();
extern mgr::OptNamer::PARSE_RESULTS CheckDisableOptions();

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
*   PDF stitching functions:
*
*   Given a central PDF and a single PDF pairs, it will attempt to create an
*   Linear interpolation of in the form of :
*
*   f_tot = f_cen * ( 1 - abs(x) ) +[ x * (x>0) * f_1 *  ]   + [x*(x<0)*f_2]
*
*   where x is a floating value in the range [-1,1].
*
*   The input will be a simple all public struct with the name of the resulting
*   joined PDF, the name of the central pdf and the names of the two additional
*   pdfs.
*
*   In the case that multiple pdf pairs are wished to join to a central pdf with
*   a similar fashion. The input format should be:
*     * the name of the final pdf output
*     * the name of the central pdf
*     * a list of pdf names in pairs
*
*******************************************************************************/
struct JoinRequest
{
  std::string joinname;
  std::string centralpdfname;
  std::string uppdfname;
  std::string downpdfname;

  static std::string JoinCoeffName( const std::string& joinname );
  std::string        JoinCoeffName() const;
};

extern RooAbsPdf* MakeJoinPdf(
  SampleRooFitMgr*   sample,
  const JoinRequest& x
  );


extern RooAbsPdf* MakeMultiJoinPdf(
  SampleRooFitMgr* sample,
  const std::string& stitchname,
  const std::string& central,
  const std::vector<std::pair<std::string, std::string> >& joinlist
  );

/*******************************************************************************
*   Make simple stitching
*   Simple Stitching that works without any pairing. Linear interpolation between
*   all given PDF functions
*******************************************************************************/
extern RooAbsPdf* MakeSimpleStitchPdf(
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
  RooAbsData*                     data,
  const std::vector<RooAbsPdf*>&  pdflist,
  const std::vector<RooAbsReal*>& funclist
  );

extern void MakeCardCommon(
  std::ofstream& outfile,
  RooAbsData*    data,
  RooAbsPdf*     bkg,
  RooAbsPdf*     sig
  );

extern void PrintNuisanceFloats(
  std::ofstream&        outfile,
  const std::string&    nuisance_name,
  const std::string&    nuisance_type,
  const mgr::Parameter& sig_nuisance,// Leave (0,0,0) if skip
  const mgr::Parameter& bkg_nuisance
  );

extern void PrintFloatParam(
  std::ofstream&    outfile,
  const RooRealVar* var
  );

extern void PrintFlatParam(
  std::ofstream&    outfile,
  const RooRealVar* var
  );

// Legacy functions
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

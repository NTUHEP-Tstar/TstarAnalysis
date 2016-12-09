/*******************************************************************************
*
*  Filename    : Common_Save.cc
*  Description : Common functions required by fitting methods
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/ParameterFormat.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooFitResult.h"
#include "RooWorkspace.h"
#include "TFile.h"

#include <string>
#include <vector>

using namespace std;

/*******************************************************************************
*   Save results
*******************************************************************************/
static const string ws_name = "wspace";

void
SaveRooWorkSpace(
   RooAbsData*               data,
   const vector<RooAbsPdf*>& pdflist,
   const vector<RooAbsReal*>& funclist
   )
{
   const string roofit_file = limnamer.RootFileName( "roofitobj" );
   RooWorkspace ws( ws_name.c_str(), ws_name.c_str() );

   cout << "Saving RooFit objects to " << roofit_file << endl;
   ws.import( *data );

   for( auto& pdf  : pdflist ){
      ws.import( *pdf  );
   }

   for( auto& func : funclist ){
      ws.import( *func );
   }

   ws.writeToFile( roofit_file.c_str() );

}

/******************************************************************************/

FILE*
MakeCardCommon(
   RooAbsData*        data,
   RooAbsPdf*         bg_pdf,
   RooAbsPdf*         sig_pdf,
   const std::string& tag
   )
{
   FILE* card_file = fopen( limnamer.TextFileName( "card", {tag} ).c_str(), "w" );

   fprintf( card_file, "imax 1\n" );
   fprintf( card_file, "jmax *\n" );
   fprintf( card_file, "kmax *\n" );
   fprintf( card_file, "----------------------------------------\n" );

   // Printing Objects
   fprintf(
      card_file, "shapes %10s %15s %30s %s:%s\n",
      "data_obs",
      limnamer.GetChannel().c_str(),
      limnamer.RootFileName( "roofitobj" ).c_str(),
      ws_name.c_str(),
      data->GetName()
      );
   fprintf(
      card_file, "shapes %10s %15s %30s %s:%s\n",
      "bg",
      limnamer.GetChannel().c_str(),
      limnamer.RootFileName( "roofitobj" ).c_str(),
      ws_name.c_str(),
      bg_pdf->GetName()
      );

   fprintf(
      card_file, "shapes %10s %15s %30s %s:%s\n",
      "sig",
      limnamer.GetChannel().c_str(),
      limnamer.RootFileName( "roofitobj" ).c_str(),
      ws_name.c_str(),
      sig_pdf->GetName()
      );
   fprintf( card_file, "----------------------------------------\n" );

   // Printing data correspondence
   fprintf( card_file, "%12s %s\n",  "bin",         limnamer.GetChannel().c_str() );
   fprintf( card_file, "%12s %lg\n", "observation", data->sumEntries() );
   fprintf( card_file, "----------------------------------------\n" );

   fprintf(
      card_file, "%12s %15s %15s\n", "bin",
      limnamer.GetChannel().c_str(),
      limnamer.GetChannel().c_str()
      );
   fprintf( card_file, "%12s %15s %15s\n", "process", "sig", "bg" );
   fprintf( card_file, "%12s %15s %15s\n", "process", "-1",  "1" );

   return card_file;
}

/******************************************************************************/

void
PrintNuisanceFloats(
   FILE*            card_file,
   const string&    nuisance_name,
   const string&    nuisance_type,
   const Parameter& sig_nuisance,
   const Parameter& bkg_nuisance
   )
{

   fprintf(
      card_file, "%-20s %3s %15s %15s\n",
      nuisance_name.c_str(),
      nuisance_type.c_str(),
      HiggsDataCard( sig_nuisance ).c_str(),
      HiggsDataCard( bkg_nuisance ).c_str()
      );
}

/******************************************************************************/

void
PrintFloatParam(
   FILE*             card_file,
   const RooRealVar* var
   )
{
   fprintf(
      card_file, "%-45s %-10s %lf %lf\n",
      var->GetName(),
      "param",
      var->getVal(),
      var->getError()
      );
}

/******************************************************************************/

void
PrintFlatParam(
   FILE*             cardfile,
   const RooRealVar* var
   )
{
   fprintf(
      cardfile, "%-45s %s %lf %lf\n",
      var->GetName(),
      "param",
      var->getVal(),
      var->getMax()
      );
}

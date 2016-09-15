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

// ------------------------------------------------------------------------------
//   Save results
// ------------------------------------------------------------------------------
static const string ws_name = "wspace";

void
SaveRooWorkSpace(
   RooDataSet*               data,
   const vector<RooAbsPdf*>& bg_list,
   const vector<RooAbsPdf*>& sig_list
   )
{
   const string roofit_file = limit_namer.RootFileName( "roofitobj" );
   RooWorkspace ws( ws_name.c_str(), ws_name.c_str() );

   cout << "Saving RooFit objects to " << roofit_file << endl;
   ws.import( *data );

   for( auto& bg  : bg_list ){
      ws.import( *bg  );
   }

   for( auto& sig : sig_list ){
      ws.import( *sig );
   }

   ws.writeToFile( roofit_file.c_str() );

}


FILE*
MakeCardCommon(
   RooDataSet*        data,
   RooAbsPdf*         bg_pdf,
   RooAbsPdf*         sig_pdf,
   const std::string& tag
   )
{
   FILE* card_file = fopen( limit_namer.TextFileName( "card", {tag} ).c_str(), "w" );

   fprintf( card_file, "imax 1\n" );
   fprintf( card_file, "jmax *\n" );
   fprintf( card_file, "kmax *\n" );
   fprintf( card_file, "----------------------------------------\n" );

   // Printing Objects
   fprintf(
      card_file, "shapes %10s %15s %30s %s:%s\n",
      "data_obs",
      limit_namer.GetChannel().c_str(),
      limit_namer.RootFileName( "roofitobj" ).c_str(),
      ws_name.c_str(),
      data->GetName()
      );
   fprintf(
      card_file, "shapes %10s %15s %30s %s:%s\n",
      "bg",
      limit_namer.GetChannel().c_str(),
      limit_namer.RootFileName( "roofitobj" ).c_str(),
      ws_name.c_str(),
      bg_pdf->GetName()
      );

   fprintf(
      card_file, "shapes %10s %15s %30s %s:%s\n",
      "sig",
      limit_namer.GetChannel().c_str(),
      limit_namer.RootFileName( "roofitobj" ).c_str(),
      ws_name.c_str(),
      sig_pdf->GetName()
      );
   fprintf( card_file, "----------------------------------------\n" );

   // Printing data correspondence
   fprintf( card_file, "%12s %s\n",  "bin",         limit_namer.GetChannel().c_str() );
   fprintf( card_file, "%12s %lg\n", "observation", data->sumEntries() );
   fprintf( card_file, "----------------------------------------\n" );

   fprintf(
      card_file, "%12s %15s %15s\n", "bin",
      limit_namer.GetChannel().c_str(),
      limit_namer.GetChannel().c_str()
      );
   fprintf( card_file, "%12s %15s %15s\n", "process", "sig", "bg" );
   fprintf( card_file, "%12s %15s %15s\n", "process", "-1",  "1" );

   return card_file;
}

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
      card_file, "%8s %3s %15s %15s\n",
      nuisance_name.c_str(),
      nuisance_type.c_str(),
      HiggsDataCard( sig_nuisance ).c_str(),
      HiggsDataCard( bkg_nuisance ).c_str()
      );
}

void
PrintFloatParam(
   FILE*             card_file,
   const RooRealVar* var
   )
{
   fprintf(
      card_file, "%s %s %lf %lf\n",
      var->GetName(),
      "param",
      var->getVal(),
      var->getError()
      );
}


void
PrintFlatParam(
   FILE*             cardfile,
   const RooRealVar* var
   )
{
   fprintf(
      cardfile, "%s %s\n",
      var->GetName(),
      "flatParam"
      );
}

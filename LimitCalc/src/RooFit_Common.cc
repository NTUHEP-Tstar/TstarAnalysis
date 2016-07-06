/*******************************************************************************
 *
 *  Filename    : RooFit_Common.cc
 *  Description : Common functions required by fitting methods
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"

#include "TFile.h"
#include "RooWorkspace.h"
#include "RooFitResult.h"
#include "RooKeysPdf.h"
#include "RooGenericPdf.h"

#include <vector>
#include <string>

using namespace std;

//------------------------------------------------------------------------------
//   External variables
//------------------------------------------------------------------------------
const string ws_name = "wspace";

//------------------------------------------------------------------------------
//   PDF Definitions
//------------------------------------------------------------------------------
RooKeysPdf* MakeKeysPdf(SampleRooFitMgr* sample)
{
   const string pdf_name = sample->MakePdfAlias( "key" );
   RooKeysPdf*  pdf      = new RooKeysPdf(
      pdf_name.c_str(), pdf_name.c_str(),
      SampleRooFitMgr::x(),
      *(sample->OriginalDataSet()),
      RooKeysPdf::NoMirror,
      2.
   );
   sample->AddPdf( pdf );
   return pdf;
}

RooGenericPdf* MakeFermi(SampleRooFitMgr* sample,const string& name = "fermi" )
{
   char formula[1024];
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar* a  = var_mgr.NewVar( "a" + pdf_name , 100, -1000,1000);
   RooRealVar* b  = var_mgr.NewVar( "b" + pdf_name , 100, -1000,1000);

   sprintf( formula, "1/(1+exp((x-%s)/%s))", a->GetName() , b->GetName() );

   RooGenericPdf* pdf = new RooGenericPdf(
      pdf_name.c_str() , pdf_name.c_str(),
      formula,
      RooArgSet(SampleRooFitMgr::x(),*a,*b)
   );

   sample->AddPdf( pdf );
   return pdf;
}

RooGenericPdf* MakeExo( SampleRooFitMgr* sample, const string& name="exo" )
{
   char formula[1024];
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar* a  = var_mgr.NewVar( "a" + pdf_name , 1, -1000, +1000);
   RooRealVar* b  = var_mgr.NewVar( "b" + pdf_name , 1, -1000, +1000);
   RooRealVar* c  = var_mgr.NewVar( "c" + pdf_name , 1, -1000, +1000);
   sprintf( formula, "(TMath::Power((1-(x/13000.)),(%s)))/(TMath::Power((x/13000.),(%s+%s*TMath::Log(x/13000.))))",
      a->GetName(),
      b->GetName(),
      c->GetName()
   );

   RooGenericPdf* pdf = new RooGenericPdf(
      pdf_name.c_str() , pdf_name.c_str(),
      formula,
      RooArgSet(SampleRooFitMgr::x(),*a,*b,*c)
   );

   sample->AddPdf( pdf );
   return pdf;
}

//------------------------------------------------------------------------------
//   Save results
//------------------------------------------------------------------------------
void SaveRooWorkSpace(
   RooDataSet*            data,
   const vector<RooAbsPdf*>&    bg_list ,
   const vector<RooAbsPdf*>&    sig_list,
   const vector<RooFitResult*>& result_list
)
{
   const string roofit_file = limit_namer.RootFileName("roofitobj");
   const string result_file = limit_namer.RootFileName("roofitresults");
   RooWorkspace ws( ws_name.c_str() , ws_name.c_str() );

   cout << "Saving RooFit objects to " << roofit_file << endl;
   ws.import( *data ) ;
   for( auto& bg  : bg_list ) { ws.import( *bg  ); }
   for( auto& sig : sig_list ){ ws.import( *sig ); }
   ws.writeToFile( roofit_file.c_str() );

   cout << "Saving fit results to " << result_file.c_str() << endl;
   TFile* fit_file = TFile::Open( result_file.c_str() , "RECREATE" );
   for( auto& result: result_list ){
      result->Write( result->GetName() );
   }
   fit_file->Close();
   delete fit_file;
}


FILE* MakeCardCommon(
   RooDataSet*  data,
   RooAbsPdf*  bg_pdf,
   RooAbsPdf*  sig_pdf,
   const std::string& tag
)
{
   FILE* card_file = fopen( limit_namer.TextFileName( "card" , tag ).c_str()  , "w" );

   fprintf( card_file , "imax 1\n" );
   fprintf( card_file , "jmax *\n" );
   fprintf( card_file , "kmax *\n" );
   fprintf( card_file , "----------------------------------------\n" );

   // Printing Objects
   fprintf(
      card_file , "shapes %10s %15s %30s %s:%s\n" ,
      "data_obs",
      limit_namer.GetChannel().c_str() ,
      limit_namer.RootFileName("roofitobj").c_str() ,
      ws_name.c_str(),
      data->GetName()
   );
   fprintf(
      card_file , "shapes %10s %15s %30s %s:%s\n" ,
      "bg",
      limit_namer.GetChannel().c_str() ,
      limit_namer.RootFileName("roofitobj").c_str() ,
      ws_name.c_str(),
      bg_pdf->GetName()
   );

   fprintf(
      card_file , "shapes %10s %15s %30s %s:%s\n" ,
      "sig",
      limit_namer.GetChannel().c_str() ,
      limit_namer.RootFileName("roofitobj").c_str() ,
      ws_name.c_str(),
      sig_pdf->GetName()
   );
   fprintf( card_file , "----------------------------------------\n" );

   // Printing data correspondence
   fprintf( card_file , "%12s %s\n" , "bin",         limit_namer.GetChannel().c_str() );
   fprintf( card_file , "%12s %lg\n", "observation", data->sumEntries() );
   fprintf( card_file , "----------------------------------------\n" );

   fprintf(
      card_file , "%12s %15s %15s\n" , "bin",
      limit_namer.GetChannel().c_str(),
      limit_namer.GetChannel().c_str()
   );
   fprintf( card_file , "%12s %15s %15s\n" , "process" , "sig", "bg" );
   fprintf( card_file , "%12s %15s %15s\n" , "process" , "-1" , "1" );

   return card_file;
}

void PrintNuisanceFloats(
   FILE* card_file,
   const string& nuisance_name,
   const string& nuisance_type,
   const Parameter& sig_nuisance,
   const Parameter& bkg_nuisance
){

   fprintf(
      card_file , "%8s %3s %15s %15s\n",
      nuisance_name.c_str(),
      nuisance_type.c_str(),
      sig_nuisance.DataCardFormat().c_str(),
      bkg_nuisance.DataCardFormat().c_str()
   );
}

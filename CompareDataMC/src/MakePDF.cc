/*******************************************************************************
 *
 *  Filename    : MakePDF.cc
 *  Description : Declaring pdfs objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleRooFitMgr.hh"
#include "TstarAnalysis/CompareDataMC/interface/VarMgr.hh"

#include "RooKeysPdf.h"
#include "RooGenericPdf.h"
#include <string>

using namespace std;

//------------------------------------------------------------------------------
//   Making pdfs
//------------------------------------------------------------------------------
RooKeysPdf* MakeKeysPdf(SampleRooFitMgr* sample,const string& name = "")
{
   const string pdf_name = sample->MakePdfAlias( name );
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

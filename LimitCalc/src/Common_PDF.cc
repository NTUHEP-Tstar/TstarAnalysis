/*******************************************************************************
 *
 *  Filename    : Common_PDF.cc
 *  Description : Definition of PDF generations
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"

#include "RooKeysPdf.h"
#include "RooGenericPdf.h"
#include "RooLognormal.h"
#include "RooLandau.h"

#include <string>

using namespace std;

//------------------------------------------------------------------------------
//   Keys PDF
//------------------------------------------------------------------------------
RooKeysPdf* MakeKeysPdf(SampleRooFitMgr* sample)
{
   if( sample->GetPdfFromAlias("key") ){ // Do not reproduce if already has it
      return (RooKeysPdf*)sample->GetPdfFromAlias("key");
   }

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

//------------------------------------------------------------------------------
//   Fermi function f(m) = 1/(1+exp((m-a)/b))
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
//   EXO function f(m) = (1-x)^a / x^b ; x = m/sqrt(s)
//------------------------------------------------------------------------------
RooGenericPdf* MakeExo( SampleRooFitMgr* sample, const string& name="exo" )
{
   char formula[1024];
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar* a  = var_mgr.NewVar( "a" + pdf_name , 1, -100, +100);
   RooRealVar* b  = var_mgr.NewVar( "b" + pdf_name , 1, -100, +100);
   sprintf( formula, "(TMath::Power((1-(x/13000.)),(%s)))/(TMath::Power((x/13000.),(%s)))",
      a->GetName(),
      b->GetName()
   );

   RooGenericPdf* pdf = new RooGenericPdf(
      pdf_name.c_str() , pdf_name.c_str(),
      formula,
      RooArgSet(SampleRooFitMgr::x(),*a,*b)
   );

   sample->AddPdf( pdf );
   return pdf;
}

//------------------------------------------------------------------------------
//   Lognormal distribution effective f(m) = 1/x * exp( -b ln^2(x/a))
//------------------------------------------------------------------------------
RooLognormal* MakeLognorm( SampleRooFitMgr* sample, const string& name ="lognorm" )
{
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar*  a = var_mgr.NewVar( "a"+pdf_name, 200, 0.0001,10000 );
   RooRealVar*  b = var_mgr.NewVar( "b"+pdf_name, 200, 1.0001,10000 ); // could not be negative
   RooLognormal* pdf = new RooLognormal(
      pdf_name.c_str(), pdf_name.c_str(),
      SampleRooFitMgr::x(),  *a, *b
   );
   sample->AddPdf( pdf );
   return pdf ;
}

//------------------------------------------------------------------------------
//   Landau distribution
//------------------------------------------------------------------------------
RooLandau* MakeLandau( SampleRooFitMgr* sample, const string& name ="landau" )
{
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar*  a = var_mgr.NewVar( "a"+pdf_name, 200, -10000,10000 );
   RooRealVar*  b = var_mgr.NewVar( "b"+pdf_name, 200, 0, 10000 ); // could not be negative
   RooLandau* pdf = new RooLandau(
      pdf_name.c_str(), pdf_name.c_str(),
      SampleRooFitMgr::x(),  *a, *b
   );
   sample->AddPdf( pdf );
   return pdf ;
}

//------------------------------------------------------------------------------
//   Edit this function for testing purposes
//------------------------------------------------------------------------------
RooGenericPdf* MakeTrial( SampleRooFitMgr* sample,  const string& name="trial")
{
   char formula[1024];
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar* a  = var_mgr.NewVar( "a" + pdf_name , 100, -200  , +200    );
   RooRealVar* b  = var_mgr.NewVar( "b" + pdf_name , 160 ,  0    , +200    );
   // RooRealVar* c  = var_mgr.NewVar( "c" + pdf_name , 100,  0.01 , +10000 );
   sprintf( formula,
      "exp( 0.5 * ( -(x-(%s))/(%s) + exp( -(x-(%s)/(%s)))  ) )",
      a->GetName(),
      b->GetName(),
      a->GetName(),
      b->GetName()
   );

   RooGenericPdf* pdf = new RooGenericPdf(
      pdf_name.c_str() , pdf_name.c_str(),
      formula,
      RooArgSet(SampleRooFitMgr::x(),*a,*b)
   );

   sample->AddPdf( pdf );
   return pdf;
}

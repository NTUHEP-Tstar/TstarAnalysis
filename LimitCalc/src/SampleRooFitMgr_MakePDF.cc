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
//   Function for defining pdf functions
//------------------------------------------------------------------------------
static vector<RooRealVar*> MakeKeysPdf(SampleRooFitMgr*, const std::string& tag);
static vector<RooRealVar*> MakeFermi  (SampleRooFitMgr*, const std::string& tag);
static vector<RooRealVar*> MakeExo    (SampleRooFitMgr*, const std::string& tag);
static vector<RooRealVar*> MakeLognorm(SampleRooFitMgr*, const std::string& tag);
static vector<RooRealVar*> MakeLandau (SampleRooFitMgr*, const std::string& tag);
static vector<RooRealVar*> MakeTrial  (SampleRooFitMgr*, const std::string& tag);

//------------------------------------------------------------------------------
//   SampleRooFitMgr Generic MakePDF function for
//------------------------------------------------------------------------------
vector<RooRealVar*> SampleRooFitMgr::MakePDF( const string& fitfunc, const string& alias )
{
   static const string def = "Exo";

   if( GetPdfFromAlias(alias) ){ // Do not reproduce if already exists
      fprintf(
         stderr,
         "Warning! Duplicate name detected, not regenerating!\n"
      );
      return vector<RooRealVar*>();
   }

   if      ( fitfunc == "Exo"     ) { return MakeExo    ( this , alias ); }
   else if ( fitfunc == "Fermi"   ) { return MakeFermi  ( this , alias ); }
   else if ( fitfunc == "Lognorm" ) { return MakeLognorm( this , alias ); }
   else if ( fitfunc == "Landau"  ) { return MakeLandau ( this , alias ); }
   else if ( fitfunc == "Trial"   ) { return MakeTrial  ( this , alias ); }
   else if ( fitfunc == "Key"     ) { return MakeKeysPdf( this , alias ); }
   else {
      fprintf(
         stderr, "Warning! %s function not found, using %s\n",
         fitfunc.c_str(),
         def.c_str()
      );
      return MakePDF( def, alias );
   }
}

//------------------------------------------------------------------------------
//   Keys PDF
//------------------------------------------------------------------------------
vector<RooRealVar*> MakeKeysPdf(SampleRooFitMgr* sample, const string& tag )
{
   const string pdf_name = sample->MakePdfAlias( tag );
   RooKeysPdf*  pdf      = new RooKeysPdf(
      pdf_name.c_str(), pdf_name.c_str(),
      SampleRooFitMgr::x(),
      *(sample->OriginalDataSet()),
      RooKeysPdf::NoMirror,
      2.
   );
   sample->AddPdf( pdf );
   return vector<RooRealVar*>(); // returning empty vector
}

//------------------------------------------------------------------------------
//   Fermi function f(m) = 1/(1+exp((m-a)/b))
//------------------------------------------------------------------------------
vector<RooRealVar*> MakeFermi(SampleRooFitMgr* sample,const string& name = "fermi" )
{
   vector<RooRealVar*>  varlist;
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

   varlist.push_back(a); varlist.push_back(b);
   return varlist;
}

//------------------------------------------------------------------------------
//   EXO function f(m) = (1-x)^a / x^b ; x = m/sqrt(s)
//------------------------------------------------------------------------------
vector<RooRealVar*> MakeExo( SampleRooFitMgr* sample, const string& name="exo" )
{
   vector<RooRealVar*> varlist;
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

   varlist.push_back(a); varlist.push_back(b);
   return varlist;
}

//------------------------------------------------------------------------------
//   Lognormal distribution effective f(m) = 1/x * exp( -b ln^2(x/a))
//------------------------------------------------------------------------------
vector<RooRealVar*> MakeLognorm( SampleRooFitMgr* sample, const string& name ="lognorm" )
{
   vector<RooRealVar*> varlist;
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar*  a = var_mgr.NewVar( "a"+pdf_name, 300, 250    , 350 );
   RooRealVar*  b = var_mgr.NewVar( "b"+pdf_name, 1.6, 0.00001, 100 ); // could not be negative
   RooLognormal* pdf = new RooLognormal(
      pdf_name.c_str(), pdf_name.c_str(),
      SampleRooFitMgr::x(),  *a, *b
   );
   sample->AddPdf( pdf );
   varlist.push_back(a); varlist.push_back(b);
   return varlist;
}

//------------------------------------------------------------------------------
//   Landau distribution
//------------------------------------------------------------------------------
vector<RooRealVar*> MakeLandau( SampleRooFitMgr* sample, const string& name ="landau" )
{
   vector<RooRealVar*> varlist;
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar*  a = var_mgr.NewVar( "a"+pdf_name, 200, -10000,10000 );
   RooRealVar*  b = var_mgr.NewVar( "b"+pdf_name, 200, 0, 10000 ); // could not be negative
   RooLandau* pdf = new RooLandau(
      pdf_name.c_str(), pdf_name.c_str(),
      SampleRooFitMgr::x(),  *a, *b
   );
   sample->AddPdf( pdf );
   varlist.push_back(a); varlist.push_back(b);
   return varlist;
}

//------------------------------------------------------------------------------
//   Edit this function for testing purposes
//------------------------------------------------------------------------------
vector<RooRealVar*> MakeTrial( SampleRooFitMgr* sample,  const string& name="trial")
{
   vector<RooRealVar*> varlist;
   const string pdf_name = sample->MakePdfAlias( name );
   RooRealVar* a  = var_mgr.NewVar( "a" + pdf_name , 200,     0, +2000 );
   RooRealVar* b  = var_mgr.NewVar( "b" + pdf_name ,  0.01, -2000, +2000  );
   // RooRealVar* c  = var_mgr.NewVar( "c" + pdf_name , 100,  0.01 , +10000 );
   char formula[1024];
   sprintf( formula,
      "exp( -(x)/( (%s) + (%s)*x ) )",
      a->GetName(),
      b->GetName()
   );

   RooGenericPdf* pdf = new RooGenericPdf(
      pdf_name.c_str() , pdf_name.c_str(),
      formula,
      RooArgSet(SampleRooFitMgr::x(),*a,*b)
   );

   sample->AddPdf( pdf );

   varlist.push_back(a); varlist.push_back(b);
   return varlist;
}

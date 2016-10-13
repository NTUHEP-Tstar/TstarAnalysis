/*******************************************************************************
*
*  Filename    : Common_PDF.cc
*  Description : Definition of PDF generations
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include "RooGenericPdf.h"
#include "RooKeysPdf.h"
#include "RooLandau.h"
#include "RooLognormal.h"

#include <string>

using namespace std;

// ------------------------------------------------------------------------------
//   Function for defining pdf functions
// ------------------------------------------------------------------------------
static RooAbsPdf* MakeFermi( SampleRooFitMgr*, const std::string& tag );
static RooAbsPdf* MakeExo( SampleRooFitMgr*, const std::string& tag );
static RooAbsPdf* MakeLognorm( SampleRooFitMgr*, const std::string& tag );
static RooAbsPdf* MakeLandau( SampleRooFitMgr*, const std::string& tag );
static RooAbsPdf* MakeTrial( SampleRooFitMgr*, const std::string& tag );

// ------------------------------------------------------------------------------
//   SampleRooFitMgr Generic MakePDF function for
// ------------------------------------------------------------------------------
RooAbsPdf*
SampleRooFitMgr::NewPdf( const string& name, const std::string& fitfunc )
{
   static const string def = "Exo";

   if( Pdf( name ) ){// Do not reproduce if already exists
      fprintf(
         stderr,
         "Warning! Duplicate name detected, not regenerating!\n"
         );
      return Pdf( name );
   }

   if( fitfunc == "Exo"     ){
      return MakeExo( this, name );
   } else if( fitfunc == "Fermi"   ){
      return MakeFermi( this, name );
   } else if( fitfunc == "Lognorm" ){
      return MakeLognorm( this, name );
   } else if( fitfunc == "Landau"  ){
      return MakeLandau( this, name );
   } else if( fitfunc == "Trial"   ){
      return MakeTrial( this, name );
   } else {
      fprintf(
         stderr, "Warning! %s function not found, using %s\n",
         fitfunc.c_str(),
         def.c_str()
         );
      return NewPdf( def, name );
   }
}

// ------------------------------------------------------------------------------
//   Fermi function f(m) = 1/(1+exp((m-a)/b))
// ------------------------------------------------------------------------------
RooAbsPdf*
MakeFermi( SampleRooFitMgr* sample, const string& name = "fermi" )
{
   static char formula[1024];
   RooRealVar* a = sample->NewVar( name+"a", 100, -1000, 1000 );
   RooRealVar* b = sample->NewVar( name+"b", 100, -1000, 1000 );

   sprintf( formula, "1/(1+exp((x-%s)/%s))", a->GetName(), b->GetName() );

   RooGenericPdf* pdf = new RooGenericPdf(
      name.c_str(), name.c_str(),
      formula,
      RooArgSet( SampleRooFitMgr::x(), *a, *b )
      );

   sample->AddPdf( pdf );
   return pdf;
}

// ------------------------------------------------------------------------------
//   EXO function f(m) = (1-x)^a / x^b ; x = m/sqrt(s)
// ------------------------------------------------------------------------------
RooAbsPdf*
MakeExo( SampleRooFitMgr* sample, const string& name = "exo" )
{
   static char formula[1024];
   RooRealVar* a = sample->NewVar( name+"a", 61.7, 0, 100 );
   RooRealVar* b = sample->NewVar( name+"b", 0.18, 0, 100 );
   sprintf(
      formula, "(TMath::Power((1-(x/13000.)),(%s)))/(TMath::Power((x/13000.),(%s)))",
      a->GetName(),
      b->GetName()
      );

   RooGenericPdf* pdf = new RooGenericPdf(
      name.c_str(), name.c_str(),
      formula,
      RooArgSet( SampleRooFitMgr::x(), *a, *b )
      );

   sample->AddPdf( pdf );

   return pdf;
}

// ------------------------------------------------------------------------------
//   Lognormal distribution effective f(m) = 1/x * exp( -b ln^2(x/a))
// ------------------------------------------------------------------------------
RooAbsPdf*
MakeLognorm( SampleRooFitMgr* sample, const string& name = "lognorm" )
{
   RooRealVar* a     = sample->NewVar( name+"a", 300, 0, 1000 );
   RooRealVar* b     = sample->NewVar( name+"b", 1.6, 0.00001, 100 );// could not be negative
   RooLognormal* pdf = new RooLognormal(
      name.c_str(), name.c_str(),
      SampleRooFitMgr::x(),  *a, *b
      );
   sample->AddPdf( pdf );
   return pdf;
}

// ------------------------------------------------------------------------------
//   Landau distribution
// ------------------------------------------------------------------------------
RooAbsPdf*
MakeLandau( SampleRooFitMgr* sample, const string& name = "landau" )
{
   RooRealVar* a  = sample->NewVar( name+"a", 200, -10000, 10000 );
   RooRealVar* b  = sample->NewVar( name+"b", 200, 0, 10000 ); // could not be negative
   RooLandau* pdf = new RooLandau(
      name.c_str(), name.c_str(),
      SampleRooFitMgr::x(),  *a, *b
      );
   sample->AddPdf( pdf );
   return pdf;
}

// ------------------------------------------------------------------------------
//   Edit this function for testing purposes
// ------------------------------------------------------------------------------
RooAbsPdf*
MakeTrial( SampleRooFitMgr* sample,  const string& name = "trial" )
{
   static char formula[1024];
   RooRealVar* a = sample->NewVar( "a" + name, 200,     0, +2000 );
   RooRealVar* b = sample->NewVar( "b" + name,  0.01, -2000, +2000  );
   sprintf( formula,
      "exp( -(x)/( (%s) + (%s)*x ) )",
      a->GetName(),
      b->GetName()
      );

   RooGenericPdf* pdf = new RooGenericPdf(
      name.c_str(), name.c_str(),
      formula,
      RooArgSet( SampleRooFitMgr::x(), *a, *b )
      );

   sample->AddPdf( pdf );
   return pdf;
}

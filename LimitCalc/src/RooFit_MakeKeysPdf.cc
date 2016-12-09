/*******************************************************************************
*
*  Filename    : RooFit_MakeKeysPdf.cc
*  Description : Implemetations related
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include "RooAddPdf.h"
#include "RooConstVar.h"
#include "RooDataSet.h"// Must have unbinned dataset parts
#include "RooFormulaVar.h"
#include "RooGenericPdf.h"
#include "RooKeysPdf.h"
#include "TF1.h"
#include <string>
#include <vector>

using namespace std;

/*******************************************************************************
*   Naming definition
*******************************************************************************/
string
KeyPdfName( const string& datasetname )
{
   return datasetname + "key";
}

/******************************************************************************/

string
KeyNormName( const string& datasetname )
{
   return datasetname + "_norm";
}

/******************************************************************************/

extern const string StitchKeyPdfName  = "keymaster";
extern const string StitchKeyNormName = "keymaster_norm";

/*******************************************************************************
*   Keys PDF creation
*******************************************************************************/
RooKeysPdf*
MakeSingleKeysPdf( SampleRooFitMgr* sample, const string& datasetname )
{
   // Directly returning zero if corresponding dataset is not found;
   if( sample->DataSet( datasetname ) == NULL ){ return NULL; }

   // Caching objects
   const string keypdfname  = KeyPdfName( datasetname );
   const string keynormname = KeyNormName( datasetname );
   RooDataSet& dataset      = *( (RooDataSet*)( sample->DataSet( datasetname ) ) );

   // Early exit if keys PDF is already created
   if( sample->Pdf( keypdfname ) ){
      return (RooKeysPdf*)( sample->Pdf( keypdfname ) );
   }


   // Running roofit KeysPdf construction`
   RooKeysPdf* pdf = new RooKeysPdf(
      keypdfname.c_str(),
      keypdfname.c_str(),
      SampleRooFitMgr::x(),
      dataset,
      RooKeysPdf::NoMirror,
      2.
      );
   sample->AddPdf( pdf );

   // Making normalization as RooConstVar
   RooConstVar* norm = new RooConstVar(
      keynormname.c_str(),
      keynormname.c_str(),
      dataset.sumEntries()
      );
   sample->AddFunc( norm );// Normalization is a function!

   cout << ">>> " << datasetname << ":" << dataset.sumEntries() << endl;

   return pdf;
}

/******************************************************************************/

RooAbsPdf*
MakeFullKeysPdf( SampleRooFitMgr* sample )
{
   // Should only attempt to run this function once;
   // Early exit with previous result other wise;
   if( sample->Pdf( StitchKeyPdfName ) ){
      return (RooAddPdf*)( sample->Pdf( StitchKeyPdfName ) );
   }


   // Ensure that all Keys PDF have been made
   for( const auto& datasetname : sample->SetNameList() ){
      MakeSingleKeysPdf( sample, datasetname );
   }

   vector<std::pair<std::string, std::string> > stitchlist;

   for( const auto& source : uncsource ){
      stitchlist.push_back(
         std::make_pair(
            KeyPdfName( source + "Up" ),
            KeyPdfName( source + "Down" )
            )
         );
   }

   // Making pdf stitching.
   return MakeMultiJoinPdf(
      sample,
      StitchKeyPdfName,
      KeyPdfName( "" ),
      stitchlist
      );

   /* Joining via Better PDF definitions is still not working

      // Joining function together
      RooAbsPdf* currentpdf   = sample->Pdf( KeyPdfName( "" ) );// Getting central value pdf
      RooAbsReal* currentnorm = sample->Func( KeyNormName( "" ) );

      for( const auto& source : uncsource ){
      RooAbsPdf* uppdf     = sample->Pdf( KeyPdfName( source+"Up" ) );
      RooAbsPdf* downpdf   = sample->Pdf( KeyPdfName( source+"Down" ) );
      RooAbsReal* upnorm   = sample->Func( KeyNormName( source + "Up" ) );
      RooAbsReal* downnorm = sample->Func( KeyNormName( source + "Down" ) );

      // Typing required for binFunction
      RooAbsReal* intvar = sample->NewVar( source+"coeff", 0, -1, 1 );

      // Creating pdf object for shape

      const RooArgList pdflist = RooArgList( *intvar, *currentpdf, *uppdf, *downpdf );

      RooAbsPdf* joinpdf = new RooGenericPdf(
         JoinPdfName( source ).c_str(),
         JoinPdfName( source ).c_str(),
         "@1 * ( 1 - TMath::Abs(@0))  + @2 * @0 * ( @0 > 1 ) + @3 * @0 * (@0 < 1)",
         pdflist
         );
      sample->AddPdf( joinpdf );

      // Making requirements for coefficients
      const RooArgList normlist = RooArgList( *intvar, *currentnorm, *upnorm, *downnorm );

      // Creating RooRealVar for normalization
      RooAbsReal* joinnorm = new RooFormulaVar(
         JoinNormName( source ).c_str(),
         JoinNormName( source ).c_str(),
         "@1 * ( 1 - TMath::Abs(@0))  + @2 * @0 * ( @0 > 1 ) + @3 * @0 * (@0 < 1)",
         normlist
         );
      sample->AddFunc( joinnorm );

      // Setting interval to constant for fixing central value
      ( (RooRealVar*)( intvar ) )->setConstant( kTRUE );

      // Shifting for next
      currentpdf  = joinpdf;
      currentnorm = joinnorm;
      }

      return currentpdf;
    */
}

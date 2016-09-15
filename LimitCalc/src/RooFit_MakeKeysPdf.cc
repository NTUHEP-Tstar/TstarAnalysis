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
#include "RooKeysPdf.h"
#include <string>
#include <vector>

using namespace std;
// ------------------------------------------------------------------------------
//   Naming definition
// ------------------------------------------------------------------------------
string
KeyPdfName( const string& datasetname )
{
   return datasetname + "key";
}

extern const string StitchKeyPdfName = "keymaster";

// ------------------------------------------------------------------------------
//   Keys pdf creating
// ------------------------------------------------------------------------------
RooKeysPdf*
MakeSingleKeysPdf( SampleRooFitMgr* sample, const string& datasetname )
{
   // Directly returning zero if corresponding dataset is not found;
   if( sample->DataSet( datasetname ) == NULL ){ return NULL; }

   // Early exit if keys PDF is already created
   const string keypdfname = KeyPdfName( datasetname );
   if( sample->Pdf( keypdfname ) ){
      return (RooKeysPdf*)( sample->Pdf( keypdfname ) );
   }

   // Running roofit KeysPdf construction`
   RooKeysPdf* pdf = new RooKeysPdf(
      ( keypdfname ).c_str(),
      ( keypdfname ).c_str(),
      SampleRooFitMgr::x(),
      *( sample->DataSet( datasetname ) ),
      RooKeysPdf::NoMirror,
      2.
      );
   sample->AddPdf( pdf );
   return pdf;
}


RooAddPdf*
MakeFullKeysPdf( SampleRooFitMgr* sample )
{
   // Should only attempt to run this function once;
   // Early exit with previous result other wise;
   if( sample->Pdf( StitchKeyPdfName ) ){
      return (RooAddPdf*)( sample->Pdf( StitchKeyPdfName ) );
   }

   // Detecting all pdfs
   vector<string> keypdfnamelist;

   for( const auto& datasetname : sample->SetNameList() ){
      if( MakeSingleKeysPdf( sample, datasetname ) ){// Checking if dataset / key pdf exists
         const string keypdfname = datasetname + "key";
         keypdfnamelist.push_back( keypdfname );
      }
   }

   // Making pdf stitching.
   return MakeStichPdf(
      sample,
      StitchKeyPdfName,
      keypdfnamelist
      );
}

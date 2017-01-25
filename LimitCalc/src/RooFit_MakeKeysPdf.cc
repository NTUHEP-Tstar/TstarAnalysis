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
  return KeyPdfName(datasetname) + "_norm";
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

  // Change rho factor according to the dataset name (1.38 as central value)
  const double rho =
    datasetname.find( "model" ) == string::npos ? 1.38 :
    datasetname.find( "Up" )    == string::npos ? 1.18 :
    1.68;

  // Running roofit KeysPdf construction`
  RooKeysPdf* pdf = new RooKeysPdf(
    keypdfname.c_str(),
    keypdfname.c_str(),
    SampleRooFitMgr::x(),
    dataset,
    RooKeysPdf::NoMirror,
    rho
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
  vector<string> simplelist;
  simplelist.push_back( KeyPdfName( "" ) );

  for( const auto& source : uncsource ){
    stitchlist.push_back(
      std::make_pair(
        KeyPdfName( source + "Up" ),
        KeyPdfName( source + "Down" )
        )
      );
    simplelist.push_back( KeyPdfName( source + "Up" ) );
    simplelist.push_back( KeyPdfName( source + "Down" ) );
  }


  // Making pdf stitching.
  return MakeSimpleStitchPdf(
    sample,
    StitchKeyPdfName,
    simplelist
    );

  // Complex version doesn't work for now
  // return MakeMultiJoinPdf(
  //   sample,
  //   StitchKeyPdfName,
  //   KeyPdfName( "" ),
  //   stitchlist
  //   );
}

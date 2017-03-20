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
#include "RooBifurGauss.h"
#include "RooConstVar.h"
#include "RooDataSet.h"// Must have unbinned dataset parts
#include "RooFFTConvPdf.h"
#include "RooFormulaVar.h"
#include "RooGaussian.h"
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
  return KeyPdfName( datasetname ) + "_norm";
}

/******************************************************************************/

extern const string StitchKeyPdfName  = "keymaster";
extern const string StitchKeyNormName = "keymaster_norm";

/*******************************************************************************
*   Keys PDF creation
*******************************************************************************/
RooAbsPdf*
MakeSingleKeysPdf( SampleRooFitMgr* sample, const string& datasetname )
{
  // Directly returning zero if corresponding dataset is not found;
  if( sample->DataSet( datasetname ) == NULL ){ return NULL; }

  const string keypdfname  = KeyPdfName( datasetname );
  const string keynormname = KeyNormName( datasetname );


  // Early exit if keys PDF is already created
  if( sample->Pdf( keypdfname ) ){
    return (RooKeysPdf*)( sample->Pdf( keypdfname ) );
  }
  RooAbsPdf* ans;

  if( limnamer.CheckInput( "scaleres" ) ){// Modify data set
    cout << "Re-scaling resolution" << endl;

    RooRealVar& x        = SampleRooFitMgr::x();
    RooRealVar& w        = SampleRooFitMgr::w();
    RooDataSet* origdata = (RooDataSet*)( sample->DataSet( datasetname ) );
    RooDataSet* newdata  = new RooDataSet( datasetname.c_str(), "",
      RooArgSet( x, w ),
      RooFit::WeightVar( w )
      );

    const double origmean = origdata->mean( x );

    for( int i = 0; i < origdata->numEntries(); ++i ){
      // x is a copyt of the dataset and not the acutal value

      const RooArgSet* set = origdata->get(i) ;

      RooRealVar* origx = (RooRealVar*)( set->find( SampleRooFitMgr::x() ) );

      // Re-assigning value
      const double newx = origmean + ( origx->getVal()-origmean ) * limnamer.GetInput<double>( "scaleres" );
      const double neww = origdata->weight();

      if( newx < SampleRooFitMgr::MaxMass() && newx > SampleRooFitMgr::MinMass() ){
        x = newx;
        newdata->add( RooArgSet( x ), neww );
      }
    }

    cout << "orig:" << origdata->sigma( x ) << " | new:" << newdata->sigma( x ) << endl;


    // Resetting ownership
    sample->RemoveDataSet( datasetname );
    sample->AddDataSet( newdata );
  }

  // Caching objects
  RooDataSet& dataset = *( (RooDataSet*)( sample->DataSet( datasetname ) ) );

  if( !limnamer.CheckInput( "useparam" ) ){// Defaults to using Keys PDF
    // Change rho factor according to the dataset name (1.36 as central value)
    const double rho =
      datasetname.find( "model" ) == string::npos ? 1.36 :
      datasetname.find( "Up" )    == string::npos ? 1.17 :
      1.66;

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
    ans = pdf;

  } else {// Using Bifucated gaussian (x) gaussian
    RooRealVar* bimean1 = sample->NewVar(    keypdfname + "bimean1", GetInt( sample->Name() ), 400, 2000 );
    RooRealVar* bil1    = sample->NewVar(    keypdfname + "bil1", 200, 0, 1000 );
    RooRealVar* bir1    = sample->NewVar(    keypdfname + "bir1", 300, 0, 1000 );
    RooAbsPdf* bipdf1   = new RooBifurGauss( ( keypdfname + "bigauss1" ).c_str(), "", SampleRooFitMgr::x(), *bimean1, *bil1, *bir1 );

    RooConstVar* gmean1 = new RooConstVar( ( keypdfname+"convgmean1" ).c_str(), "", 0 );
    RooRealVar* gsig1   = sample->NewVar(  keypdfname+"convgsig1", 100, 0, 1000 );
    RooAbsPdf* gpdf1    = new RooGaussian( ( keypdfname+"convgauss1" ).c_str(), "", SampleRooFitMgr::x(), *gmean1, *gsig1 );

    RooAbsPdf* conv1pdf = new RooFFTConvPdf( ( keypdfname+"conv1" ).c_str(), "", SampleRooFitMgr::x(), *bipdf1, *gpdf1 );

    RooRealVar* bimean2 = sample->NewVar(    keypdfname+"bimean2", GetInt( sample->Name() )+100, 400, 2000 );
    RooRealVar* bil2    = sample->NewVar(    keypdfname+"bil2", 100, 0, 1000 );
    RooRealVar* bir2    = sample->NewVar(    keypdfname+"bir2", 200, 0, 1000 );
    RooAbsPdf* bipdf2   = new RooBifurGauss( ( keypdfname+"bigauss2" ).c_str(), "", SampleRooFitMgr::x(), *bimean2, *bil2, *bir2 );

    RooConstVar* gmean2 = new RooConstVar( ( keypdfname+"convgmean2" ).c_str(), "", 0 );
    RooRealVar* gsig2   = sample->NewVar(  keypdfname+"convgsig2", 100, 0, 1000 );
    RooAbsPdf* gpdf2    = new RooGaussian( ( keypdfname+"convgauss2" ).c_str(), "", SampleRooFitMgr::x(), *gmean2, *gsig2 );

    RooAbsPdf* conv2pdf = new RooFFTConvPdf( ( keypdfname+"conv2" ).c_str(), "", SampleRooFitMgr::x(), *bipdf2, *gpdf2 );

    RooRealVar* bico = sample->NewVar( keypdfname+"bijoin", 0, 1 );

    RooAbsPdf* bisum = new RooAddPdf( keypdfname.c_str(), "", *conv1pdf, *conv2pdf, *bico );

    bisum->fitTo(// Forcing fit, and setting to constant
      dataset,
      RooFit::Minimizer( "Minuit", "Migrad" ),
      RooFit::Minos( kTRUE ),
      RooFit::SumW2Error( kTRUE ),
      RooFit::Verbose( kFALSE ),
      RooFit::PrintLevel( -1 ),
      RooFit::PrintEvalErrors( -1 ),
      RooFit::Warnings( kFALSE )
      );

    // make sample claim ownership of pdfs
    sample->AddPdf( bipdf1 );
    sample->AddPdf( gpdf1 );
    sample->AddPdf( conv1pdf );
    sample->AddPdf( bipdf2 );
    sample->AddPdf( gpdf2 );
    sample->AddPdf( conv2pdf );
    sample->AddPdf( bisum );

    sample->AddFunc( gmean1 );
    sample->AddFunc( gmean2 );

    sample->SetConstant( kTRUE );// Freezing all variables.

    // make sample claim ownership of pdfs
    ans = bisum;
  }


  // Making normalization as RooConstVar
  RooConstVar* norm = new RooConstVar(
    keynormname.c_str(),
    keynormname.c_str(),
    dataset.sumEntries()
    );

  sample->AddFunc( norm );// Normalization is a function!

  cout << ">>> " << datasetname << ":" << dataset.sumEntries() << endl;

  return ans;
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

/*******************************************************************************
*
*  Filename    : Common_PdfStitch.cc
*  Description : Pdf stitching functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"

#include "RooAddPdf.h"
#include "RooGenericPdf.h"
#include <iostream>

using namespace std;

/*******************************************************************************
*   Defining naming conventaion as static functions of JoinRequest object
*******************************************************************************/
// static version for external query
std::string
JoinRequest::JoinCoeffName(  const std::string& joinname )
{
  return joinname + "coeff";
}

/******************************************************************************/
std::string
JoinRequest::JoinCoeffName() const
{
  return JoinRequest::JoinCoeffName( joinname );
}

/*******************************************************************************
*   Single Stitch function
*******************************************************************************/
static const std::string LinearInterpolateForm =
  "@1 * ( 1 - TMath::Abs(@0) ) + @2 * @0 * (@0 > 0 ) + @3 * (-@0) * (@0 < 0 )";


/******************************************************************************/

RooAbsPdf*
MakeJoinPdf(
  SampleRooFitMgr*   sample,
  const JoinRequest& x
  )
{
  if( sample->Pdf( x.joinname ) ){
    return sample->Pdf( x.joinname );
  }

  RooRealVar* coeff  = sample->NewVar( x.JoinCoeffName(), 0, -1, 1 );
  RooAbsPdf* central = sample->Pdf( x.centralpdfname );
  RooAbsPdf* up      = sample->Pdf( x.uppdfname );
  RooAbsPdf* down    = sample->Pdf( x.downpdfname );

  RooGenericPdf* joinpdf = new RooGenericPdf(
    x.joinname.c_str(), x.joinname.c_str(),
    LinearInterpolateForm.c_str(),
    RooArgList( *coeff, *central, *up, *down )
    );

  coeff->setConstant( kTRUE );
  sample->AddPdf( joinpdf );
  return joinpdf;
}

/******************************************************************************/


RooAbsPdf*
MakeMultiJoinPdf(
  SampleRooFitMgr* sample,
  const string& stitchname,
  const string& centralname,
  const vector<pair<string, string> >& joinlist
  )
{
  if( sample->Pdf( stitchname ) ){
    return sample->Pdf( stitchname );
  }

  string opcen   = centralname;
  RooAbsPdf* ans = sample->Pdf( centralname );

  for( const auto& joinpair : joinlist  ){
    const string joinname = ( joinpair == joinlist.back() ) ?
                            stitchname :
                            joinpair.first + "plus" + joinpair.second;

    JoinRequest req = {
      joinname,
      opcen,
      joinpair.first,
      joinpair.second
    };

    ans   = MakeJoinPdf( sample, req );
    opcen = joinname;
  }

  return ans;
}


/*******************************************************************************
*   Simple stitching function
*******************************************************************************/
RooAbsPdf*
MakeSimpleStitchPdf(
  SampleRooFitMgr*      sample,
  const string&         stitchname,
  const vector<string>& pdfnamelist
  )
{
  if( sample->Pdf( stitchname ) ){
    return sample->Pdf( stitchname );
  }

  RooArgList pdflist;
  RooArgList coefflist;
  vector<string> coeffnamelist;

  // Creating PDF list and coeffname;
  for( const string& pdfname : pdfnamelist ){
    if( ! sample->Pdf( pdfname ) ) { continue; }
    if( pdfname != pdfnamelist.front()  ){
      // Skipping the first, shifting it to the last
      pdflist.add( *( sample->Pdf( pdfname ) ) );
      coeffnamelist.push_back( stitchname + "coeff_" + pdfname );
    } else {

    }
  }

  pdflist.add( *( sample->Pdf( pdfnamelist.front() ) ) );

  // Creating Coefficient and coefficient list
  for( const string& coeffname : coeffnamelist ){
    RooRealVar* coeff = sample->NewVar( coeffname, 0.00001, 0, 1 );
    *coeff = 0.00001 ;
    coeff->setConstant( kTRUE );  // freezing everying!
    coefflist.add( *coeff );
  }
  cout << "PDF List size:" << pdflist.getSize() << endl;
  cout << "Joining object List size:" << coefflist.getSize() << endl; 

  // Creating add pdf list
  RooAddPdf* ans = new RooAddPdf(
    stitchname.c_str(), stitchname.c_str(),
    pdflist,
    coefflist
    );
  sample->AddPdf( ans );
  sample->SetConstant(kTRUE); // Freezing coefficients
  return ans;
}

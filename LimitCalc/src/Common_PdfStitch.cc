/*******************************************************************************
*
*  Filename    : Common_PdfStitch.cc
*  Description : Pdf stitching functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "RooAddPdf.h"
#include "RooArgList.h"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include <string>
#include <vector>

using namespace std;

RooAddPdf*
MakeStichPdf(
   SampleRooFitMgr*      sample,
   const string&         stitchname,
   const vector<string>& pdfnamelist
   )
{
   if( sample->Pdf( stitchname ) ){ return (RooAddPdf*)( sample->Pdf( stitchname ) ); }

   RooArgList pdflist;
   RooArgList coefflist;
   vector<string> coeffnamelist;

   for( const string& pdfname : pdfnamelist ){
      if( sample->Pdf( pdfname ) ){
         pdflist.add( *( sample->Pdf( pdfname ) ) );
         coeffnamelist.push_back( stitchname + "coeff" + to_string( coeffnamelist.size() ) );
      }
   }

   if( coeffnamelist.size() < 2 ){ return NULL; }// Error! cannot create a RooAdd Pdf with less than one pdf

   coeffnamelist.pop_back();// Last coeff is to be drop

   for( const string& coeffname : coeffnamelist ){
      RooRealVar* coeff = sample->NewVar( coeffname, 0, 1 );
      if( coeffname == coeffnamelist.front() ){ *coeff = 1.; }// The first one is dominant
      else                                    { *coeff = 0.; }
      coeff->setConstant( kTRUE );// freezing everying!
      coefflist.add( *coeff );
   }

   RooAddPdf* ans = new RooAddPdf(
      stitchname.c_str(), stitchname.c_str(),
      pdflist,
      coefflist
      );
   sample->AddPdf( ans );
   return ans;
}

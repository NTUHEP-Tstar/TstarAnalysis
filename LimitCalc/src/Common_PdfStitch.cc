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

   // Creating PDF list and coeffname;
   for( const string& pdfname : pdfnamelist ){
      if( pdfname != pdfnamelist.front() && sample->Pdf( pdfname ) ){
         // Skipping the first, shifting it to the last
         pdflist.add( *( sample->Pdf( pdfname ) ) );
         coeffnamelist.push_back( stitchname + "coeff" + to_string( coeffnamelist.size() ) + "_" + pdfname );
      }
   }

   pdflist.add( *( sample->Pdf( pdfnamelist.front() ) ) );

   // Creating Coefficient and coefficient list
   for( const string& coeffname : coeffnamelist ){
      RooRealVar* coeff  = sample->NewVar( coeffname, 0, 1 );
      *coeff = 0.;
      coeff->setConstant( kTRUE );// freezing everying!
      coefflist.add( *coeff );
   }

   // Creating add pdf list
   RooAddPdf* ans = new RooAddPdf(
      stitchname.c_str(), stitchname.c_str(),
      pdflist,
      coefflist
      );
   sample->AddPdf( ans );
   return ans;
}

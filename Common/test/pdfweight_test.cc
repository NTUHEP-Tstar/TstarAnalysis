/*******************************************************************************
*
*  Filename    : pdfweight_test.cc
*  Description : Unit testing for PDF weighs
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"

#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include <iostream>

using namespace std;
int
main( int argc, char const* argv[] )
{

  const auto& pdfidgroup = GetPdfIdGrouping( argv[1] );

  unsigned i = 0;

  for( const auto& idgroup : pdfidgroup ){
    cout << "Group " << i << endl;

    for( const auto& id : idgroup ){
      cout << "\t" << id;
    }

    cout << endl;
    ++i;
  }

  fwlite::Event ev( TFile::Open( argv[1] ) );
  fwlite::Handle<LHEEventProduct> evthandle;
  evthandle.getByLabel( ev, "externalLHEProducer" );
  double pdfavg   = 0;
  double qcdupavg = 0;
  double qcddwavg = 0;
  double qcdavg   = 0;
  unsigned numevt = 0;

  TH1D hist( "pdfweight", "", 50, -2, 3 );

  for( ev.toBegin(); !ev.atEnd(); ++ev, ++numevt ){
    evthandle.getByLabel( ev, "externalLHEProducer" );
    cout << GetPdfWeightError( ev, pdfidgroup ) << " "
        << GetScaleWeightError(ev,pdfidgroup) << " "
         << GetScaleWeightUpError( ev, pdfidgroup )  << " "
         << GetScaleWeightDownError( ev, pdfidgroup ) << "\t|"
         << evthandle.ref().weights().front().id << " "
         << evthandle.ref().weights().front().wgt << " "
         <<endl;

    pdfavg   += GetPdfWeightError( ev, pdfidgroup );
    qcdavg   += GetScaleWeightError( ev, pdfidgroup );
    qcdupavg += GetScaleWeightUpError( ev, pdfidgroup );
    qcddwavg += GetScaleWeightDownError( ev, pdfidgroup );

    const auto& idlist = pdfidgroup[1];
    for( const auto& weightinfo : evthandle.ref().weights() ){
      const unsigned thisid   = stoi( weightinfo.id );
      const double thisweight = weightinfo.wgt / evthandle.ref().weights().front().wgt;
      if( find( idlist.begin(), idlist.end(), thisid ) != idlist.end() ){
        hist.Fill( thisweight );
      }
    }
  }

  TCanvas c( "c", "", 500, 500 );
  hist.Draw();
  c.SaveAs("tmp.pdf");

  cout << "Average:"
       << pdfavg / numevt  << " "
       << qcdavg / numevt  << " "
       << qcdupavg / numevt << " "
       << qcddwavg / numevt << " "
       << endl;

  return 0;
}

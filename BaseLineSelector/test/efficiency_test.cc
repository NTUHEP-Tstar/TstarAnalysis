/*******************************************************************************
 *
 *  Filename    : efficiency_test.cc
 *  Description : Simple test on how well the efficiency_test could be computed
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "DataFormats/Common/interface/MergeableCounter.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/LuminosityBlock.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "TFile.h"

#include <iostream>

using namespace std;

int main(int argc, char const *argv[]) {
   fwlite::LuminosityBlock Lumi( TFile::Open("tstarBaseline.root")) ;
   fwlite::Handle<edm::MergeableCounter> positiveHandle;
   fwlite::Handle<edm::MergeableCounter> negativeHandle;
   for( Lumi.toBegin() ; ! Lumi.atEnd() ; ++Lumi ){
      positiveHandle.getByLabel( Lumi , "beforeAny" , "positiveEvents" );
      negativeHandle.getByLabel( Lumi , "beforeAny" , "negatvieEvents" );

      cout << positiveHandle->value;
      cout << negativeHandle->value;
   }
   return 0;
}

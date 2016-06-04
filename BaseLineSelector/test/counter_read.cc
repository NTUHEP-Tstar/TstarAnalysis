/*******************************************************************************
 *
 *  Filename    : counter_read.cc
 *  Description : Reading the counters
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/

#include "DataFormats/Common/interface/MergeableCounter.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/LuminosityBlock.h"
#include "DataFormats/FWLite/interface/Run.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/Common/interface/MergeableCounter.h"

#include "TFile.h"
#include <iostream>
using namespace std;

int main(int argc, char const *argv[]) {
   fwlite::Run myRun( TFile::Open("merge.root") );
   fwlite::Handle<edm::MergeableCounter>  positive;
   for( myRun.toBegin() ; !myRun.atEnd() ; ++myRun ){
      positive.getByLabel( myRun , "beforeAny" , "positiveEvents" );
      cout << myRun.id() << ": " << positive->value << endl;
   }

   return 0;
}

#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"

#include "DataFormats/Common/interface/MergeableCounter.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"

#include <iostream>
#include <vector>

using namespace std;

int
main( int argc, char const* argv[] )
{
   fwlite::Event ev( TFile::Open( "test_data.root" ) );
   fwlite::Handle<LHEEventProduct> lhehandle;

   TH1D scaleplot( "test", "test", 60, -8000, 8000 );

   for( ev.toBegin(); !ev.atEnd(); ++ev ){
      lhehandle.getByLabel( ev, "externalLHEProducer" );
      scaleplot.Fill( lhehandle->hepeup().XWGTUP );
      cout << ev.isRealData() << " " << lhehandle->hepeup().XWGTUP << endl;
   }

   fwlite::Run run( TFile::Open( "test_bg.root" ) );
   fwlite::Handle<edm::MergeableCounter> counter;

   for( run.toBegin(); !run.atEnd(); ++run ){
      counter.getByLabel( run, "afterBaseLine", "positiveEvents" );
      cout << counter->value << endl;
      counter.getByLabel( run, "afterBaseLine", "negativeEvents" );
      cout << counter->value << endl;
   }

   TCanvas c( "c", "c" );
   scaleplot.Draw();
   c.SaveAs( "test.png" );

   return 0;
}

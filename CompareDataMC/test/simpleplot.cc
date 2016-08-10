#include "DataFormats/FWLite/interface/ChainEvent.h"
#include "DataFormats/FWLite/interface/Handle.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"

#include <vector>

int main(int argc, char const *argv[]) {
   fwlite::Event ev( TFile::Open("test.root") );
   fwlite::Handle<std::vector<pat::Jet>>  jethandle;

   TH1D scaleplot("scale","scale", 50, 0 , 50 );

   for( ev.toBegin() ; ! ev.atEnd() ; ++ev ){
      jethandle.getByLabel( ev, "skimmedPatJets" );
      for( const auto& jet : *jethandle ){
         scaleplot.Fill( 1. + jet.userFloat("jecunc") );
      }
   }

   TCanvas c("c","c");
   scaleplot.Draw();
   c.SaveAs("test.png");

   return 0;
}

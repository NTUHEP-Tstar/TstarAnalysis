/*******************************************************************************
 *
 *  Filename    : MakePileupWeights.cc
 *  Description : Writing pile up weights to a single file.
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "TH1D.h"
#include "TFile.h"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char const *argv[]) {
   TFile* mcfile   = TFile::Open("results/mcpileuphist.root");
   TFile* datafile = TFile::Open("results/pileuphist.root");

   TH1D* mchist    = (TH1D*)(mcfile->Get("MCPileUpHist/pu")->Clone());
   TH1D* datahist  = (TH1D*)(datafile->Get("pileup")->Clone());

   vector<float> mcpu;
   vector<float> datapu;
   for( int i = 1 ;  i <= 50 ; ++i ){
      mcpu.push_back( mchist->GetBinContent(i) );
      datapu.push_back( datahist->GetBinContent(i) );
   }

   FILE* results = fopen( "data/pileupweights.csv", "w" );
   edm::LumiReWeighting* rw = new edm::LumiReWeighting( mcpu , datapu );
   for( int i = 0 ; i < 50 ; ++i ){
      fprintf( results, "%lf\n", rw->weight(i) );
   }
   cout << "Done!" << endl;
   fclose(results);

   mcfile->Close();
   datafile->Close();
   return 0;
}

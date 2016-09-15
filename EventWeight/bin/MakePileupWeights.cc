/*******************************************************************************
*
*  Filename    : MakePileupWeights.cc
*  Description : Writing pile up weights to a single file.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "TFile.h"
#include "TH1D.h"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

vector<int> xseclist = { 62000, 69200, 71260 };

int
main( int argc, char const* argv[] )
{
   for( auto& xsec : xseclist ){
      char datafilename[256];
      char outputfile[256];
      sprintf( datafilename, "results/pileuphist_%d.root", xsec );
      sprintf( outputfile,   "data/pileupweights_%d.csv",  xsec );

      TFile* mcfile   = TFile::Open( "results/mcpileuphist.root" );
      TFile* datafile = TFile::Open( datafilename );
      TH1D* mchist    = (TH1D*)( mcfile->Get( "MCPileUpHist/pu" )->Clone() );
      TH1D* datahist  = (TH1D*)( datafile->Get( "pileup" )->Clone() );

      vector<float> mcpu;
      vector<float> datapu;

      for( int i = 1; i <= 50; ++i ){
         mcpu.push_back( mchist->GetBinContent( i ) );
         datapu.push_back( datahist->GetBinContent( i ) );
      }

      FILE* results            = fopen( outputfile, "w" );
      edm::LumiReWeighting* rw = new edm::LumiReWeighting( mcpu, datapu );

      for( int i = 0; i < 50; ++i ){
         fprintf( results, "%lf\n", rw->weight( i ) );
      }

      cout << "Done!" << endl;
      fclose( results );

      mcfile->Close();
      datafile->Close();

   }

   return 0;
}

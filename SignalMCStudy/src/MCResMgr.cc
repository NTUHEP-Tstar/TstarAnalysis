/*******************************************************************************
*
*  Filename    : Implementation of filling and plotting functions
*  Description : One Line descrption of file contents
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/SignalMCStudy/interface/Common.hpp"
#include "TstarAnalysis/SignalMCStudy/interface/MCResMgr.hpp"

#include "TFile.h"
#include "TTree.h"
#include <boost/format.hpp>
#include <iostream>

using namespace std;
using namespace mgr;

MCResMgr::MCResMgr( const string& name ) :
  Named( name ),
  HistMgr( name )
{
  AddHist( "HadWMass",      "Hadronic W mass (truth reconstruction)",   "GeV/c^{2}",  80,    0,  240 );
  AddHist( "HadTopMass",    "Hadronic top mass (truth reconstuction)",  "GeV/c^{2}",  70,    0,  350 );
  AddHist( "HadTstarMass",  "Hadronic t* mass (truth reconstuction)",   "GeV/c^{2}", 120,    0, 3000 );
  AddHist( "LepTopMass",    "Leptonic top mass (truth reconstuction)",  "GeV/c^{2}",  70,    0,  350 );
  AddHist( "LepTstarMass",  "Leptonic t* mass (truth reconstuction)",   "GeV/c^{2}", 120,    0, 3000 );
  AddHist( "TstarMassDiff", "t* mass difference (truth reconstuction)", "GeV/c^{2}",  60, -600,  600 );
  AddHist( "TopMassDiff",   "Top mass (truth reconstuction)",           "GeV/c^{2}",  60,  -30,   30 );

  FillFromFile();
}

void
MCResMgr::FillFromFile()
{
  const string filename = ResultsDir() / ( "mcres_M"+Name()+".root" );
  TFile* file           = TFile::Open( filename.c_str() );
  TTree* tree           = (TTree*)( file->Get( "mcres/MCResolution" ) );

  double lepwmass;
  double leptopmass;
  double leptstarmass;
  double hadwmass;
  double hadtopmass;
  double hadtstarmass;

  tree->SetBranchAddress( "LepWMass",     &lepwmass );
  tree->SetBranchAddress( "LepTopMass",   &leptopmass );
  tree->SetBranchAddress( "LepTstarMass", &leptstarmass );
  tree->SetBranchAddress( "HadWMass",     &hadwmass );
  tree->SetBranchAddress( "HadTopMass",   &hadtopmass );
  tree->SetBranchAddress( "HadTstarMass", &hadtstarmass );


  boost::format entryfmt( "\r%s Entry[%d/%d]" );

  for( int i = 0; i < tree->GetEntries(); ++i ){
    tree->GetEntry( i );

    cout << entryfmt
      % Name()
      % i % tree->GetEntries()
         << flush;


    Hist( "LepTopMass" )->Fill( leptopmass );
    Hist( "LepTstarMass" )->Fill( leptstarmass );
    Hist( "HadWMass" )->Fill( hadwmass );
    Hist( "HadTopMass" )->Fill( hadtopmass );
    Hist( "HadTstarMass" )->Fill( hadtstarmass );
    Hist( "TstarMassDiff" )->Fill( hadtstarmass - leptstarmass );
    Hist( "TopMassDiff" )->Fill( hadtopmass - leptopmass );
  }

  cout << "Done!" << endl;
  cout << " lept:" << Hist( "LepTopMass" )->GetRMS() << "\t"
       << " lepts:" << Hist( "LepTstarMass" )->GetRMS() <<  "\t"
       << " hadw:" << Hist( "HadWMass" )->GetRMS() << "\t"
       << " hadt:" << Hist( "HadTopMass" )->GetRMS() << "\t"
       << " hadts:" << Hist( "HadTstarMass" )->GetRMS() << "\t"
       << " dts:" << Hist( "TstarMassDiff" )->GetRMS() << endl;
}

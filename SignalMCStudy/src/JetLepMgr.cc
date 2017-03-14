/*******************************************************************************
*
*  Filename    : JetLepMgr.cc
*  Description : Definition of histograms and how they are filled.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/SignalMCStudy/interface/Common.hpp"
#include "TstarAnalysis/SignalMCStudy/interface/JetLepMgr.hpp"

#include <boost/format.hpp>
#include <iostream>

#include "TFile.h"
#include "TTree.h"

using namespace std;


/******************************************************************************/

JetLepMgr::JetLepMgr( const std::string& name ) :
  Named( name ),
  HistMgr( name )
{
  AddHist( "NumLep",        "Number of good leptons",                    "",  5, 0,   5 );
  AddHist( "JetLepSep",     "Jet-lepton angular seperation",             "", 50, 0, 0.5 );
  AddHist( "JetLepSep1Lep", "Jet-lepton angular seperation (=1 lepton)", "", 50, 0, 0.5 );
  FillFromFile();
}


/******************************************************************************/

JetLepMgr::~JetLepMgr()
{

}


/******************************************************************************/

void
JetLepMgr::FillFromFile()
{
  const string filename = ResultsDir() / ( "jetlep_M"+Name()+".root" );
  TFile* file           = TFile::Open( filename.c_str() );
  TTree* tree           = (TTree*)( file->Get( "jetlepsep/JetLepTest" ) );

  unsigned numlep;
  unsigned numjetlep;
  double jetlepsep[64];

  tree->SetBranchAddress( "NLepton",   &numlep );
  tree->SetBranchAddress( "NJetLep",   &numjetlep );
  tree->SetBranchAddress( "JetLepSep", jetlepsep );

  boost::format entryfmt( "\r%s Entry[%d/%d]" );

  for( int i = 0; i< tree->GetEntries(); ++i ){
    tree->GetEntry( i );

    cout << entryfmt
      % Name()
      % i % tree->GetEntries()
      << flush;


    Hist( "NumLep" )->Fill( numlep );

    for( unsigned j = 0; j < numjetlep; ++j ){
      Hist( "JetLepSep" )->Fill( jetlepsep[j] );
      if( numlep == 1 ){
        Hist( "JetLepSep1Lep" )->Fill( jetlepsep[j] );
      }
    }
  }
  cout << "Done!" << endl;
}

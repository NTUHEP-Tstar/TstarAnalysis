/*******************************************************************************
*
*  Filename    : PlotLimit_DisableNuisance.cc
*  Description : Functions for disabling nuisable parameter
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/DisableNuisance.hpp"

#include <fstream>
#include <iostream>

using namespace std;


extern const vector<string> unclist = {
  "jec",
  "jetres",
  "btag",
  "pu",
  "lep",
  "pdf",
  "scale",
  "model",
  "stat",
  "lumi"
};


/******************************************************************************/

extern void
MakeNewCard( const std::string& masspoint, const std::string nuisance )
{
  const string origfilename = limnamer.TextFileName( "card", masspoint );
  const string newfilename  = limnamer.TextFileName( "card", masspoint, nuisance );
  ifstream originalfile( origfilename );
  ofstream newfile( newfilename );
  string originalline;

  cout << "Output to " << newfilename << endl;

  while( getline( originalfile, originalline ) ){
    if( originalline.find( nuisance ) != string::npos ){
      cout << "# " << originalline << endl;
    } else {
      newfile << originalline << endl;
    }
  }

  originalfile.close();
  newfile.close();
}

/******************************************************************************/

extern void
MakeLimitTable( const std::map<std::string, std::pair<mgr::Parameter, mgr::Parameter> >& )
{

}

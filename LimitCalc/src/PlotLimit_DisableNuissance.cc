/*******************************************************************************
*
*  Filename    : PlotLimit_DisableNuisance.cc
*  Description : Functions for disabling nuisable parameter
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/Parameter/Format.hpp"
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/DisableNuisance.hpp"

#include <boost/format.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;


extern const vector<string> unclist = {
  "",
  // "bg", //disabling background fitting errors for now
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
    istringstream iss( originalline );
    string firstword;
    iss >> firstword;// Getting first word only
    if( firstword.find( nuisance ) != string::npos ){
      cout << "# Disabled line: " << originalline << endl;
    } else if( firstword == "kmax" ){
      newfile << "kmax *" << endl; // Saving kmax 
    } else {
      newfile << originalline << endl;
    }
  }

  originalfile.close();
  newfile.close();
}

/******************************************************************************/

extern void
MakeLimitTable( const std::map<std::string, std::pair<mgr::Parameter, mgr::Parameter> >& limitresults )
{
  const string filename = limnamer.TexFileName( "disable_nuisance" );
  ofstream limiteffect( filename  );

  boost::format tableentry( "%15s & %30s & %30s\\\\" );

  limiteffect << "\\begin{tabular}{|l|cc|}" << endl;
  limiteffect << "\\hline" << endl;
  limiteffect << tableentry % "" % "Expected Limit" % "Observed Limit" << endl;
  limiteffect << "\\hline" << endl;

  const mgr::Parameter explimit = limitresults.at( "" ).first;
  const mgr::Parameter obslimit = limitresults.at( "" ).second;

  for( const auto& resultpair : limitresults ){
    const auto& disabled  = resultpair.first;
    const auto& limitpair = resultpair.second;

    if( disabled == "" ){
      const string obslimstring = ( limnamer.CheckInput( "drawdata" ) ) ? mgr::FloatingPoint( obslimit, 1 ) : "--";
      limiteffect << tableentry
        % "Central value"
        % FloatingPoint( explimit, 1 )
        % obslimstring
                  << endl;
      limiteffect << "\\hline" << endl;
    } else {
      const double explimdiff = fabs( explimit.CentralValue() - limitpair.first.CentralValue() );
      const double obslimdiff = fabs( obslimit.CentralValue() - limitpair.second.CentralValue() );

      const string explimstr = mgr::FloatingPoint( explimdiff, 1 );
      const string obslimstr = ( limnamer.CheckInput( "drawdata" ) ) ? mgr::FloatingPoint( obslimdiff, 1 ) : "--";
      limiteffect << tableentry
        % disabled
        % explimstr
        % obslimstr
                  << endl;
    }
  }

  limiteffect << "\\hline" << endl;
  limiteffect << "\\end{tabular}" << endl;
  limiteffect.close();

  cout << "Writting results to tex file: " << filename << endl;
}

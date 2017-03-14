/*******************************************************************************
*
*  Filename    : Common.cc
*  Description : Implementation of common functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/SignalMCStudy/interface/Common.hpp"

using namespace std;

std::string ResultsDir()
{
  return mgr::SubPackagePath( "TstarAnalysis", "SignalMCStudy" ) / "results" ;
}

string PlotFileName( const string& filename )
{
  return ResultsDir() / filename + ".pdf";
}

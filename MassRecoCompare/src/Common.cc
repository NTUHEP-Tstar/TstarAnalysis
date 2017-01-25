/*******************************************************************************
*
*  Filename    : Common.cc
*  Description : Declaration of common global variables and functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*   Additional comments
*
*******************************************************************************/
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"
#include <boost/algorithm/string.hpp>

using namespace std;
/*******************************************************************************
*   Declaring global objects
*******************************************************************************/
TstarNamer reconamer( "MassRecoCompare" );

const Color_t Color_Sequence[5] = {
  kBlue,
  kRed,
  kGreen,
  kBlack,
  kCyan
};

/*******************************************************************************
*   Module naming functions
*******************************************************************************/
std::string
ProcessName(){ return "HitFitCompare"; }

/******************************************************************************/

std::string
LabelName( const std::string& modulename )
{
  return boost::starts_with( modulename, "ChiSq" ) ?
         "ChiSquareResult" : "HitFitResult";
}

/*******************************************************************************
*   Common functions
*******************************************************************************/
const vector<tstar::Particle_Label> fitlabellist = {
  tstar::lepb_label,
  tstar::lepg_label,
  tstar::hadw1_label,
  tstar::hadw2_label,
  tstar::hadb_label,
  tstar::hadg_label
};

const vector<tstar::Particle_Label> truthlabellist = {
  tstar::lepb_label,
  tstar::lepg_label,
  tstar::hadw1_label,
  tstar::hadw2_label,
  tstar::hadb_label,
  tstar::hadg_label,
  tstar::unknown_label
};

/******************************************************************************/

unsigned
NumCorrectAssign( const RecoResult& results )
{
  unsigned ans = 0;

  for( const auto x : fitlabellist ){
    const auto y = results.GetParticle( x ).TypeFromTruth();

    if( x == y ){ ans++; continue; }
    if( x == tstar::hadw1_label && y == tstar::hadw2_label ){ ans++; continue; }
    if( x == tstar::hadw2_label && y == tstar::hadw1_label ){ ans++; continue; }
  }

  return ans;
}

/*******************************************************************************
 *
 *  Filename    : PileUpWeighter.cc
 *  Description : Implementations for the pileupweighter class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/EventWeight/interface/PileUpWeighter.hpp"
#include <cstdio>
#include <cstdlib>
using namespace std;

PileUpWeighter::PileUpWeighter( const string& filename )
{
   double weight = 0;
   FILE* pufile = fopen( filename.c_str() , "r" );
   while( fscanf(pufile,"%lf", &weight ) != EOF ){
      _pileupweights.push_back( weight );
   }
}

PileUpWeighter::~PileUpWeighter()
{
}

double PileUpWeighter::GetWeight( const unsigned pileup ) const
{
   if( pileup > _pileupweights.size() ){ return 0 ; }
   else { return _pileupweights[pileup]; }
}

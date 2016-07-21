/*******************************************************************************
 *
 *  Filename    : BTagChecker.cc
 *  Description : Implementation of BTag Checker
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/BaseLineSelector/interface/BTagChecker.hpp"

//------------------------------------------------------------------------------
//   Constructor
//------------------------------------------------------------------------------
BTagChecker::BTagChecker( const string& tagger, const string& filename ):
   _calib( tagger, filename ),
{
   for( int i = BTagEntry::OP_LOOSE ; i != BTagEntry::OP_RESHAPE ; ++i ){
      for( const auto& sys : {"central","up","down"} ){
         _reader_map[i][sys] = BTagCalibrationReader( i , sys );
         _reader_map[i][sys].load(_calib, BTagEntry::FLAV_B, "comb");
      }
   }
}

BTagChecker::~BTagChecker()
{
   
}

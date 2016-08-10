/*******************************************************************************
 *
 *  Filename    : BTagChecker.cc
 *  Description : Implementation of BTag Checker
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/Common/interface/BTagChecker.hpp"

using namespace std;
//------------------------------------------------------------------------------
//   Constructor
//------------------------------------------------------------------------------
BTagChecker::BTagChecker( const string& tagger, const string& filename ):
   _calib( tagger, filename )
{
   for( int i = BTagEntry::OP_LOOSE ; i != BTagEntry::OP_RESHAPING ; ++i ){
      _reader_map[BTagEntry::OperatingPoint(i)] = BTagCalibrationReader(
         BTagEntry::OP_LOOSE,  // operating point
         "central",            // central sys type
         {"up", "down"}
      );      // other sys types
      _reader_map[BTagEntry::OperatingPoint(i)].load(
         _calib,
         BTagEntry::FLAV_B,
         "comb"
      );
   }
}

BTagChecker::~BTagChecker()
{

}

//------------------------------------------------------------------------------
//   Main functions
//------------------------------------------------------------------------------
bool BTagChecker::PassLoose( const pat::Jet& jet , bool IsData ) const
{
   return jet.bDiscriminator(DISCRIMTAG) > LOOSEWP_VAL ;
}

bool BTagChecker::PassMedium( const pat::Jet& jet, bool IsData ) const
{
   return jet.bDiscriminator(DISCRIMTAG) > MEDIUMWP_VAL ;
}

bool BTagChecker::PassTight( const pat::Jet& jet, bool IsData ) const
{
   return jet.bDiscriminator(DISCRIMTAG) > TIGHTWP_VAL ;
}


//------------------------------------------------------------------------------
//   Scale factor functions
//------------------------------------------------------------------------------
double BTagChecker::GetLooseScaleFactor( const pat::Jet& jet ) const
{ return scalefactor(jet,BTagEntry::OP_LOOSE,"central"); }
double BTagChecker::GetLooseScaleFactorUp( const pat::Jet& jet ) const
{ return scalefactor(jet,BTagEntry::OP_LOOSE,"up"); }
double BTagChecker::GetLooseScaleFactorDown( const pat::Jet& jet  ) const
{ return scalefactor(jet,BTagEntry::OP_LOOSE,"down"); }

double BTagChecker::GetMediumScaleFactor( const pat::Jet& jet ) const
{ return scalefactor(jet,BTagEntry::OP_MEDIUM,"central"); }
double BTagChecker::GetMediumScaleFactorUp( const pat::Jet& jet ) const
{ return scalefactor(jet,BTagEntry::OP_MEDIUM,"up"); }
double BTagChecker::GetMediumScaleFactorDown( const pat::Jet& jet ) const
{ return scalefactor(jet,BTagEntry::OP_MEDIUM,"down"); }

double BTagChecker::GetTightScaleFactor( const pat::Jet& jet ) const
{ return scalefactor(jet,BTagEntry::OP_TIGHT,"central"); }
double BTagChecker::GetTightScaleFactorUp( const pat::Jet& jet ) const
{ return scalefactor(jet,BTagEntry::OP_TIGHT,"up"); }
double BTagChecker::GetTightScaleFactorDown( const pat::Jet& jet ) const
{ return scalefactor(jet,BTagEntry::OP_TIGHT,"down"); }

//------------------------------------------------------------------------------
//   General helper functions
//------------------------------------------------------------------------------
double BTagChecker::scalefactor(
   const pat::Jet& jet ,
   BTagEntry::OperatingPoint op,
   const std::string& sys
) const
{
   return _reader_map.at(op).eval_auto_bounds(
      sys,
      BTagEntry::FLAV_B,
      jet.eta(),
      jet.pt()
   );
}

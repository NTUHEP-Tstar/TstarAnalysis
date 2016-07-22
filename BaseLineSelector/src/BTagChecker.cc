/*******************************************************************************
 *
 *  Filename    : BTagChecker.cc
 *  Description : Implementation of BTag Checker
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/BaseLineSelector/interface/BTagChecker.hpp"

using namespace std;
//------------------------------------------------------------------------------
//   Constructor
//------------------------------------------------------------------------------
BTagChecker::BTagChecker( const string& tagger, const string& filename ):
   _calib( tagger, filename )
{
   for( int i = BTagEntry::OP_LOOSE ; i != BTagEntry::OP_RESHAPING ; ++i ){
      for( const auto& sys : {"central","up","down"} ){
         _reader_map[BTagEntry::OperatingPoint(i)][sys] = BTagCalibrationReader( BTagEntry::OperatingPoint(i) , sys );
         _reader_map[BTagEntry::OperatingPoint(i)][sys].load(_calib, BTagEntry::FLAV_B, "comb");
      }
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
   double scalefactor = 1 ;
   if( ! IsData ) scalefactor = GetLooseScaleFactor( jet );
   return jet.bDiscriminator(DISCRIMTAG)*scalefactor > LOOSEWP_VAL ;
}

bool BTagChecker::PassMedium( const pat::Jet& jet, bool IsData ) const
{
   double scalefactor = 1 ;
   if( ! IsData ) scalefactor = GetMediumScaleFactor( jet );
   return jet.bDiscriminator(DISCRIMTAG)*scalefactor > MEDIUMWP_VAL ;
}

bool BTagChecker::PassTight( const pat::Jet& jet, bool IsData ) const
{
   double scalefactor = 1 ;
   if( ! IsData ) scalefactor = GetTightScaleFactor( jet );
   return jet.bDiscriminator(DISCRIMTAG)*scalefactor > TIGHTWP_VAL ;
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
   return _reader_map.at(op).at(sys).eval_auto_bounds(
          sys,
          BTagEntry::FLAV_B,
          jet.eta(),
          jet.pt()
   );
}

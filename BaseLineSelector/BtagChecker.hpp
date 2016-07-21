/*******************************************************************************
 *
 *  Filename    : BTagChecker.hpp
 *  Description : Class for checking btag
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  In complience with recommendations page:
 *     https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation76X
 *  Specialized for pfCombinedInclusiveSecondaryVertexV2BJetTags
*******************************************************************************/
#ifndef TSTARANALYSIS_BASELINESELECTOR
#define TSTARANALYSIS_BASELINESELECTOR

#include "CondFormats/BTauObjects/interface/BTagCalibration.h"
#include "CondTools/BTau/interface/BTagCalibrationReader.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

class BTagChecker
{
public:
   static const double LOOSEWP_VAL   = 0.460;
   static const double MEDIUMWP_VAL  = 0.800;
   static const double TIGHTWP_VAL   = 0.935;

   BTagChecker( const std::string& filename );
   virtual ~BTagChecker ();

   bool PassLoose ( const pat::Jet& , bool IsData ) const ;
   bool PassMedium( const pat::Jet& , bool IsData ) const ;
   bool PassMedium( const pat::Jet& , bool IsData ) const ;

   double GetLooseScaleFactor    ( const pat::Jet& ) const ;
   double GetLooseScaleFactorUp  ( const pat::Jet& ) const ;
   double GetLooseScaleFactorDown( const pat::Jet& ) const ;

   double GetMediumScaleFactor    ( const pat::Jet& ) const ;
   double GetMediumScaleFactorUp  ( const pat::Jet& ) const ;
   double GetMediumScaleFactorDown( const pat::Jet& ) const ;

   double GetTightScaleFactor    ( const pat::Jet& ) const ;
   double GetTightScaleFactorUp  ( const pat::Jet& ) const ;
   double GetTightScaleFactorDown( const pat::Jet& ) const ;

private:
   BTagCalibration _calib;
   BTagCalibrationReader _loose_reader;
   BTagCalibrationReader _loose_up_reader;
   BTagCalibrationReader _loose_down_reader;
   BTagCalibrationReader _medium_reader;
   BTagCalibrationReader _medium_up_reader;
   BTagCalibrationReader _medium_down_reader;
   BTagCalibrationReader _tight_reader;
   BTagCalibrationReader _tight_up_reader;
   BTagCalibrationReader _tight_down_reader;
};

#endif /* end of include guard: TSTARANALYSIS_BASELINESELECTOR */

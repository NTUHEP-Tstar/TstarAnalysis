/*******************************************************************************
*
*  Filename    : SampleRooFitMgr.hh
*  Description : RooFit object per SampleGroup management process
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_LIMITCALC_SAMPLEROOFITMGR_HPP
#define TSTARANALYSIS_LIMITCALC_SAMPLEROOFITMGR_HPP

#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "ManagerUtils/RootMgr/interface/RooFitMgr.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include <string>
#include <vector>

class SampleRooFitMgr :
   public mgr::SampleGroup,
   public mgr::RooFitMgr
{
public:
   SampleRooFitMgr( const std::string&, const mgr::ConfigReader& );
   ~SampleRooFitMgr();

   // Specialization for analysis variables
   static RooRealVar& x();
   static double      MinMass();
   static double      MaxMass();
   static RooRealVar& w();

   static void InitStaticVars( const double, const double );

   // Speicalization for creating new datasets
   RooDataSet* NewDataSet( const std::string& );

   // Specialized function for called default dataset
   RooDataSet* DataSet( const std::string& name = "" );

   // Speicalized functions for inserting datapoint (defined in fill set)
   void AddToDataSet( const std::string& name , const double mass, const double weight );

   // Specialized function for creating predefined pdf functions
   RooAbsPdf* NewPdf( const std::string& name, const std::string& type );

private:
   // Filling functions specific to this analysis, see SampleRooFitMgr_FillSet.cc
   void definesets();
   void fillsets( mgr::SampleMgr& );
};

#endif/* end of include guard: __SAMPLE_ROOFIT_MGR_HH__ */

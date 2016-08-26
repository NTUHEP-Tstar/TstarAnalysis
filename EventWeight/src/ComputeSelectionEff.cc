/*******************************************************************************
 *
 *  Filename    : ComputeSelectionEff.cc
 *  Description : Implementing a new computation of selection efficiencies
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "ManagerUtils/Maths/interface/Efficiency.hpp"

#include "DataFormats/FWLite/interface/Run.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/Common/interface/MergeableCounter.h"
#include "TFile.h"

#include <exception>

using mgr::SampleMgr;

double GetOriginalEventCount( const SampleMgr& sample )
{
   fwlite::Handle<edm::MergeableCounter> positive_count;
   fwlite::Handle<edm::MergeableCounter> negative_count;
   double count = 0 ;

   for( const auto& file_path : sample.GlobbedFileList() ) {
      fwlite::Run run( TFile::Open(file_path.c_str()) );
      for( run.toBegin() ; !run.atEnd() ; ++run ){
         positive_count.getByLabel( run , "beforeAny" , "positiveEvents" );
         negative_count.getByLabel( run , "beforeAny" , "negativeEvents" );
         count += positive_count->value;
         count -= negative_count->value;
      }
   }
   return count;
}

double GetSelectedEventCount( const SampleMgr& sample )
{
   fwlite::Handle<double> count_handle ;
   double ans = 0 ;

   try {
      for( const auto& file_path : sample.GlobbedFileList() ){
         fwlite::Run run( TFile::Open(file_path.c_str()) );
         for( run.toBegin() ; !run.atEnd() ; ++run ){
            count_handle.getByLabel( run , "EventWeight" , "WeightSum" );
            ans += *count_handle;
         }
      }
   } catch( std::exception ) {
      ans = sample.Event().size(); // For data files
   }
   return ans;
}

const Parameter& ComputeSelectionEff( SampleMgr& sample )
{
   const double passed = GetSelectedEventCount( sample );
   const double total  = GetOriginalEventCount( sample );

   sample.SetSelectionEfficiency( Efficiency(passed,total) );
   return sample.SelectionEfficiency();
}

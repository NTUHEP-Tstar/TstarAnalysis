/*******************************************************************************
*
*  Filename    : ComputeSelectionEff.cc
*  Description : Implementing a new computation of selection efficiencies
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/Efficiency.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"

#include "DataFormats/Common/interface/MergeableCounter.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/FWLite/interface/Run.h"
#include "TFile.h"

#include <exception>

using mgr::SampleMgr;

double
GetOriginalEventCount( const SampleMgr& sample )
{
   return sample.GetCacheDouble( "OriginalEventCount" );
}

double
GetSelectedEventCount( const SampleMgr& sample )
{
   return sample.GetCacheDouble( "SelectedEventCount" );
}

double
SetOriginalEventCount( SampleMgr& sample )
{
   fwlite::Handle<edm::MergeableCounter> positive_count;
   fwlite::Handle<edm::MergeableCounter> negative_count;
   double count = 0;

   for( const auto& file_path : sample.GlobbedFileList() ){
      fwlite::Run run( TFile::Open( file_path.c_str() ) );

      for( run.toBegin(); !run.atEnd(); ++run ){
         positive_count.getByLabel( run, "beforeAny", "positiveEvents" );
         negative_count.getByLabel( run, "beforeAny", "negativeEvents" );
         count += positive_count->value;
         count -= negative_count->value;
      }
   }

   sample.AddCacheDouble( "OriginalEventCount", count );
   return count;
}

double
SetSelectedEventCount( SampleMgr& sample )
{
   fwlite::Handle<double> count_handle;
   double ans = 0;

   for( const auto& file : sample.GlobbedFileList() ){
      fwlite::Event ev( TFile::Open( file.c_str() ) );

      for( ev.toBegin(); !ev.atEnd(); ++ev ){
         ans += GetEventWeight( ev ) * GetSampleEventTopPtWeight( sample, ev );
      }
   }

   sample.AddCacheDouble( "SelectedEventCount", ans );
   return ans;
}

const Parameter&
ComputeSelectionEff( SampleMgr& sample )
{
   if( !sample.HasCacheDouble( "OriginalEventCount" ) ){
      SetOriginalEventCount( sample );
   }
   if( !sample.HasCacheDouble( "SelectedEventCount" ) ){
      SetSelectedEventCount( sample );
   }

   const double passed = GetSelectedEventCount( sample );
   const double total  = GetOriginalEventCount( sample );

   sample.SetSelectionEfficiency( Efficiency( passed, total ) );
   return sample.SelectionEfficiency();
}

double
GetSampleWeight( const SampleMgr& sample )
{
   const double selectedEvents = GetSelectedEventCount( sample );
   const double expyield       = sample.ExpectedYield().CentralValue();
   if( selectedEvents == 0 ){ return 0; } else { return expyield / selectedEvents; }
}

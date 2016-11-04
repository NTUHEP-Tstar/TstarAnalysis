/*******************************************************************************
*
*  Filename    : InitSampleStatic.cc
*  Description : Implementation of initialization functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/EDMUtils/interface/Counter.hpp"
#include "ManagerUtils/SampleMgr/interface/MultiFile.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"

#include "DataFormats/Common/interface/MergeableCounter.h"
#include "DataFormats/FWLite/interface/Handle.h"

#include <boost/algorithm/string.hpp>
#include <iostream>

void
InitSampleStatic( const TstarNamer& namer )
{
   using namespace std;
   const mgr::ConfigReader cfg( namer.MasterConfigFile() );
   mgr::SampleMgr::InitStaticFromReader( cfg );
   if( boost::contains( namer.GetChannel(), "2015" ) ){
      cout << "Using 2015 Luminosity" << endl;
      mgr::SampleMgr::SetTotalLuminosity( cfg.GetStaticDouble( "Total Luminosity 2015" ) );
   } else {
      mgr::SampleMgr::SetTotalLuminosity( cfg.GetStaticDouble( "Total Luminosity 2016" ) );
   }
   mgr::SampleMgr::SetFilePrefix( namer.GetChannelEDMPath() );
   mgr::SampleGroup::SetSampleCfgPrefix( namer.SettingsDir() );
}

/******************************************************************************/

void
InitSampleFromEDM( mgr::SampleMgr& sample )
{
   mgr::MultiFileRun myrun( sample.GlobbedFileList() );

   // Calculation original number of events is the same for MC and Data
   fwlite::Handle<edm::MergeableCounter> positivehandle;
   fwlite::Handle<edm::MergeableCounter> negativehandle;
   double origcount = 0;
   double selccount = 0;
   double topwcount = 0;

   for( myrun.toBegin(); !myrun.atEnd(); ++myrun ){
      const auto& run = myrun.Base();

      positivehandle.getByLabel( run, "beforeAny", "positiveEvents" );
      negativehandle.getByLabel( run, "beforeAny", "negativeEvents" );
      origcount += positivehandle->value;
      origcount -= negativehandle->value;
   }

   // Selected event counting is not the same for data and MC

   if( sample.IsRealData() ){
      // For data, the selected number of events is just the number of events
      mgr::MultiFileEvent myevent( sample.GlobbedFileList() );
      selccount = myevent.size();
      topwcount = myevent.size();
   } else {
      fwlite::Handle<mgr::Counter> evtweighthandle;
      fwlite::Handle<mgr::Counter> evtweightallhandle;

      for( myrun.toBegin(); !myrun.atEnd() ; ++myrun ){
         const auto& run = myrun.Base();

         evtweighthandle.getByLabel( run, "EventWeight","WeightSum");
         evtweightallhandle.getByLabel( run,"EventWeightAll","WeightSum");
         selccount += evtweighthandle.ref().value ;
         topwcount += evtweightallhandle.ref().value ;
      }
   }

   // Adding to samplemgr
   sample.SetOriginalEventCount( origcount );
   sample.SetSelectedEventCount( selccount );
   sample.AddCacheDouble( "TopPtWeightSum", topwcount );
}

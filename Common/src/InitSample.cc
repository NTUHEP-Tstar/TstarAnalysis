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
   mgr::SampleMgr::InitStaticFromReader( namer.MasterConfig() );
   mgr::SampleMgr::SetFilePrefix( namer.GetChannelEDMPath() );
   mgr::SampleGroup::SetSampleCfgPrefix( namer.SettingsDir() );

   // Luminosity settings
   if( namer.HasOption( "era" ) ){
      if( namer.GetChannel().find( "Electron" ) != std::string::npos ){
         mgr::SampleMgr::SetTotalLuminosity( namer.GetExtDouble( "era", "EleLumi" ) );
      } else {
         mgr::SampleMgr::SetTotalLuminosity( namer.GetExtDouble( "era", "Lumi" ) );
      }
   } else {
      std::cerr << "Warning!! Era of data taking not specified! Setting luminosity to 0!" << std::endl;
      mgr::SampleMgr::SetTotalLuminosity( 0 );
   }
}

/******************************************************************************/

void
InitSampleFromEDM( mgr::SampleMgr& sample )
{
   mgr::MultiFileRun myrun( sample.GlobbedFileList() );

   // Calculation original number of events is the same for MC and Data
   fwlite::Handle<mgr::Counter> orighandle;
   fwlite::Handle<mgr::Counter> evtweighthandle;
   fwlite::Handle<mgr::Counter> evtweightallhandle;

   double origcount = 0;
   double selccount = 0;
   double topwcount = 0;

   for( myrun.toBegin(); !myrun.atEnd(); ++myrun ){
      const auto& run = myrun.Base();

      orighandle.getByLabel( run, "BeforeAll", "WeightSum" );
      evtweighthandle.getByLabel( run, "EventWeight", "WeightSum" );
      evtweightallhandle.getByLabel( run, "EventWeightAll", "WeightSum" );

      origcount += orighandle.ref().value;
      selccount += evtweighthandle.ref().value;
      topwcount += evtweightallhandle.ref().value;
   }

   // Adding to samplemgr
   sample.SetOriginalEventCount( origcount );
   sample.SetSelectedEventCount( selccount );
   sample.AddCacheDouble( "TopPtWeightSum", topwcount );
}

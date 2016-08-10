/*******************************************************************************
 *
 *  Filename    : InitSampleStatic.cc
 *  Description : Implementation of initialization functions
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"

#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include <boost/algorithm/string.hpp>
#include <iostream>

void InitSampleStatic( const TstarNamer& namer )
{
   using namespace std;
   const mgr::ConfigReader cfg(namer.MasterConfigFile() );
   mgr::SampleMgr::InitStaticFromReader( cfg );
   if( boost::contains( namer.GetChannel(), "2015" ) ){
      cout << "Using 2015 Luminosity" << endl;
      mgr::SampleMgr::SetTotalLuminosity( cfg.GetStaticDouble("Total Luminosity 2015") );
   } else {
      mgr::SampleMgr::SetTotalLuminosity( cfg.GetStaticDouble("Total Luminosity 2016") ) ;
   }
   mgr::SampleMgr::SetFilePrefix     ( namer.GetChannelEDMPath() );
   mgr::SampleGroup::SetSampleCfgPrefix( namer.SettingsDir() );
}

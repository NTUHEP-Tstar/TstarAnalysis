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

void InitSampleStatic( const TstarNamer& namer )
{
   mgr::SampleMgr::InitStaticFromFile( namer.MasterConfigFile() );
   mgr::SampleMgr::SetFilePrefix     ( namer.GetChannelEDMPath() );
   mgr::SampleGroup::SetSampleCfgPrefix( namer.SettingsDir() );
}

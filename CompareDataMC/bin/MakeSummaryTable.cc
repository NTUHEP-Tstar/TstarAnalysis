/*******************************************************************************
 *
 *  Filename    : MakeSummaryTable.cc
 *  Description : Generate Latex Summary table
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;
using mgr::SampleMgr;
using mgr::SampleGroup;
using mgr::ConfigReader;

typedef vector<SampleGroup*> GroupList;
//------------------------------------------------------------------------------
//   External functions, see src/SummaryTable.cc
//------------------------------------------------------------------------------
extern void SummaryComplete(
   const GroupList& sig_list,
   const GroupList& bkg_list,
   const SampleGroup* data
);

extern void SummarySignal(
   const GroupList& sig_list
);

extern void SummaryBKGBrief(
   const GroupList& bkg_list,
   const SampleGroup* data
);

extern void SummaryMCLumi(
   const GroupList& sig_list,
   const GroupList& bkg_list
);

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
   if( argc != 2 ){
      cerr << "Error! Expected exactly one arguement!" << endl;
      return 1;
   }
   cout << "Making summary table!" << endl;
   SetChannelType( argv[1] );

   SampleMgr::InitStaticFromReader( StaticCfg() );
   SampleMgr::SetFilePrefix( GetEDMPrefix() );

   vector<SampleGroup*> signal_mc_list;
   for( const auto& sig_tag : StaticCfg().GetStaticStringList("Signal List") ){
      signal_mc_list.push_back( new mgr::SampleGroup(sig_tag , StaticCfg()) );
   }

   vector<SampleGroup*> bkg_mc_list;
   for( const auto& bkg_tag : StaticCfg().GetStaticStringList("Background List") ){
      bkg_mc_list.push_back( new mgr::SampleGroup(bkg_tag,StaticCfg() ) );
   }

   SampleGroup* data = new mgr::SampleGroup( GetDataTag(), StaticCfg() );

   SummaryComplete(signal_mc_list,bkg_mc_list,data);
   SummarySignal(signal_mc_list);
   SummaryBKGBrief(bkg_mc_list,data);
   SummaryMCLumi(signal_mc_list,bkg_mc_list);

   return 0;
}

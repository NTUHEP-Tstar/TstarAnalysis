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
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"
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
   cout << "Making summary table!" << endl;
   const vector<string> manditory = {"channel"};
   const int run = InitOptions( argc, argv );
   if( run == PARSE_HELP  ){ ShowManditory( manditory ); return 0; }
   if( run == PARSE_ERROR ){ return 1; }
   if( CheckManditory( manditory ) != PARSE_SUCESS) { return 1; }

   InitSampleSettings( compare_namer );

   const ConfigReader& master = compare_namer.MasterConfig();

   vector<SampleGroup*> signal_mc_list;
   for( const auto& sig_tag : master.GetStaticStringList("Signal List") ){
      signal_mc_list.push_back( new mgr::SampleGroup(sig_tag , master) );
   }

   vector<SampleGroup*> bkg_mc_list;
   for( const auto& bkg_tag : master.GetStaticStringList("Background List") ){
      bkg_mc_list.push_back( new mgr::SampleGroup(bkg_tag,master ) );
   }

   SampleGroup* data = new mgr::SampleGroup( compare_namer.GetChannelDataTag(),master );

   cout << "Making complete summary table...." << endl;
   SummaryComplete(signal_mc_list,bkg_mc_list,data);
   cout << "Making signal summary table...." << endl;
   SummarySignal(signal_mc_list);
   cout << "Making simple summary table...." << endl;
   SummaryBKGBrief(bkg_mc_list,data);
   cout << "Making lumi summary table...." << endl;
   SummaryMCLumi(signal_mc_list,bkg_mc_list);

   return 0;
}

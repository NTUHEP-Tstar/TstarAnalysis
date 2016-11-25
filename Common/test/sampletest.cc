/*******************************************************************************
*
*  Filename    : sampletest.cc
*  Description : Testing the initializing and caching functions for mgr::SampleMgr
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include <iostream>

using namespace std;

int
main( int argc, char const* argv[] )
{
   mgr::SampleMgr test( "test" );

   test.SetFileList( {"test.root"} );

   InitSampleFromEDM( test );

   cout << test.OriginalEventCount()  << endl;

   cout << test.SelectedEventCount() << endl;

   cout << test.GetCacheDouble( "TopPtWeightSum" ) << endl;

   return 0;
}

/*******************************************************************************
*
*  Filename    : Common.cc
*  Description : Declaring  and implementaing common functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"
#include <string>

using namespace std;
/*******************************************************************************
*   Declaring external objects
*******************************************************************************/
TstarNamer compnamer( "CompareDataMC" );

/*******************************************************************************
*   Common naming convention
*******************************************************************************/
string
SampleCacheFile( const mgr::SampleMgr& x )
{
   return compnamer.CustomFileName( "txt", {x.Name()} );
}

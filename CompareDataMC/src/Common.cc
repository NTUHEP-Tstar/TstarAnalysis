/*******************************************************************************
*
*  Filename    : Common.cc
*  Description : Declaring  and implementaing common functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"

#include <boost/program_options.hpp>
#include <string>

using namespace std;
namespace opt = boost::program_options;

/*******************************************************************************
*   Declaring external objects
*******************************************************************************/
TstarNamer compnamer( "CompareDataMC" );

/*******************************************************************************
*   Common options for all binaries
*******************************************************************************/
boost::program_options::options_description
CompareOptions()
{
  boost::program_options::options_description ans( "Common options for CompareDataMC" );
  ans.add_options()
    ( "channel,c", opt::value<string>()->required(),              "What channel to run"       )
    ( "era,e",     opt::value<string>()->default_value( "Rereco" ), "which era to normalize to" )
  ;
  return ans;
}

/*******************************************************************************
*   Common naming convention
*******************************************************************************/
string
SampleCacheFile( const mgr::SampleMgr& x )
{
  return compnamer.CustomFileName( "txt", {x.Name()} );
}

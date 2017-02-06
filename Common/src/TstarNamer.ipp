/*******************************************************************************
*
*  Filename    : TstarNamer.ipp
*  Description : Implementation for TstarNamer variadic functions
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMMON_TSTARNAMER_IPP
#define TSTARANALYSIS_COMMON_TSTARNAMER_IPP

#include "ManagerUtils/Common/interface/Variadic.hpp"
#include "TstarAnalysis/Common/interface/TstarNamer.hpp"

/******************************************************************************/

template<typename ... TS>
void
TstarNamer::SetNamingOptions( const std::string& first, TS ... args )
{
  SetNamingOptions( mgr::MakeVector<std::string>( first, args ... ) );
}

/******************************************************************************/

template<typename ... TS>
std::string
TstarNamer::CustomFileName( const std::string& ext, const std::string& first,  TS ... args ) const
{
  return CustomFileName( ext, mgr::MakeVector<std::string>( first, args ... ) );
}

/******************************************************************************/

template<typename ... TS>
std::string
TstarNamer::OptFileName( const std::string& ext, const std::string& maintag, const std::string& subfirst, TS ... args ) const
{
  return OptFileName( ext, maintag, mgr::MakeVector<std::string>( subfirst, args ... ) );
}

/******************************************************************************/

template<typename ... TS>
std::string
TstarNamer::TextFileName( const std::string& main, const std::string& subfirst, TS ... args ) const
{
  return TextFileName( main, mgr::MakeVector<std::string>( subfirst, args ... ) );
}

/******************************************************************************/

template<typename ... TS>
std::string
TstarNamer::PlotFileName( const std::string& main, const std::string& subfirst, TS ... args ) const
{
  return PlotFileName( main, mgr::MakeVector<std::string>( subfirst, args ... ) );
}

/******************************************************************************/

template<typename ... TS>
std::string
TstarNamer::TexFileName( const std::string& main, const std::string& subfirst, TS ... args ) const
{
  return TexFileName( main, mgr::MakeVector<std::string>( subfirst, args ... ) );
}

/******************************************************************************/

template<typename ... TS>
std::string
TstarNamer::RootFileName( const std::string& main, const std::string& subfirst, TS ... args ) const
{
  return RootFileName( main, mgr::MakeVector<std::string>( subfirst, args ... ) );
}

#endif/* end of include guard: TSTARANALYSIS_COMMON_TSTARNAMER_IPP */

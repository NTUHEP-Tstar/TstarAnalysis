/*******************************************************************************
*
*  Filename    : IDCache.hpp
*  Description : Using BITWISE operation to cache ID variables, flag definition
*                and wrapper functions placed here
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMMON_IDCACHE_HPP
#define TSTARANALYSIS_COMMON_IDCACHE_HPP

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

/*******************************************************************************
*   Electron ID part, using inline function for clarity
*******************************************************************************/
#define ELEIDVAR    "ELEID"
#define ELEVETOFLAG    ( 0x1<<0 )
#define ELELOOSEFLAG   ( 0x1<<1 )
#define ELEMEDIUMFLAG  ( 0x1<<2 )
#define ELETIGHTFLAG   ( 0x1<<3 )
#define ELEHEEPFLAG    ( 0x1<<4 )
#define ELEHLTFLAG     ( 0x1<<5 )

inline bool
PassID( const pat::Electron& el, const int workingpoint )
{
   if( el.hasUserInt( ELEIDVAR ) ){
      return el.userInt( ELEIDVAR ) & workingpoint;
   } else {
      return false;
   }
}

inline bool
PassVetoID( const pat::Electron& el ){ return PassID( el, ELEVETOFLAG ); }

inline bool
PassLooseID( const pat::Electron& el ){ return PassID( el, ELELOOSEFLAG ); }

inline bool
passMediumID( const pat::Electron& el ){ return PassID( el, ELEMEDIUMFLAG ); }

inline bool
PassTightID( const pat::Electron& el ){ return PassID( el, ELETIGHTFLAG ); }

inline bool
PassHEEPID( const pat::Electron& el ){ return PassID( el, ELEHEEPFLAG ); }

inline bool
PassHLTID( const pat::Electron& el ){ return PassID( el, ELEHLTFLAG ); }


/*******************************************************************************
*   Muon ID part, using inline function for clarity
*******************************************************************************/
#define MUIDVAR "MUID"
#define MUSOFTFLAG    ( 0x1<<0 )
#define MULOOSEFLAG   ( 0x1<<1 )
#define MUMEDIUMFLAG  ( 0x1<<2 )
#define MUTIGHTFLAG   ( 0x1<<3 )
#define MUHIGHTPTFLAG ( 0x1<<4 )

inline bool
PassID( const pat::Muon& mu, const int workingpoint )
{
   if( mu.hasUserInt( MUIDVAR ) ){
      return mu.userInt( MUIDVAR )& workingpoint;
   } else {
      return false;
   }
}

inline bool
PassSoftID( const pat::Muon& mu ){ return PassID( mu, MUSOFTFLAG ); }

inline bool
PassLooseID( const pat::Muon& mu ){ return PassID( mu, MULOOSEFLAG ); }

inline bool
passMediumID( const pat::Muon& mu ){ return PassID( mu, MUMEDIUMFLAG ); }

inline bool
PassTightID( const pat::Muon& mu ){ return PassID( mu, MUTIGHTFLAG ); }

inline bool
PassHighPtID( const pat::Muon& mu ){ return PassID( mu, MUHIGHTPTFLAG ); }


#endif/* end of include guard: TSTARANALYSIS_COMMON_IDCACHE_HPP */

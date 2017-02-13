/*******************************************************************************
*
*  Filename    : DisableNuisance.hpp
*  Description : Declaring the functions required for checking impact of the nuisance parameters
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_LIMITCALC_DISABLENUISANCE_HPP
#define TSTARANALYSIS_LIMITCALC_DISABLENUISANCE_HPP

#include <vector>
#include <string>

extern const std::vector<std::string> unclist ;

extern void MakeNewCard( const std::string& masspoint, const std::string nuisance );

extern void MakeLimitTable( const std::map< std::string, std::pair<mgr::Parameter, mgr::Parameter>>& );

#endif /* end of include guard: TSTARANALYSIS_LIMITCALC_DISABLENUISANCE_HPP */

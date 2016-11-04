/*******************************************************************************
*
*  Filename    : Compare_Common.hpp
*  Description : Defining common functions used by al scripts here
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_COMPARECOMMON_HPP
#define TSTARANALYSIS_COMPAREDATAMC_COMPARECOMMON_HPP

#include "TstarAnalysis/Common/interface/TstarNamer.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include <vector>
#include <string>

/*******************************************************************************
*   Common external objects
*******************************************************************************/
extern TstarNamer compnamer;// see src/PlotHist.cc

/*******************************************************************************
*   Common plotting functions
*******************************************************************************/
extern void
MakePlot(
   THStack*                        stack,
   TH1D*                           bkgerror,
   TH1D*                           datahist,
   TH1D*                           sighist,
   TH1D*                           bkgrel,
   TH1D*                           datarel,
   TLegend*                        legend,
   const std::string               filenametag,
   const std::vector<std::string>& taglist
);


#endif/* end of include guard: TSTARANALYSIS_COMPAREDATAMC_COMPARECOMMON_HPP */

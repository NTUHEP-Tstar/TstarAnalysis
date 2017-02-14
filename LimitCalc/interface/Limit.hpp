/*******************************************************************************
*
*  Filename    : Limit.hpp
*  Description : Defining functions for required for limit calculation and plottings
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_LIMITCALC_LIMIT_HPP
#define TSTARANALYSIS_LIMITCALC_LIMIT_HPP

#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include <map>
#include <string>
#include <vector>

/*******************************************************************************
*   Main control flow to be called by main functions
*******************************************************************************/
extern void RunCombine(
  const std::string& additionaltag = "",
  const std::string& higgs_opt = ""
  );// src/RunCombine.cc

// First is the expected limit , second is the observed limit
extern std::pair<mgr::Parameter, mgr::Parameter> MakeLimitPlot( const std::string& additionaltag = ""
                                                                );// src/PlotLimit.cc

/*******************************************************************************
*   Helper functions for limit plotting
*
*   GetXsectionMap - returning a map<double,double> of the theoretical cross section
*
*   MakeTheoryGraph - Making the theoretical cross section TGraph
*
*   MakeCalcGraph - Making the calculated graph, requires the theoretical theory,
*                   the signal tag list,
*                   as well as the central value entry and the up,down error entries
*                   below has some quick definitions
*
*   GetInterSect - return the intersect of a TGraph, and a TGraph with error.
*                  returning the three intersecting points as a Parameters
*
*
*******************************************************************************/

extern std::map<double, double> GetXsectionMap();

extern TGraphAsymmErrors* MakeTheoryGraph( const std::map<double, double>&  );

extern const int explim;
extern const int obslim;
extern const int onesig_up;
extern const int onesig_down;
extern const int twosig_up;
extern const int twosig_down;
extern const int skip;

extern TGraph* MakeCalcGraph(
  const std::map<double, double>& xsec,
  const std::string& additionaltag,
  const int centralentry,
  const int uperrorentry,
  const int downerrorentry
  );


extern mgr::Parameter GetInterSect( const TGraph* graph, const TGraph* errorgraph );


#endif/* end of include guard: TSTARANALYSIS_LIMITCALC_LIMIT_HPP */

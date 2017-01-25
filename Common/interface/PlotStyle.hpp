/*******************************************************************************
*
*  Filename    : PlotStyle.hpp
*  Description : Helper functions for uniform plot styling.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMMON_PLOTSTYLE_HPP
#define TSTARANALYSIS_COMMON_PLOTSTYLE_HPP

#include "TGraph.h"

/*******************************************************************************
*   Plotting options strings
*******************************************************************************/
#define PS_SAME    "SAME"
#define PS_AXIS    "AXIS"
#define PS_DATA    "LPE"
#define PS_HIST    "HIST"
#define PS_ERROR   "E2"
#define PGS_DATA   "PZ"// P for point, Z for no end of error bar tick
#define PGS_SIGNAL "L"

/*******************************************************************************
*   Color settings
*******************************************************************************/
#define KRED    (kRed+2)
#define KGREEN  (kGreen+1)
#define KBLUE   (kBlue+2)
#define KSKY    (kBlue-7)

/*******************************************************************************
*   Fill settings
*******************************************************************************/
#define FILL_NON   0
#define FILL_SOLID 1001
#define FILL_ST    3004
#define FILL_DENSE 3144

/*******************************************************************************
*   Object settings functions
*******************************************************************************/
namespace tstar
{

/*******************************************************************************
*   Template functions for fitting Common styling options, see src/PlotStyle.ipp
*******************************************************************************/
template<class T>
void SetDataStyle( T* x );

template<class T>
void SetErrorStyle( T* x );

template<class T>
void SetSignalStyle( T* x );

/*******************************************************************************
*   RooFit results styling options, see src/PlotStyle.cc
*******************************************************************************/
void SetFitBGStyle( TGraph* );
void SetFitBGErrStyle( TGraph* );
void SetFitCombStyle( TGraph* );

/*******************************************************************************
*   Limit Graph styling, Using TGraph ( see src/PlotStyle.cc for definitions)
*******************************************************************************/
void SetOneSigmaStyle( TGraph* x );
void SetTwoSigmaStyle( TGraph* x );
void SetExpLimitStyle( TGraph* x );
void SetObsLimitStyle( TGraph* x );
void SetTheoryStyle( TGraph* x );


};

#include "TstarAnalysis/Common/src/PlotStyle.ipp"

#endif/* end of include guard: TSTARANALYSIS_COMMON_PLOTSTYLE_HPP */

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
#define PGS_DATA   "PZ" // P for point, Z for no end of error bar tick
#define PGS_SIGNAL "LB"

namespace tstar
{

/*******************************************************************************
*   Template functions for fitting Common styling options
*******************************************************************************/
template<class T>
void
SetDataStyle( T* x )
{
   x->SetMarkerStyle( 20 );
   x->SetMarkerSize( 0.7 );
   x->SetLineColor( kBlack );
}

template<class T>
void
SetErrorStyle( T* x )
{
   x->SetFillColor( kBlack );
   x->SetLineColor( kBlack );
   x->SetFillStyle( 3004 );
}

template<class T>
void
SetSignalStyle( T* x )
{
   x->SetLineColor( kRed );
   x->SetFillColor( kRed );
   x->SetFillStyle( 3005 );
}

/*******************************************************************************
*   Limit Graph styling, Using TGraph ( see src/PlotStyle.cc for definitions)
*******************************************************************************/
void SetOneSigmaStyle( TGraph* x );
void SetTwoSigmaStyle( TGraph* x );
void SetExpLimitStyle( TGraph* x );
void SetObsLimitStyle( TGraph* x );
void SetTheoryStyle( TGraph* x );


};

#endif/* end of include guard: TSTARANALYSIS_COMMON_PLOTSTYLE_HPP */

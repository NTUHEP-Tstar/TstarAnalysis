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

namespace tstar
{

template<class T>
void
SetDataStyle( T* x )
{
   x->SetMarkerStyle( 21 );
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

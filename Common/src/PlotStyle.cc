/*******************************************************************************
*
*  Filename    : PlotStyle.cc
*  Description : Implementations for plotting style
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TGraph.h"
#include "TstarAnalysis/Common/interface/PlotStyle.hpp"

namespace tstar {
/*******************************************************************************
*   RooFit results styling
*******************************************************************************/
void
SetFitBGStyle( TGraph* x )
{
  x->SetFillStyle( 3245 );
  x->SetFillColor( KSKY );
  x->SetLineColor( KBLUE );
  x->SetLineWidth( 1 );
  x->SetLineStyle( 1 );
}

/******************************************************************************/

void
SetFitCombStyle( TGraph* x )
{
  x->SetFillColor( KGREEN );
  x->SetLineColor( KGREEN );
  x->SetLineWidth( 2 );
  x->SetLineStyle( 3 );
}

/*******************************************************************************
*   Limit plots stylings
*******************************************************************************/
void
SetOneSigmaStyle( TGraph* x )
{
  x->SetFillColor( KGREEN );
  x->SetLineColor( KGREEN );
  x->SetLineWidth( 0 );
  x->SetFillStyle( FILL_SOLID );
}

/******************************************************************************/

void
SetTwoSigmaStyle( TGraph* x )
{
  x->SetFillColor( kOrange );
  x->SetLineColor( kOrange );
  x->SetLineWidth( 0 );
  x->SetFillStyle( FILL_DENSE );
}

/******************************************************************************/

void
SetTheoryStyle( TGraph* x )
{
  x->SetLineColor( KBLUE );
  x->SetLineWidth( 2 );
  x->SetLineStyle( 2 );

  x->SetFillColor( KSKY );
  x->SetFillStyle( 3254 );
}

/******************************************************************************/

void
SetExpLimitStyle( TGraph* x )
{
  x->SetLineColor( KRED );
  x->SetLineWidth( 2 );
  x->SetLineStyle( 3 );
}

/******************************************************************************/

void
SetObsLimitStyle( TGraph* x )
{
  x->SetLineColor( kBlack );
  x->SetLineWidth( 2 );
  x->SetLineStyle( 1 );

}

};

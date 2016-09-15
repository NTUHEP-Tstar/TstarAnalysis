/*******************************************************************************
*
*  Filename    : PlotStyle.cc
*  Description : Implementations for plotting style
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TGraph.h"

namespace tstar {

void
SetOneSigmaStyle( TGraph* x )
{
   x->SetFillColor( kGreen +1 );
   x->SetLineColor( kGreen +1 );
   x->SetLineWidth( 0 );
   x->SetFillStyle( 1001 );
}

void
SetTwoSigmaStyle( TGraph* x )
{
   x->SetFillColor( kOrange );
   x->SetLineColor( kOrange );
   x->SetLineWidth( 0 );
   x->SetFillStyle( 3001 );
}

void
SetTheoryStyle( TGraph* x )
{
   x->SetLineColor( kBlue );
   x->SetLineWidth( 2 );
   x->SetLineStyle( 2 );

}

void
SetExpLimitStyle( TGraph* x )
{
   x->SetLineColor( kBlack );
   x->SetLineWidth( 2 );
   x->SetLineStyle( 2 );
}

void
SetObsLimitStyle( TGraph* x )
{
   x->SetLineColor( kBlack );
   x->SetLineWidth( 2 );
   x->SetLineStyle( 1 );

}

};

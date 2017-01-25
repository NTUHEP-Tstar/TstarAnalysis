/*******************************************************************************
*
*  Filename    : PlotStyle.ipp
*  Description : Implementation for template functions for plot styling
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMMON_PLOTSTYLE_IPP
#define TSTARANALYSIS_COMMON_PLOTSTYLE_IPP

template<class T>
void
tstar::SetDataStyle( T* x )
{
   x->SetMarkerStyle( 20 );
   x->SetMarkerSize( 0.7 );
   x->SetLineColor( kBlack );
}

/******************************************************************************/

template<class T>
void
tstar::SetErrorStyle( T* x )
{
   x->SetFillColor( KBLUE );
   x->SetLineColor( KBLUE );
   x->SetFillStyle( FILL_ST );
}

/******************************************************************************/

template<class T>
void
tstar::SetSignalStyle( T* x )
{
   x->SetLineColor( KRED );
   x->SetLineStyle( 5 );
//   x->SetFillColor( kRed );
//   x->SetFillStyle( 3005 );
}


#endif /* end of include guard: TSTARANALYSIS_COMMON_PLOTSTYLE_IPP */

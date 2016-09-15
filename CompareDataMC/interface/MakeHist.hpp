/*******************************************************************************
*
*  Filename    : MakeHist.hpp
*  Description : Helper functions for making histograms, for KinematicCompare
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_MAKEHIST_HPP
#define TSTARANALYSIS_COMPAREDATAMC_MAKEHIST_HPP

#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"

#include "TH1D.h"
#include "THStack.h"
#include <string>
#include <vector>

/*******************************************************************************
*   Main contrl flows to be called by main function see Kinematic compare.
*******************************************************************************/
extern void MakeComparePlot(
   SampleHistMgr*               data,
   std::vector<SampleHistMgr*>& bg,
   SampleHistMgr*               signal,
   const std::string            label = ""
   );

extern void Normalize(
   SampleHistMgr*               data,
   std::vector<SampleHistMgr*>& bg
   );


/*******************************************************************************
*   Helper functions for making plotting objects
*
*   ** GetBkgYield - returning the sum of all expected yield
*
*   ** SetBkgColor - Setting the color sequence of each background group
*
*   ** MakeBkgStack - Making the THStack object of a the background histograms
*
*   ** MakeBkgError - Making the Background error histogram
*
*   ** MakeBkgRelHist - Make the relative background error histograms
*
*   ** MakeDataRelHist - Make the realtive data histrogram
*
*******************************************************************************/
extern Parameter GetBkgYield( const std::vector<SampleHistMgr*>& );
extern void      SetBkgColor( std::vector<SampleHistMgr*>& );
extern THStack*  MakeBkgStack( const std::vector<SampleHistMgr*>&, const std::string& );
extern TH1D*     MakeBkgError( const std::vector<SampleHistMgr*>&, const std::string& );
extern TH1D*     MakeBkgRelHist( TH1D*, const Parameter& );
extern TH1D*     MakeDataRelHist( const TH1D* data, const TH1D* bkg );



#endif/* end of include guard: TSTARANALYSIS_COMPAREDATAMC_MAKEHIST_HPP */

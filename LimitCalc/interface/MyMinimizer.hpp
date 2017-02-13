/*******************************************************************************
*
*  Filename    : MyMinimizer.hpp
*  Description : Custom wrapper class for better function fitting option access
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_LIMITCALC_MYMINIMIZER_HPP
#define TSTARANALYSIS_LIMITCALC_MYMINIMIZER_HPP

#include "RooAbsData.h"
#include "RooAbsPdf.h"
#include "RooFitResult.h"
#include "RooMinimizer.h"

class MyMinimizer : public RooMinimizer
{
public:
  MyMinimizer( RooAbsReal* );
  virtual
  ~MyMinimizer ();

  RooFitResult* RunFit( const int weightflag );

private:

  RooAbsReal* _nll;
};

#endif/* end of include guard: TSTARANALYSIS_LIMITCALC_MYMINIMIZER_HPP */

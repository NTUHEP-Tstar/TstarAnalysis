/*******************************************************************************
*
*  Filename    : MCResMgr.hpp
*  Description : Manager class for MC resolution comparison.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_SIGNALMCSTUDY_MCRESMGR_HPP
#define TSTARANALYSIS_SIGNALMCSTUDY_MCRESMGR_HPP

#include "ManagerUtils/RootMgr/interface/HistMgr.hpp"

class MCResMgr : public mgr::HistMgr
{

public:
  MCResMgr( const std::string& name );
  virtual~MCResMgr() {}


private:
  void FillFromFile();
};

/*******************************************************************************
*   External plotting functions
*******************************************************************************/
extern void MakeMCResPlot( MCResMgr* );


#endif/* end of include guard: TSTARANALYSIS_SIGNALMCSTUDY_MCRESMGR */

/*******************************************************************************
*
*  Filename    : JetLepMgr.hpp
*  Description : Results for JetLepMgr.hpp
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_SIGNALMCSTUDY_JETLEPMGR_HPP
#define TSTARANALYSIS_SIGNALMCSTUDY_JETLEPMGR_HPP

#include "ManagerUtils/RootMgr/interface/HistMgr.hpp"

// Just for defining histograms
class JetLepMgr : public mgr::HistMgr {
public:
  JetLepMgr( const std::string& name );
  virtual ~JetLepMgr ();

private:
  void FillFromFile(); // Getting file automatically
};


/*******************************************************************************
*   Functions for producing comparison plots
*******************************************************************************/
extern void MakeJetLepCompare( std::vector<JetLepMgr*>& );


#endif /* end of include guard: TSTARANALYSIS_SIGNALMCSTUDY_JETLEPMGR_HPP */

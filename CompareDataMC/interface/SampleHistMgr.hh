/*******************************************************************************
 *
 *  Filename    : SampleHistMgr.hh
 *  Description : Augmented SampleMgr with histograms stored
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef __SAMPLEHISTMGR_HH__
#define __SAMPLEHISTMGR_HH__

#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "ManagerUtils/HistMgr/interface/HistMgr.hpp"
#include <vector>
#include <string>
#include "TH1D.h"

class SampleHistMgr : public mgr::SampleGroup, public mgr::HistMgr
{
public:
   SampleHistMgr( const std::string& );
   ~SampleHistMgr();

private:
   virtual void FillHistograms( mgr::SampleMgr& ) ;
};

#endif /* end of include guard: __SAMPLEHISTMGR_HH__ */

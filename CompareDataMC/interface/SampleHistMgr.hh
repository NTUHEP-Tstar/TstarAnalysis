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
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include <vector>
#include <string>
#include "TH1D.h"

class SampleHistMgr : public mgr::SampleGroup, public mgr::HistMgr
{
public:
   SampleHistMgr( const std::string&, const mgr::ConfigReader& );
   ~SampleHistMgr();

private:
   void fill_histograms( mgr::SampleMgr& ) ;
   virtual void define_hist(); // From HistMgr
};

#endif /* end of include guard: __SAMPLEHISTMGR_HH__ */

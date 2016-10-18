/*******************************************************************************
*
*  Filename    : SampleHistMgr.hh
*  Description : Augmented SampleMgr with histograms stored
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_SAMPLEHISTMGR_HPP
#define TSTARANALYSIS_COMPAREDATAMC_SAMPLEHISTMGR_HPP

#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/RootMgr/interface/HistMgr.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include <string>

class SampleHistMgr :
   public mgr::SampleGroup,
   public mgr::HistMgr
{
public:
   SampleHistMgr( const std::string&, const mgr::ConfigReader& );
   ~SampleHistMgr();

   void FillFromSample( mgr::SampleMgr& );

private:
   void define_hist();
};

#endif/* end of include guard: __SAMPLEHISTMGR_HH__ */

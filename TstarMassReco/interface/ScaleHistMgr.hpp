/*******************************************************************************
 *
 *  Filename    : ScaleHistMgr.hh
 *  Description : Comparison Histograms Mangaer
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef TSTARANALYSIS_TSTARMASSRECO_SCALEHISTMGR_HPP
#define TSTARANALYSIS_TSTARMASSRECO_SCALEHISTMGR_HPP

#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "ManagerUtils/HistMgr/interface/HistMgr.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include <string>

class ScaleHistMgr : public mgr::SampleGroup, public mgr::HistMgr
{
public:
   ScaleHistMgr( const std::string&, const mgr::ConfigReader& );
   ~ScaleHistMgr();

private:
   void fill_histograms( mgr::SampleMgr& ) ;
   virtual void define_hist(); // From HistMgr
};

#endif // end of include guard

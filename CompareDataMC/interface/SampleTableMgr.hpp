/*******************************************************************************
*
*  Filename    : SampleTableMgr,hpp
*  Description : Inheritance class of SampleGroup to save/load cache results to
*                file.
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_SAMPLETABLEMGR_HPP
#define TSTARANALYSIS_COMPAREDATAMC_SAMPLETABLEMGR_HPP

#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"

class SampleTableMgr :
   public mgr::SampleGroup
{
public:
   SampleTableMgr( const std::string&, const mgr::ConfigReader& );
   ~SampleTableMgr();

   void LoadFromFile();
   void LoadFromEDM();

private:
   void LoadFromEDM( mgr::SampleMgr& );
   void LoadExtraString();
   void LoadExtraString( mgr::SampleMgr&, const mgr::ConfigReader& );
};


#endif/* end of include guard: TSTARANALYSIS_COMPAREDATAMC_SAMPLETABLEMGR_HPP */

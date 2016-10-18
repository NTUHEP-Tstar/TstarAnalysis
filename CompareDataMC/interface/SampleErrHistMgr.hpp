/*******************************************************************************
*
*  Filename    : SampleErrHistMgr.hpp
*  Description : Sample histogram for managing historgram with correct error
*                representations
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_COMPAREDATAMC_SAMPLEERRHISTMGR_HPP
#define TSTARANALYSIS_COMPAREDATAMC_SAMPLEERRHISTMGR_HPP

#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "ManagerUtils/RootMgr/interface/HistMgr.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include <string>

class SampleErrHistMgr :
   public mgr::SampleGroup,
   public mgr::HistMgr
{
public:
   SampleErrHistMgr( const std::string&, const mgr::ConfigReader& );
   ~SampleErrHistMgr();

   void FillFromSample( mgr::SampleMgr& );

private:
   void define_hist();

   void AddErrHists(
      const std::string& centralhistname,
      const std::string& xlabel,
      const std::string& xunit,
      const int          bin_size,
      const double       x_min,
      const double       x_max
      );

   void FillWeightErrHists(
      const std::string& centralhistname,
      const double fillvalue,
      const double sampleweight,
      const mgr::SampleMgr& sample,
      const fwlite::EventBase& ev,
      const std::vector<std::vector<unsigned>>& pdfidgoup,
      const bool filljec
   );
};

extern const std::vector<std::string> histnamelist;

struct ErrorSource {
   std::string tag;
   std::string rootname;
   std::string label;
};
extern const std::vector<ErrorSource> histerrlist;


#endif/* end of include guard: TSTARANALYSIS_COMPAREDATAMC_SAMPLEERRHISTMGR_HPP */

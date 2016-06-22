/*******************************************************************************
 *
 *  Filename    : CompareHistMgr.hh
 *  Description : Comparison Histograms Mangaer
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#ifndef __COMPAREHISTMGR_HH__
#define __COMPAREHISTMGR_HH__

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "ManagerUtils/HistMgr/interface/HistMgr.hpp"

#include <vector>
#include <string>

#include "TH1D.h"
#include "TH2D.h"

class CompareHistMgr : public mgr::HistMgr {
public:
   CompareHistMgr(const std::string&, const std::string&,const std::string&,const std::string&,const std::string&);
   virtual ~CompareHistMgr();

   void AddEvent( const fwlite::Event& );
   TH2D* MatchMap() { return _match_map; };
   const TH2D* MatchMap() const { return _match_map; };

   unsigned EventCount() const ;
   double LepMatchRate() const ;
   double LepBMatchRate() const ;
   double LepGMatchRate() const ;
   double HadJ1MatchRate() const ;
   double HadJ2MatchRate() const ;
   double HadBMatchRate() const ;
   double HadGMatchRate() const ;

private:
   const std::string _name;
   const std::string _latex_name;
   const std::string _module_label;
   const std::string _product_label;
   const std::string _process_label;
   fwlite::Handle<RecoResult> _result_handle;

   unsigned _event_count;
   unsigned _lep_match;
   unsigned _lep_b_match;
   unsigned _lep_g_match;
   unsigned _had_j1_match;
   unsigned _had_j2_match;
   unsigned _had_b_match;
   unsigned _had_g_match;
   std::vector<TH1D*>  _hist_list;
   TH2D*  _match_map;

};

#endif /* end of include guard: __COMPAREHISTMGR_HH__ */

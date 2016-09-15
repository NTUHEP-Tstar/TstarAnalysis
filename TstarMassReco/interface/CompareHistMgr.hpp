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

#include "ManagerUtils/RootMgr/interface/HistMgr.hpp"

#include <string>
#include <vector>

#include "TH1D.h"
#include "TH2D.h"

class CompareHistMgr : public mgr::HistMgr
{
public:
   CompareHistMgr( const std::string&, const std::string& );
   virtual ~CompareHistMgr();

   void AddEvent( const fwlite::Event& );
   TH2D* MatchMap(){ return _match_map; };
   const TH2D* MatchMap() const { return _match_map; };

   int EventCount() const;

private:
   fwlite::Handle<RecoResult> _result_handle;
   TH2D* _match_map;

   virtual void define_hist();// From HistMgr

};

#endif/* end of include guard: __COMPAREHISTMGR_HH__ */

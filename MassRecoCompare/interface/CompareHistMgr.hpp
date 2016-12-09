/*******************************************************************************
*
*  Filename    : CompareHistMgr.hh
*  Description : Comparison Histograms Mangaer
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_MASSRECOCOMPARE_HPP
#define TSTARANALYSIS_MASSRECOCOMPARE_HPP

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "ManagerUtils/RootMgr/interface/HistMgr.hpp"
#include "ManagerUtils/RootMgr/interface/Hist2DMgr.hpp"

#include <string>
#include <vector>

class CompareHistMgr :
   public mgr::HistMgr,
   public mgr::Hist2DMgr
{
public:
   CompareHistMgr( const std::string&, const std::string& );
   virtual
   ~CompareHistMgr();

   void AddEvent( const fwlite::Event& );
   std::string ModuleName() const ;

   // Static member functions for histogram binning
   static int GetBinPosition( tstar::Particle_Label x );

private:
   fwlite::Handle<RecoResult>  _resulthandle;

   virtual void define_hist();// From HistMgr

};

#endif/* end of include guard */

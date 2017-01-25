/*******************************************************************************
*
*  Filename    : KeysCompMgr.hpp
*  Description : Object manager class for RooKeysPdf comparison
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_MASSRECOCOMPARE_KEYSCOMPMGR_HPP
#define TSTARANALYSIS_MASSRECOCOMPARE_KEYSCOMPMGR_HPP

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "ManagerUtils/RootMgr/interface/RooFitMgr.hpp"

#include <string>

class KeysCompMgr : public mgr::RooFitMgr
{

public:
   KeysCompMgr( const std::string&, const std::string& );
   virtual
   ~KeysCompMgr();

   static void InitStaticVars();
   static RooRealVar& x();
   static RooRealVar& w();
   static RooRealVar& n();

   void FillDataSet( const std::string& filename );
   void MakeKeysPdf();

};

#endif/* end of include guard: TSTARANALYSIS_MASSRECOCOMPARE_KEYSCOMPMGR_HPP */

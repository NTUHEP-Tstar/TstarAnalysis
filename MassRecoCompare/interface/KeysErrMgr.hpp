/*******************************************************************************
*
*  Filename    : KeysErrMgr.hpp
*  Description : KeyPdf comparison for different error sources
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#ifndef TSTARANALYSIS_MASSRECOCOMPARE_KEYSERRMGR_HP
#define TSTARANALYSIS_MASSRECOCOMPARE_KEYSERRMGR_HP

#include "ManagerUtils/RootMgr/interface/RooFitMgr.hpp"
#include <string>

class KeysCompMgr : public mgr::RooFitMgr
{

public:
  KeysCompMgr( const std::string&, const std::string& );
  virtual ~KeysCompMgr();

  static void        InitStaticVars();
  static RooRealVar& x();
  static RooRealVar& w();

  void FillDataSet( const std::string& filename );
  void MakeKeysPdf();

};


#endif/* end of include guard: TSTARANALYSIS_MASSRECOCOMPARE_KEYSERRMGR_HP */

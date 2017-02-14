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
#include <vector>
#include <string>

class KeysErrMgr : public mgr::RooFitMgr
{

public:
  KeysErrMgr( const std::string&, const std::string& );
  virtual ~KeysErrMgr();

  static void        InitStaticVars();
  static RooRealVar& x();
  static RooRealVar& w();

  void FillDataSet( const std::string& filename );
  void MakeKeysPdf();

};


#endif/* end of include guard: TSTARANALYSIS_MASSRECOCOMPARE_KEYSERRMGR_HP */

/*******************************************************************************
 *
 *  Filename    : VarMgr.hh
 *  Description : Class for RooRealVar managing across scopes
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/

#ifndef __VARMGR_HH__
#define __VARMGR_HH__

#include "RooRealVar.h"
#include <string>
#include <vector>

class VarMgr {
public:
   VarMgr();
   virtual ~VarMgr ();

   RooRealVar* NewVar(const std::string&, double,double,double);
   void SetConstant();

private:
   std::vector<RooRealVar*> _var_list;
};

extern VarMgr var_mgr;

#endif /* end of include guard: __ROOREALVARMGR_HH__ */

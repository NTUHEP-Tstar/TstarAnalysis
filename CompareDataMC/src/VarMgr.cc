/*******************************************************************************
 *
 *  Filename    : VarMgr.cc
 *  Description : Implementations for Variable Manager class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/VarMgr.hh"

using namespace std;

//------------------------------------------------------------------------------
//   Global Variables
//------------------------------------------------------------------------------
VarMgr var_mgr;

//------------------------------------------------------------------------------
//   Constructor and Desctructor
//------------------------------------------------------------------------------
VarMgr::VarMgr()
{

}

VarMgr::~VarMgr()
{
   for( auto* ptr : _var_list ){
      delete ptr;
   }
}

//------------------------------------------------------------------------------
//   Public Member functions
//------------------------------------------------------------------------------
RooRealVar* VarMgr::NewVar(const string& name, double val, double min, double max )
{
   _var_list.push_back( new RooRealVar(name.c_str(), name.c_str(), val,min,max) );
   return _var_list.back();
}

void VarMgr::SetConstant()
{
   for( auto& var : _var_list ){
      var->setConstant(kTRUE);
   }
}

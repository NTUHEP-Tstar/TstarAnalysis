/*******************************************************************************
 *
 *  Filename    : VarMgr.cc
 *  Description : Implementations for Variable Manager class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"

#include <iostream>
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
      cout << ptr->GetName() << " " << ptr->getVal() << " " << ptr->getError() << endl;
      delete ptr;
   }
}

//------------------------------------------------------------------------------
//   Public Member functions
//------------------------------------------------------------------------------
RooRealVar* VarMgr::NewVar(const string& name, double val, double min, double max )
{
   for( auto ptr: _var_list ){
      if( ptr->GetName() == name ){
         ptr->setConstant(kFALSE);
         ptr->setMax(max);
         ptr->setMin(min);
         *ptr = val;
         return ptr;
      }
   }
   _var_list.push_back( new RooRealVar(name.c_str(), name.c_str(), val,min,max) );
   return _var_list.back();
}

void VarMgr::SetConstant(const bool x)
{
   for( auto& var : _var_list ){
      var->setConstant(x);
   }
}

RooRealVar* VarMgr::FindByName(const string& name)
{
   for( auto& var : _var_list ){
      if( var->GetName() == name ){
         return var;
      }
   }
   return NULL;
}

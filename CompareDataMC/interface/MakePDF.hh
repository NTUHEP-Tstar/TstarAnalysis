/*******************************************************************************
 *
 *  Filename    : MakePDF.hh
 *  Description : Declaring common pdfs in SampleRooFitMgr objects
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/

#include "TstarAnalysis/CompareDataMC/interface/SampleRooFitMgr.hh"
#include "RooKeysPdf.h"
#include "RooGenericPdf.h"
#include <string>

// see src/MakePDF.cc for implementations
extern RooKeysPdf* MakeKeysPdf(SampleRooFitMgr*,const std::string& = "key" );

extern RooGenericPdf* MakeFermi(SampleRooFitMgr*,const std::string& = "fermi");

extern RooGenericPdf* MakeExo(SampleRooFitMgr*,const std::string& = "exo" );

// Misc. Helper functions, in src/MakePDF.cc
extern void SetConstant(RooAbsPdf*);

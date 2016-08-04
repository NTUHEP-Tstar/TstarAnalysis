/*******************************************************************************
 *
 *  Filename    : RooFit_SimFit_Val.cc
 *  Description : Validation of simfit method via signal injection
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <boost/format.hpp>

#include "TRandom.h"

using namespace std;

//------------------------------------------------------------------------------
//   Helper structures and functions
//------------------------------------------------------------------------------
void RunValGenFit( SampleRooFitMgr* data, SampleRooFitMgr* mc, SampleRooFitMgr* sigmgr )
{
   RooFitResult* bgconstrain = tmplt::MakeBGFromMC( mc );
   const double bkgnum   = data->DataSet()->sumEntries();
   const double signum   = limit_namer.HasOption("relmag") ?
      sigmgr->ExpectedYield().CentralValue() * limit_namer.InputDou("relmag") :
      limit_namer.InputDou("absmag")
   ;
   vector<RooRealVar*> paramlist = mc->VarContains("template");
   const double param1 = paramlist[0]->getVal();
   const double param2 = paramlist[1]->getVal();
   TRandom ran;

   RooAbsPdf* bkgfunc =  mc->Pdf("template");
   RooAbsPdf* sigfunc =  sigmgr->MakeKeysPdf( "key" );

   const string strgthtag = val::SigStrengthTag();
   FILE* result = fopen( limit_namer.TextFileName("valsimfit" , {strgthtag}).c_str(), "w" );
   fprintf( result , "%lf %lf %lf %lf\n" , bkgnum, signum, param1, param2);

   for( int i = 0 ; i < limit_namer.InputInt("num") ; ++i ){
      dataset_alias = "valgen";
      const int genbkgnum = ran.Poisson( bkgnum );
      const int gensignum = ran.Poisson( signum );

      RooDataSet* psuedoset = bkgfunc->generate(
         SampleRooFitMgr::x(),
         genbkgnum,
         RooFit::Name(dataset_alias.c_str())
      );
      RooDataSet* sigset = sigfunc->generate(
         SampleRooFitMgr::x(),
         gensignum
      );
      psuedoset->append( *sigset );
      data->AddDataSet( psuedoset );
      // Running sim fit
      data->SetConstant(kFALSE);
      smft::FitPDFs( data, sigmgr, bgconstrain );

      // Getting results
      const string psuedobgname = "bg"+sigmgr->Name();
      const RooRealVar* bkgfit = data->Var( sigmgr->Name()+"nb" );
      const RooRealVar* sigfit = data->Var( sigmgr->Name()+"ns" );
      const RooRealVar* p1fit  = data->Var( psuedobgname + "a" );
      const RooRealVar* p2fit  = data->Var( psuedobgname + "b" );
      fprintf(
         result, "%lf %lf \t %lf %lf \t %lf %lf \t %lf %lf\n",
         bkgfit->getVal() , bkgfit->getError(),
         sigfit->getVal() , sigfit->getError(),
         p1fit->getVal() , p1fit->getError(),
         p2fit->getVal() , p2fit->getError()
      );


      data->RemoveDataSet( dataset_alias );

      delete psuedoset;
      delete sigset;
   }
   fclose(result);
}

string val::SigStrengthTag()
{
   static boost::format secfmt("%1%%2%");
   return limit_namer.HasOption("relmag") ?
      str( secfmt % "rel" % limit_namer.InputDou("relmag") ):
      str( secfmt % "abs" % limit_namer.InputDou("absmag") );
}

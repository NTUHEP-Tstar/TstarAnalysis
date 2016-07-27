/*******************************************************************************
 *
 *  Filename    : RooFit_SimFit_Val.cc
 *  Description : Validation of simfit method via signal injection
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"

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
   tmplt::MakeBGFromMC( mc );
   const string funcname = mc->MakePdfAlias("template");
   const double bkgnum   = data->OriginalDataSet()->sumEntries();
   const double signum   = limit_namer.HasOption("relmag") ?
      sigmgr->ExpectedYield().CentralValue() * limit_namer.InputDou("relmag") :
      limit_namer.InputDou("absmag")
   ;
   const double param1 = var_mgr.FindByName("a"+funcname)->getVal();
   const double param2 = var_mgr.FindByName("b"+funcname)->getVal();
   TRandom ran;
   RooAbsPdf* bkgfunc =  mc->GetPdfFromAlias("template");
   RooAbsPdf* sigfunc =  MakeKeysPdf(sigmgr,"key");

   const string strgthtag = val::SigStrengthTag();
   FILE* result = fopen( limit_namer.TextFileName("valsimfit" , {strgthtag}).c_str(), "w" );
   fprintf( result , "%lf %lf %lf %lf\n" , bkgnum, signum, param1, param2);

   for( int i = 0 ; i < limit_namer.InputInt("num") ; ++i ){
      dataset_alias = "valgen";
      const string psuedoname    = data->MakeDataAlias(dataset_alias);
      const int genbkgnum = ran.Poisson( bkgnum );
      const int gensignum = ran.Poisson( signum );

      RooDataSet* psuedoset = bkgfunc->generate(
         SampleRooFitMgr::x(),
         genbkgnum,
         RooFit::Name(psuedoname.c_str())
      );
      RooDataSet* sigset = sigfunc->generate(
         SampleRooFitMgr::x(),
         gensignum
      );
      psuedoset->append( *sigset );

      cout << "\n\n#########\n\n\n" << endl;
      cout << gensignum << " " << genbkgnum << endl;
      cout << psuedoset->numEntries() << " " << sigset->numEntries() << " ";
      for( int i = 0 ; i < 10 ; ++i ){
         cout << ((RooRealVar*)(psuedoset->get(i)->find("x")))->getVal() << " ";
      }
      data->AddDataSet(psuedoset );
      cout << "\n##\n##" <<
         data->GetDataFromAlias( dataset_alias ) << " " <<
         psuedoset << "\n##" << endl;

      // Running sim fit
      var_mgr.SetConstant(kFALSE);
      smft::FitPDFs( data, sigmgr );

      // Getting results
      const string psuedobgname = data->MakePdfAlias( "bg"+sigmgr->Name() );
      const RooRealVar* bkgfit = var_mgr.FindByName( "nb"+sigmgr->Name() );
      const RooRealVar* sigfit = var_mgr.FindByName( "ns"+sigmgr->Name() );
      const RooRealVar* p1fit  = var_mgr.FindByName( "a"+psuedobgname );
      const RooRealVar* p2fit  = var_mgr.FindByName( "b"+psuedobgname );
      fprintf(
         result, "%lf %lf \t %lf %lf \t %lf %lf \t %lf %lf\n",
         bkgfit->getVal() , bkgfit->getError(),
         sigfit->getVal() , sigfit->getError(),
         p1fit->getVal() , p1fit->getError(),
         p2fit->getVal() , p2fit->getError()
      );


      data->RemoveDataSet( psuedoset );

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

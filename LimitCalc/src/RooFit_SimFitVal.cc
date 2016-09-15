/*******************************************************************************
*
*  Filename    : RooFit_SimFit_Val.cc
*  Description : Validation of simfit method via signal injection
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/MakeKeysPdf.hpp"
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFit.hpp"
#include "TstarAnalysis/LimitCalc/interface/SimFitVal.hpp"
#include "TstarAnalysis/LimitCalc/interface/Template.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "ManagerUtils/PlotUtils/interface/RooFitUtils.hpp"
#include <boost/format.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "TRandom.h"

using namespace std;

// ------------------------------------------------------------------------------
//   Helper structures and functions
// ------------------------------------------------------------------------------
void
RunGenFit( SampleRooFitMgr* data, SampleRooFitMgr* mc, SampleRooFitMgr* sigmgr )
{
   RooFitResult* bgconstrain = FitBackgroundTemplate( mc, "" );
   const double bkgnum       = data->DataSet()->sumEntries();
   const double signum       = limit_namer.HasOption( "relmag" ) ?
                               sigmgr->ExpectedYield().CentralValue() * limit_namer.InputDou( "relmag" ) :
                               limit_namer.InputDou( "absmag" );

   vector<RooRealVar*> paramlist = mc->VarContains( "template" );
   const double param1           = paramlist[0]->getVal();
   const double param2           = paramlist[1]->getVal();

   TRandom ran;

   RooAbsPdf* bkgfunc = mc->Pdf( "template" );
   RooAbsPdf* sigfunc = MakeSingleKeysPdf( sigmgr, "" );

   const string strgthtag = SigStrengthTag();
   FILE* result           = fopen( limit_namer.TextFileName( "valsimfit", {strgthtag} ).c_str(), "w" );
   fprintf( result, "%lf %lf %lf %lf\n", bkgnum, signum, param1, param2 );

   for( int i = 0; i < limit_namer.InputInt( "num" ); ++i ){
      const string pseudosetname = "pseudo";
      const int genbkgnum        = ran.Poisson( bkgnum );
      const int gensignum        = ran.Poisson( signum );

      RooDataSet* psuedoset = bkgfunc->generate(
         SampleRooFitMgr::x(),
         genbkgnum,
         RooFit::Name( pseudosetname.c_str() )
         );
      RooDataSet* sigset = sigfunc->generate(
         SampleRooFitMgr::x(),
         gensignum
         );
      psuedoset->append( *sigset );
      data->AddDataSet( psuedoset );
      data->SetConstant( kFALSE );

      SimFitSingle( data, sigmgr, pseudosetname, bgconstrain );

      // Getting results
      const string pseudobgname   = SimFitBGPdfName( pseudosetname, sigmgr->Name() );
      const string pseudocombname = SimFitCombinePdfName( pseudosetname, sigmgr->Name() );
      const RooRealVar* bkgfit    = data->Var( pseudocombname+"nb" );
      const RooRealVar* sigfit    = data->Var( pseudocombname+"ns" );
      const RooRealVar* p1fit     = data->Var( pseudobgname + "a" );
      const RooRealVar* p2fit     = data->Var( pseudobgname + "b" );

      fprintf(
         result, "%lf %lf \t %lf %lf \t %lf %lf \t %lf %lf\n",
         bkgfit->getVal(), bkgfit->getError(),
         sigfit->getVal(), sigfit->getError(),
         p1fit->getVal(), p1fit->getError(),
         p2fit->getVal(), p2fit->getError()
         );

      data->RemoveDataSet( pseudosetname );
      delete sigset;
   }

   fclose( result );
}

string
SigStrengthTag()
{
   static boost::format secfmt( "%1%%2%" );
   return limit_namer.HasOption( "relmag" ) ?
          str( secfmt % "rel" % limit_namer.InputDou( "relmag" ) ) :
          str( secfmt % "abs" % limit_namer.InputDou( "absmag" ) );
}

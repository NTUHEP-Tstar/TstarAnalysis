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
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

#include "RooRandom.h"

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

   RooAbsPdf* bkgfunc = mc->Pdf( "template" );
   RooAbsPdf* sigfunc = MakeSingleKeysPdf( sigmgr, "" );

   const string strgthtag = SigStrengthTag();
   FILE* result;
   if ( !boost::filesystem::exists( limit_namer.TextFileName("valsimfit",{strgthtag}) ) ){
      // If doesn't already exists create new file and print first fitting value.
      result = fopen( limit_namer.TextFileName( "valsimfit", {strgthtag} ).c_str(), "w" );
      fprintf( result, "%lf %lf %lf %lf\n", bkgnum, signum, param1, param2 );
   } else {
      // Else open in append mode.
      result = fopen( limit_namer.TextFileName( "valsimfit", {strgthtag} ).c_str(), "a" );
   }

   // Setting random seed to present time to allow multiple reruns
   RooRandom::randomGenerator()->SetSeed(time(NULL));

   for( int i = 0; i < limit_namer.InputInt( "num" ); ++i ){
      const string pseudosetname = "pseudo";

      RooDataSet* psuedoset = bkgfunc->generate(
         SampleRooFitMgr::x(),
         bkgnum,
         RooFit::Extended(),
         RooFit::Name( pseudosetname.c_str() )
      );
      RooDataSet* sigset = sigfunc->generate(
         SampleRooFitMgr::x(),
         signum,
         RooFit::Extended()
      );
      psuedoset->append( *sigset );
      delete sigset;

      // Passes ownership to data
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

      // Saving plots for special cases
      if( ( bkgfit->getVal() - bkgnum )/bkgfit->getError() > 3 ){
         MakeSimFitPlot( data, sigmgr, pseudosetname, "bkgexcess" );
      } else if( ( bkgfit->getVal() - bkgnum )/bkgfit->getError() < -3 ){
         MakeSimFitPlot( data, sigmgr, pseudosetname, "bkgdifficient" );
      } else if( ( sigfit->getVal() - signum )/sigfit->getError() > 3 ){
         MakeSimFitPlot( data, sigmgr, pseudosetname, "sigexcess" );
      } else if( ( sigfit->getVal() - signum )/sigfit->getError() < -3 ){
         MakeSimFitPlot( data, sigmgr, pseudosetname, "sigdifficient" );
      } else if( i%100 == 0 ){
         MakeSimFitPlot( data, sigmgr, pseudosetname );
      }

      // Deleting to avoid taking up to much memory
      data->RemoveDataSet( pseudosetname );
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
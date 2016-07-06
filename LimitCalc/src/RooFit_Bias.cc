/*******************************************************************************
 *
 *  Filename    : RooFit_Bias.cc
 *  Description : Making roofit objects for bias checking to Higgs combine
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/LimitCalc/interface/SampleRooFitMgr.hpp"
#include "TstarAnalysis/LimitCalc/interface/RooFit_Common.hpp"
#include "TstarAnalysis/LimitCalc/interface/VarMgr.hpp"
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"

using namespace std;

//------------------------------------------------------------------------------
//   Calling function in other files
//------------------------------------------------------------------------------
static void MakePsuedoData(SampleRooFitMgr*,const unsigned);
extern void MakeTemplate(SampleRooFitMgr*,SampleRooFitMgr*,vector<SampleRooFitMgr*>&);
extern void MakeSimFit(SampleRooFitMgr*,std::vector<SampleRooFitMgr*>&);

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void MakeBias(SampleRooFitMgr* data, SampleRooFitMgr* mc, vector<SampleRooFitMgr*>& signal_list )
{
   MakePsuedoData(mc,data->OriginalDataSet()->sumEntries());
   if( limit_namer.GetFitMethod() == "BiasTemplate" ){
      MakeTemplate(mc,mc,signal_list);
   } else if( limit_namer.GetFitMethod() == "BiasSimFit" ){
      MakeSimFit(mc,signal_list);
   } else {
      fprintf( stderr, "Warning! Not enough information in method [%s], using Template by default..." , limit_namer.GetFitMethod().c_str() );
      fflush( stderr );
      limit_namer.SetFitMethod("BiasTemplate");
      MakeTemplate(mc,mc,signal_list);
   }
}

//------------------------------------------------------------------------------
//   Helper function implementations
//------------------------------------------------------------------------------
void MakePsuedoData(SampleRooFitMgr* mc, const unsigned n_events )
{
   RooGenericPdf* bg_pdf;

   if( limit_namer.GetFitFunc() == "Exo" ){
      bg_pdf = MakeExo( mc, "bias" );
   } else if( limit_namer.GetFitFunc() == "Fermi" ){
      bg_pdf = MakeFermi( mc, "bias" );
   } else {
      bg_pdf = MakeFermi( mc, "bias" );
   }

   bg_pdf->fitTo( *(mc->OriginalDataSet()) ,
      RooFit::Save() ,            // Suppressing output
      RooFit::SumW2Error(kTRUE),  // For Weighted dataset
      RooFit::Range("FitRange"),  // Fitting range
      RooFit::Minos(kTRUE),
      RooFit::Verbose(kFALSE),
      RooFit::PrintLevel(-1)
   );
   var_mgr.SetConstant();

   const string dat_name = mc->MakeDataAlias("bias");
   RooDataSet* my_data = bg_pdf->generate(
      SampleRooFitMgr::x(),
      n_events,
      RooFit::Name( dat_name.c_str() )
   );
   mc->AddDataSet( my_data );
   limit_namer.SetTag("fitset","bias");
}

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

#include "RooHistPdf.h"
#include "TRandom3.h"

using namespace std;

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void MakeBias(SampleRooFitMgr* data, SampleRooFitMgr* mc, vector<SampleRooFitMgr*>& signal_list )
{
   using namespace bias;
   int events_to_gen = data->OriginalDataSet()->sumEntries() ;
   MakePsuedoData(mc,events_to_gen);

   if( limit_namer.GetInput("fitmethod") == "BiasTemplate" ){
      MakeTemplate(mc,mc,signal_list);
   } else if( limit_namer.GetInput("fitmethod") == "BiasSimFit" ){
      MakeSimFit(mc,mc,signal_list);
   }
}

//------------------------------------------------------------------------------
//   Helper function implementations
//------------------------------------------------------------------------------
void bias::MakePsuedoData(SampleRooFitMgr* mc, const unsigned n_events )
{
   SampleRooFitMgr::x().setBins(200);
   const string bin_name  = mc->MakeDataAlias("binned");
   const string pdf_name  = mc->MakePdfAlias("binned");
   const string dat_name  = mc->MakeDataAlias("bias");
   RooDataHist* bin_data  = mc->OriginalDataSet()->binnedClone(
         bin_name.c_str(),
         bin_name.c_str()
   );
   RooHistPdf*  my_pdf = new RooHistPdf(
      pdf_name.c_str(), pdf_name.c_str(),
      SampleRooFitMgr::x(),
      *bin_data
   );
   RooDataSet* my_data = my_pdf->generate(
      SampleRooFitMgr::x(),
      n_events,
      RooFit::Name(dat_name.c_str())
   );
   mc->AddDataSet( my_data );
   mc->AddDataHist( bin_data );
   mc->AddPdf( my_pdf );
   dataset_alias = "bias";

   cout << "Done! " << my_data->sumEntries() << " effective entries!" << endl;
   SampleRooFitMgr::x().setBins(40);
}

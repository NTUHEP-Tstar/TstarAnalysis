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

#include "TRandom3.h"

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
   vector<bool> has_selected;
   double       max_weight = 0;
   const string dat_name   = mc->MakeDataAlias("bias");
   RooDataSet*  mc_set     = mc->OriginalDataSet();
   RooDataSet*  my_data    = new RooDataSet(
      dat_name.c_str(), dat_name.c_str(),
      RooArgSet(SampleRooFitMgr::x(), SampleRooFitMgr::w()),
      RooFit::WeightVar( SampleRooFitMgr::w())
   );
   TRandom3    my_gen;

   mc->AddDataSet( my_data );
   limit_namer.SetTag("fitset","bias");

   // Determining maximum weight
   cout << "Determining maximum weight..." << endl;
   for( int i = 0 ; i < mc_set->numEntries(); ++i ){
      has_selected.push_back(false);
      mc_set->get(i);
      const double entry_weight = mc_set->weight();
      if( fabs(entry_weight) > max_weight ){
         max_weight = fabs(entry_weight);
      }
   }
   max_weight *= 1.1; // To avoid null selection at edges
   cout << "Maximum weight: " << max_weight;

   // Generating pseudo data from sampling mc
   while( my_data->sumEntries() < n_events ){
      cout << "\rGenerating pseudo data..."
           << my_data->sumEntries() << "/"
           << my_data->numEntries() << flush;

      int     my_index     = my_gen.Integer( mc_set->numEntries() );
      double  my_weight    = my_gen.Uniform( max_weight );
      const RooArgSet* entry = mc_set->get(my_index);
      double  entry_mass   = entry->getRealValue("x");
      double  entry_weight = mc_set->weight();
      double  input_weight = entry_weight > 0 ? 1 : -1;
      if( entry_weight > 0 && entry_weight <= my_weight && !has_selected[my_index] ){
         SampleRooFitMgr::x() = entry_mass;
         my_data->add( RooArgSet(SampleRooFitMgr::x()), input_weight );
         has_selected[my_index] == true;
      }
   }
   cout << "Done! " << my_data->sumEntries() << " effective entries!" << endl;
}

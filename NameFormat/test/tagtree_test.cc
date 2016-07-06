/*******************************************************************************
 *
 *  Filename    : tagtree_test.cc
 *  Description : Exhuastive test for file naming class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/NameFormat/interface/TagTree.hpp"
#include "TstarAnalysis/NameFormat/interface/NameObj.hpp"

#include <vector>
#include <string>
#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
   vector<TagTree*>  all_tag_settings = {
      &compare_namer,
      &massreco_namer,
      &limit_namer
   };

   vector<string>  all_channels = {
      "MuonSignal",
      "ElectronSignal"
   };

   vector<string> all_methods = {
      "SimFit",
      "Template",
      "SideBand",
      "SimFit_Bias",
   };

   vector<string> all_fit_funcs = {
      "Exo",
      "Fermi"
   };

   vector<string> all_combine_methods = {
      "Asymptotic"
   };

   for( const auto& tag : all_tag_settings ){
      for( const auto& channel : all_channels ){
         for( const auto& method: all_methods ){
            for( const auto& fit_func : all_fit_funcs ){
               for( const auto& combine : all_combine_methods ){
                  tag->SetChannel( channel );
                  tag->SetFitMethod( method );
                  tag->SetFitFunc( fit_func );
                  tag->SetCombineMethod( combine );

                  tag->SettingFilePath();
                  tag->EDMFilePath();

                  cout << tag->GetChannel() << endl;
                  cout << tag->GetChannelDataTag()   << endl;
                  cout << tag->GetChannelLatex()     << endl;
                  cout << tag->GetChannelRoot()      << endl;

                  cout << tag->GetFitMethod() << endl;
                  cout << tag->GetFitMethodFull() << endl;

                  cout << tag->GetFitFunc() << endl;
                  cout << tag->GetFitFuncFull()    << endl;
                  cout << tag->GetFitFuncLatex()   << endl;
                  cout << tag->GetFitFuncRoot()    << endl;

                  cout << tag->GetCombineMethod()  << endl;
                  cout << tag->GetCombineMethodFull()  << endl;

                  cout << tag->MakeFileName("test","file","name")  << endl;
                  cout << tag->PlotFileName("test","plot") << endl;
                  cout << tag->TextFileName("test","text") << endl;
                  cout << tag->TexFileName("test","tex") << endl;
                  cout << tag->RootFileName("test","root") << endl;
               }
            }
         }
      }
   }

   return 0;
}

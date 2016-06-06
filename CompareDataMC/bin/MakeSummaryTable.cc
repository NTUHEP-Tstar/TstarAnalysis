/*******************************************************************************
 *
 *  Filename    : MakeSummaryTable.cc
 *  Description : Generate Latex Summary table
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleMgr.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "ManagerUtils/BaseClass/interface/ConfigReader.hpp"
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;
using mgr::SampleMgr;

int main(int argc, char* argv[])
{
   if( argc != 2 ){
      cerr << "Error! Expected exactly one arguement!" << endl;
      return 1;
   }
   cout << "Making summary table!" << endl;
   SetChannelType( argv[1] );

   SampleMgr::InitStaticFromFile( "./data/Static.json" );
   SampleMgr::SetFilePrefix( GetEDMPrefix() );

   vector<SampleMgr*> all_samples;
   vector<unsigned> sep_index;
   sep_index.push_back(all_samples.size());
   all_samples.push_back( new mgr::SampleMgr( "tstar_M700"    , "./data/Signal.json" ));
   all_samples.push_back( new mgr::SampleMgr( "tstar_M800"    , "./data/Signal.json" ));
   all_samples.push_back( new mgr::SampleMgr( "tstar_M900"    , "./data/Signal.json" ));
   all_samples.push_back( new mgr::SampleMgr( "tstar_M1000"   , "./data/Signal.json"  ));
   all_samples.push_back( new mgr::SampleMgr( "tstar_M1100"   , "./data/Signal.json"  ));
   all_samples.push_back( new mgr::SampleMgr( "tstar_M1200"   , "./data/Signal.json"  ));
   // all_samples.push_back( new mgr::SampleMgr( "tstar_M1300", "./data/Signal.json" ));
   all_samples.push_back( new mgr::SampleMgr( "tstar_M1400"   , "data/Signal.json" ));
   all_samples.push_back( new mgr::SampleMgr( "tstar_M1500"   , "data/Signal.json" ));
   all_samples.push_back( new mgr::SampleMgr( "tstar_M1600"   , "data/Signal.json" ));


   sep_index.push_back(all_samples.size());
   all_samples.push_back( new mgr::SampleMgr( "TTJets"  , "./data/TopSamples.json"        ) );

   sep_index.push_back( all_samples.size());
   all_samples.push_back( new mgr::SampleMgr( "SingleT_S"     , "./data/TopSamples.json"  ) );
   all_samples.push_back( new mgr::SampleMgr( "SingleT_T"     , "./data/TopSamples.json"  ) );
   all_samples.push_back( new mgr::SampleMgr( "SingleT_TW"    , "./data/TopSamples.json"  ) );
   all_samples.push_back( new mgr::SampleMgr( "SingleTbar_TW" , "./data/TopSamples.json"  ) );

   sep_index.push_back( all_samples.size() );
   all_samples.push_back( new mgr::SampleMgr( "TTW_Lepton", "./data/TTPlusBoson.json"     ) );
   all_samples.push_back( new mgr::SampleMgr( "TTW_Quark" , "./data/TTPlusBoson.json"     ) );
   all_samples.push_back( new mgr::SampleMgr( "TTZ_Lepton", "./data/TTPlusBoson.json"     ) );
   all_samples.push_back( new mgr::SampleMgr( "TTZ_Quark" , "./data/TTPlusBoson.json"     ) );

   sep_index.push_back( all_samples.size() );
   all_samples.push_back( new mgr::SampleMgr( "WJets_100_200"   , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "WJets_200_400"   , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "WJets_400_600"   , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "WJets_600_800"   , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "WJets_800_1200"  , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "WJets_1200_2500" , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "WJets_2500_Inf"  , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "ZJets_100_200"   , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "ZJets_200_400"   , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "ZJets_400_600"   , "./data/SingleBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "ZJets_600_Inf"   , "./data/SingleBoson.json" ) );

   sep_index.push_back( all_samples.size() );
   all_samples.push_back( new mgr::SampleMgr( "WW" , "./data/DiBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "WZ" , "./data/DiBoson.json" ) );
   all_samples.push_back( new mgr::SampleMgr( "ZZ" , "./data/DiBoson.json" ) );
   sep_index.push_back( all_samples.size() );

   unsigned obs = 0;
   if( GetChannel() == "MuonSignal" ){
      obs += mgr::SampleMgr("Run2015D", "./data/Data_Muon.json" ).EventsInFile();
      obs += mgr::SampleMgr("Run2015C", "./data/Data_Muon.json" ).EventsInFile();
   } else {
      obs += mgr::SampleMgr("Run2015D", "./data/Data_Electron.json").EventsInFile();
      obs += mgr::SampleMgr("Run2015C", "./data/Data_Electron.json").EventsInFile();

   }

   Parameter obs_yield(obs,0,0);
   Parameter exp_yield(0,0,0);

   const char table_line[] = "%-55s & %25s & %35s & %10s & %35s \\\\ \n" ;
   const char hline_line[] = "\\hline\n";
   FILE* tex_file = fopen( GetTexSummaryFile().c_str() , "w" );
   fprintf( tex_file , "\\begin{tabular}{|l|ccc|c|}\n");
   fprintf( tex_file , hline_line );
   fprintf( tex_file , table_line ,
      "Sample" ,
      "Cross Section ($pb$)" ,
      "Selection Efficiency",
      "K Factor" ,
      "Expected Yield" );
   fprintf( tex_file , hline_line );

   for( unsigned i = 0 ; i < sep_index.size() -1 ; ++i ){
      fprintf( tex_file , hline_line );
      for( unsigned j = sep_index[i] ; j < sep_index[i+1]; ++j ){
         SampleMgr& sample = *(all_samples.at(j));
         fprintf( tex_file, table_line,
            sample.LatexName().c_str(),
            sample.CrossSection().LatexFormat().c_str(),
            sample.SelectionEfficiency().LatexFormat(2).c_str(),
            sample.KFactor().LatexFormat().c_str(),
            sample.ExpectedYield().LatexFormat(1).c_str() );
         if( j >= sep_index[1] ){
            exp_yield += sample.ExpectedYield();
         }
      }
   }
   fprintf( tex_file , hline_line );
   fprintf( tex_file , hline_line );
   fprintf( tex_file, table_line ,
      "Total MC Background",
      "",
      "",
      "",
      exp_yield.LatexFormat().c_str() );
   fprintf( tex_file , hline_line );
   fprintf( tex_file, table_line,
      "Data",
      "",
      "",
      "",
      obs_yield.LatexFormat().c_str() );
   fprintf( tex_file, hline_line);
   fprintf( tex_file , "\\end{tabular}\n");
   fclose( tex_file );

   return 0;
}

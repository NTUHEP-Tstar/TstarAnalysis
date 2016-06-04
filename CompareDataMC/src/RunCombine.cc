/*******************************************************************************
 *
 *  Filename    : RunCombine.cc
 *  Description : One Line descrption of file contents
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *   Additional comments
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/FileNames.hh"
#include "ManagerUtils/SysUtils/interface/SystemUtils.hh"
#include <vector>
#include <string>

using namespace std;

//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
extern int GetInt( const string& ); // in src/RooFit_SideBand.cc

//------------------------------------------------------------------------------
//   Main control flows
//------------------------------------------------------------------------------
void RunCombine( const string& hc_opt )
{
   char combine_cmd[4096];
   char mv_cmd[1024];
   char cmd[4096];

   vector<string> signal_list = {""};
   for( const auto& signal : signal_list ){
      int masspoint = GetInt(signal);
      sprintf( combine_cmd, "combine -M %s -m %d %s %s",
         GetHCMethod().c_str(),
         masspoint,
         GetCardFile(signal).c_str(),
         hc_opt.c_str()
      );
      sprintf( mv_cmd, "rm -rf %s && mv %s %s",
         HCStoreFile(masspoint).c_str(),
         HCRawOutputFile(masspoint).c_str(),
         HCStoreFile(masspoint).c_str()
      );
      sprintf( cmd , "( %s &> /dev/null && %s )&" ,
         combine_cmd,
         mv_cmd
      );
      fprintf( stdout , "Submitting command for %s\n", signal.c_str() );
      system( cmd );
   }
   WaitProcess("combine" , "PlotLimit" );
}

/*******************************************************************************
*
*  Filename    : SummaryTable.cc
*  Description : Generating latex summary tables
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/ParameterFormat.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "TstarAnalysis/Common/interface/ComputeSelectionEff.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeTable.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace mgr;

/*******************************************************************************
*   Main control functions
*******************************************************************************/
void
SummaryComplete(
   const vector<SampleGroup*>& siglist,
   const vector<SampleGroup*>& bkglist,
   const SampleGroup*          data )
{
   FILE* file = OpenSelecFile( "" );

   for( const auto& sig : siglist ){
      cout << "Signal sample " << sig->Name()  << endl;
      PrintSampleLine( file, sig->Sample() );
   }

   PrintHline( file );

   Parameter expyield( 0, 0, 0 );
   Parameter obsyield( data->EventsInFile(), 0, 0 );

   for( const auto& grp : bkglist ){
      for( const auto& smp : grp->SampleList() ){
         cout << "Background sample " << smp->Name() << endl;
         PrintSampleLine( file, smp );
      }

      expyield += grp->ExpectedYield();
      PrintHline( file );
   }

   PrintCount( file, "MC background", expyield );
   PrintHline( file );
   PrintCount( file, "Data", obsyield );

   CloseTableFile( file );
}

/******************************************************************************/

void
SummarySignal(  const vector<SampleGroup*>& siglist )
{
   FILE* file = OpenSelecFile( "sig" );

   for( const auto& sig : siglist ){
      PrintSampleLine( file, sig->Sample() );
   }

   CloseTableFile( file );
}


/******************************************************************************/

void
SummaryBKGBrief( const vector<SampleGroup*>& bkglist, const SampleGroup* data )
{
   FILE* file = OpenSimpleFile( "bkg" );
   Parameter expyield( 0, 0, 0 );
   Parameter obsyield( data->EventsInFile(), 0, 0 );

   for( const auto& grp : bkglist ){
      PrintSimpleLine( file, grp );
      expyield += grp->ExpectedYield();
   }

   PrintHline( file );
   PrintSimpleCount( file, "Bkg. Total", expyield );
   PrintHline( file );
   PrintSimpleCount( file, "Data", obsyield );

   CloseTableFile( file );
}

/******************************************************************************/

void
SummaryMCLumi(
   const vector<SampleGroup*>& siglist,
   const vector<SampleGroup*>& bkglist )
{
   FILE* file = OpenLumiFile( "lumi" );

   for( const auto& sig : siglist ){
      PrintLumiLine( file, sig->Sample() );
   }

   for( const auto& grp : bkglist ){
      PrintHline( file );

      for( const auto& smp : grp->SampleList() ){
         PrintLumiLine( file, smp );
      }
   }

   CloseTableFile( file );
}

/*******************************************************************************
*   Common Helper functions
*******************************************************************************/
void
PrintHline( FILE* file )
{
   static const char hline_line[] = "\\hline\n";
   fprintf( file, hline_line );
}

/******************************************************************************/

void
CloseTableFile( FILE* file )
{
   PrintHline( file );
   fprintf( file, "\\end{tabular}\n" );
   fclose( file );
}

/*******************************************************************************
*   Helper functions for detailed documentaion files
*******************************************************************************/
static const char selecline[] = "%-55s & %25s & %35s & %10s & %35s \\\\\n";

FILE*
OpenSelecFile( const string& tag )
{
   FILE* file = fopen( compare_namer.TexFileName( "summary", {tag} ).c_str(), "w" );
   fprintf( file, "\\begin{tabular}{|l|ccc|c|}\n" );
   PrintHline( file );
   fprintf( file, selecline,
      "Sample",
      "Cross Section ($pb$)",
      "Selection Efficiency",
      "K Factor",
      "Expected Yield"
      );
   PrintHline( file );

   return file;
}

/******************************************************************************/

void
PrintSampleLine( FILE* file, const SampleMgr* x )
{
   fprintf( file, selecline,
      x->LatexName().c_str(),
      FloatingPoint( x->CrossSection(), -1 ).c_str(),
      Scientific( x->SelectionEfficiency(), 2 ).c_str(),
      FloatingPoint( x->KFactor(),       -1 ).c_str(),
      FloatingPoint( x->ExpectedYield(), 1 ).c_str()
      );
}

/******************************************************************************/

void
PrintCount( FILE* file, const string& tag, const Parameter& x )
{
   fprintf( file, selecline,
      tag.c_str(),
      "",
      "",
      "",
      FloatingPoint( x, 0 ).c_str()
      );
}

/*******************************************************************************
*   Helper functions for simplified summary
*******************************************************************************/
static const char simpleline[] = "%-30s & %35s & %35s &%35s\\\\\n";

FILE*
OpenSimpleFile( const string& tag )
{
   FILE* file = fopen( compare_namer.TexFileName( "summary", {tag} ).c_str(), "w" );
   fprintf( file, "\\begin{tabular}{|l|cc|c|}\n" );
   PrintHline( file );
   fprintf( file, simpleline,
      "Sample",
      "Cross section ($pb$)",
      "Average Efficiency",
      "Expected Yield"
      );
   PrintHline( file );
   return file;
}

/******************************************************************************/

void
PrintSimpleLine( FILE* file, const SampleGroup* x )
{
   fprintf( file, simpleline,
      x->LatexName().c_str(),
      FloatingPoint( x->TotalCrossSection(), 2 ).c_str(),
      Scientific( x->AvgSelectionEfficiency(), 2 ).c_str(),
      FloatingPoint( x->ExpectedYield(), 0 ).c_str()
      );
}

/******************************************************************************/

void
PrintSimpleCount( FILE* file, const string& tag, const Parameter& x )
{
   fprintf( file, simpleline,
      tag.c_str(),
      "",
      "",
      FloatingPoint( x, 0 ).c_str()
      );
}

/*******************************************************************************
*   Helper functions for Lumi summary files
*******************************************************************************/
static const char lumi_line[] = "%-55s %25s %10s %35s\\\\\n";

FILE*
OpenLumiFile( const string& tag )
{
   FILE* file = fopen( compare_namer.TexFileName( "summary", {tag} ).c_str(), "w" );
   fprintf( file, "\\begin{tabular}{|l|ccc|}\n" );
   PrintHline( file );
   fprintf( file, lumi_line,
      "Sample",
      "Cross section($pb$)",
      "K Factor",
      "Equiv. Luminosity ($pb^{-1}$)"
      );
   PrintHline( file );
   return file;
}

/******************************************************************************/

void
PrintLumiLine( FILE* file, const SampleMgr* x )
{
   double equiv = GetOriginalEventCount( *x );
   equiv /= x->ExpectedYield();
   equiv /= x->KFactor().CentralValue();
   fprintf( file, lumi_line,
      x->LatexName().c_str(),
      FloatingPoint( x->CrossSection(),        -1 ).c_str(),
      FloatingPoint( x->KFactor(),             -1 ).c_str(),
      FloatingPoint( Parameter( equiv, 0, 0 ), 1 ).c_str()
      );
}

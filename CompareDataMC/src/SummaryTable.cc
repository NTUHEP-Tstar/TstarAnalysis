/*******************************************************************************
*
*  Filename    : SummaryTable.cc
*  Description : Generating latex summary tables
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/ParameterArithmetic.hpp"
#include "ManagerUtils/Maths/interface/ParameterFormat.hpp"
#include "ManagerUtils/SampleMgr/interface/MultiFile.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeTable.hpp"

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include <boost/format.hpp>
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
   const vector<SampleTableMgr>& siglist,
   const vector<SampleTableMgr>& bkglist,
   const SampleTableMgr&         data )
{
   FILE* file = OpenSelecFile( "" );

   for( const auto& sig : siglist ){
      PrintSampleLine( file, sig.Sample() );
   }

   PrintHline( file );

   Parameter expyield( 0, 0, 0 );
   Parameter obsyield( data.ExpectedYield(), 0, 0 );

   for( const auto& grp : bkglist ){
      for( const auto& smp : grp.SampleList() ){
         PrintSampleLine( file, smp );
         const Parameter smpyield = smp.ExpectedYield()// double
                                    * Prod( smp.CrossSection().NormParam()// 1+- rel error
                                          , smp.SelectionEfficiency().NormParam()
                                          , GetWeightError( smp )
            );
         expyield += smpyield;
      }

      PrintHline( file );
   }

   PrintCount( file, "MC background", expyield );
   PrintHline( file );
   PrintCount( file, "Data", obsyield );

   CloseTableFile( file );
}

/******************************************************************************/

void
SummaryBrief(
   const vector<SampleTableMgr>& siglist,
   const vector<SampleTableMgr>& bkglist,
   const SampleTableMgr&         data )
{
   FILE* file      = OpenSimpleFile( "brief" );
   double expyield = 0;
   double obsyield = data.ExpectedYield();

   for( const auto& grp : siglist ){
      PrintSimpleLine( file, grp );
   }

   PrintHline( file );

   for( const auto& grp : bkglist ){
      PrintSimpleLine( file, grp );
      expyield += grp.ExpectedYield();
   }

   PrintHline( file );
   PrintSimpleCount( file, "Bkg. Total", Parameter( expyield, 0, 0 ) );
   PrintHline( file );
   PrintSimpleCount( file, "Data", Parameter( obsyield, 0, 0 ) );

   CloseTableFile( file );
}

/******************************************************************************/

void
SummaryMCLumi(
   const vector<SampleTableMgr>& siglist,
   const vector<SampleTableMgr>& bkglist )
{
   FILE* file = OpenLumiFile( "lumi" );

   for( const auto& sig : siglist ){
      PrintLumiLine( file, sig.Sample() );
   }

   for( const auto& grp : bkglist ){
      PrintHline( file );

      for( const auto& smp : grp.SampleList() ){
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
static const char selecline[] = "%-45s & %25s & %35s & %15s & %35s \\\\\n";

FILE*
OpenSelecFile( const string& tag )
{
   FILE* file = fopen( compnamer.TexFileName( "summary", {tag} ).c_str(), "w" );
   fprintf( file, "\\begin{tabular}{|l|ccc|c|}\n" );
   PrintHline( file );
   fprintf( file, selecline,
      "Sample",
      "Cross Section (pb)",
      "Selection Efficiency",
      "Weight Errors",
      "Expected Yield"
      );
   PrintHline( file );

   return file;
}

/******************************************************************************/

void
PrintSampleLine( FILE* file, const SampleMgr& x )
{
   const string namecol = x.LatexName();
   const string xseccol = x.Name().find( "Tstar" ) != string::npos ?
                          Scientific( x.CrossSection(), 2 ) :
                          XSecStr( x.CrossSection() );
   const string effcol = Scientific( x.SelectionEfficiency()*x.KFactor(), 2 );
   const Parameter err = GetWeightError( x );
   const string errcol = "+" + FloatingPoint( err.RelUpperError(), 2 )+ "/-" + FloatingPoint( err.RelLowerError(), 2 );

   const Parameter expyield = x.ExpectedYield()// double
                              * Prod( x.CrossSection().NormParam()// 1+- rel error
                                    , x.SelectionEfficiency().NormParam()
                                    , GetWeightError( x )
      );
   string expcol;
   if( expyield.CentralValue() > 100 ){
      expcol = FloatingPoint( expyield.CentralValue(), 0 );
   } else {
      expcol = FloatingPoint( expyield.CentralValue(), 1 );
   }

   fprintf( file, selecline,
      namecol.c_str(),
      AddMathBrace( xseccol ).c_str(),
      AddMathBrace( effcol ).c_str(),
      errcol.c_str(),
      AddMathBrace( expcol ).c_str()
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
      AddMathBrace( FloatingPoint( x, 0 ) ).c_str()
      );
}

/*******************************************************************************
*   Helper functions for simplified summary
*******************************************************************************/
static const char simpleline[] = "%-40s & %25s &%20s\\\\\n";

FILE*
OpenSimpleFile( const string& tag )
{
   FILE* file = fopen( compnamer.TexFileName( "summary", {tag} ).c_str(), "w" );
   fprintf( file, "\\begin{tabular}{|l|cc|c|}\n" );
   PrintHline( file );
   fprintf( file, simpleline,
      "Process",
      "Average Efficiency",
      "Expected Yield"
      );
   PrintHline( file );
   return file;
}

/******************************************************************************/

void
PrintSimpleLine( FILE* file, const SampleTableMgr& x )
{
   Parameter expyield( 0, 0, 0 );

   for( const auto& smp : x.SampleList() ){
      expyield += smp.ExpectedYield()
                  * smp.CrossSection().NormParam()
                  * smp.SelectionEfficiency().NormParam()
                  * GetWeightError( smp );
   }

   fprintf( file, simpleline,
      x.LatexName().c_str(),
      AddMathBrace( Scientific( x.AvgSelectionEfficiency().CentralValue(), 2 ) ).c_str(),
      AddMathBrace( FloatingPoint( expyield.CentralValue(), 0 ) ).c_str()
      );
}

/******************************************************************************/

void
PrintSimpleCount( FILE* file, const string& tag, const Parameter& x )
{
   fprintf( file, simpleline,
      tag.c_str(),
      "",
      AddMathBrace( FloatingPoint( x, 0 ) ).c_str()
      );
}

/*******************************************************************************
*   Helper functions for Lumi summary files
*******************************************************************************/
static const char lumi_line[] = "%-55s & %35s & %10s & %35s &  %20s\\\\\n";

FILE*
OpenLumiFile( const string& tag )
{
   FILE* file = fopen( compnamer.TexFileName( "summary", {tag} ).c_str(), "w" );
   fprintf( file, "\\begin{tabular}{|l|ccc|c|}\n" );
   PrintHline( file );
   fprintf( file, lumi_line,
      "Process",
      "Cross section ($pb$)",
      "K Factor",
      "Equiv. \\Lumi (\\pbinv)",
      "Generator"
      );
   PrintHline( file );
   return file;
}

/******************************************************************************/

void
PrintLumiLine( FILE* file, const SampleMgr& x )
{
   double equiv = x.EffectiveLuminosity();

   const string namecol = x.LatexName();
   const string xsec    = x.Name().find( "Tstar" ) != string::npos ?
                          Scientific( x.CrossSection(), 2 ) :
                          XSecStr( x.CrossSection() );
   const string kfaccol = FloatingPoint( x.KFactor(), -1 );
   const string xseccol = str( boost::format( "%s(%s)" )%xsec%x.GetCacheString( "XsecSource" ) );
   const string lumicol = Scientific( Parameter( equiv, 0, 0 ), 2 ).c_str();
   const string gencol  = x.GetCacheString( "Generator" );

   fprintf(
      file, lumi_line,
      namecol.c_str(),
      AddMathBrace( xseccol ).c_str(),
      kfaccol.c_str(),
      AddMathBrace( lumicol ).c_str(),
      gencol.c_str()
      );
}

/******************************************************************************/

Parameter
GetWeightError( const mgr::SampleMgr& sample )
{
   Parameter err(
      sample.GetCacheDouble( "EventWeightSum" ),
      sample.GetCacheDouble( "EventWeightUpSum" ) - sample.GetCacheDouble( "EventWeightSum" ),
      sample.GetCacheDouble( "EventWeightSum" )   - sample.GetCacheDouble( "EventWeightDownSum" )
      );
   err /= err.CentralValue();
   err *= Parameter( 1, 0.046, 0.046 );
   err *= Parameter( 1, 0.03, 0.03 );
   return err;
}

/******************************************************************************/

string
AddMathBrace( const std::string& x )
{
   return "$" + x + "$";
}

/******************************************************************************/

string
XSecStr( const Parameter& x )
{
   if( x.CentralValue() > 100 ){
      return FloatingPoint( x, 0 );
   } else if( x.CentralValue() > 10 ){
      return FloatingPoint( x, 1 );
   } else {
      return Scientific( x, 2 );
   }
}

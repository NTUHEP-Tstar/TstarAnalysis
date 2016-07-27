/*******************************************************************************
 *
 *  Filename    : SummaryTable.cc
 *  Description : Generating latex summary tables
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace mgr;
typedef vector<SampleGroup*> GroupList;

//------------------------------------------------------------------------------
//   Static variables and Stuff
//------------------------------------------------------------------------------

// For complete summary
static FILE* OpenSelecFile( const string& );
static void PrintSampleLine(FILE*,const SampleMgr*);
static void PrintHline(FILE*);
static void PrintCount(FILE*,const string&,const Parameter&);

// For simple summary part
static FILE* OpenSimpleFile( const string&);
static void PrintSimpleLine(FILE*,const SampleGroup*);
static void PrintSimpleCount(FILE*,const string&,const Parameter&);

// For lumi part
static FILE* OpenLumiFile( const string& );
static void PrintLumiLine(FILE*,const SampleMgr*);

static void CloseTableFile(FILE*);

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void SummaryComplete( const GroupList& sig_list, const GroupList& bkg_list, const SampleGroup* data )
{
   FILE* file = OpenSelecFile("");

   for( const auto& sig : sig_list ){ PrintSampleLine(file,sig->Sample()); }
   PrintHline(file);

   Parameter exp_yield(0,0,0);
   Parameter obs_yield(data->EventsInFile(),0,0);
   for( const auto& grp : bkg_list ){
      for( const auto& smp : grp->SampleList() ){ PrintSampleLine(file,smp); }
      exp_yield += grp->ExpectedYield();
      PrintHline(file);
   }

   PrintCount(file,"MC background", exp_yield );
   PrintHline(file);
   PrintCount(file,"Data",obs_yield);

   CloseTableFile(file);
}

void SummarySignal(  const GroupList& sig_list )
{
   FILE* file = OpenSelecFile("sig");
   for( const auto& sig : sig_list ){ PrintSampleLine(file,sig->Sample()); }
   CloseTableFile(file);
}

void SummaryBKGBrief( const GroupList& bkg_list, const SampleGroup* data )
{
   FILE* file = OpenSimpleFile("bkg");
   Parameter exp_yield(0,0,0);
   Parameter obs_yield(data->EventsInFile(),0,0);
   for( const auto& grp : bkg_list ){
      PrintSimpleLine(file,grp);
      exp_yield += grp->ExpectedYield();
   }
   PrintHline(file);
   PrintSimpleCount(file,"Bkg. Total" , exp_yield );
   PrintHline(file);
   PrintSimpleCount(file,"Data",obs_yield);

   CloseTableFile(file);
}

extern void SummaryMCLumi( const GroupList& sig_list, const GroupList& bkg_list )
{
   FILE* file = OpenLumiFile("lumi");
   for( const auto& sig : sig_list ){ PrintLumiLine(file,sig->Sample()); }

   for( const auto& grp : bkg_list ){
      PrintHline(file);
      for( const auto& smp : grp->SampleList() ){ PrintLumiLine(file,smp); }
   }

   CloseTableFile(file);
}


//------------------------------------------------------------------------------
//   Helper functions
//------------------------------------------------------------------------------
static const char selec_line[] = "%-55s & %25s & %35s & %10s & %35s \\\\ \n" ;
static const char hline_line[] = "\\hline\n";
void PrintHline(FILE* file)
{
   fprintf( file, hline_line );
}

void CloseTableFile( FILE* file )
{
   fprintf( file, hline_line);
   fprintf( file , "\\end{tabular}\n");
   fclose( file );
}

FILE* OpenSelecFile( const string& tag )
{
   FILE* file = fopen( compare_namer.TexFileName("summary",{tag}).c_str() , "w" );
   fprintf( file, "\\begin{tabular}{|l|ccc|c|}\n");
   fprintf( file, hline_line );
   fprintf( file , selec_line ,
      "Sample" ,
      "Cross Section ($pb$)" ,
      "Selection Efficiency",
      "K Factor" ,
      "Expected Yield"
   );
   fprintf( file , hline_line );

   return file;
}

void PrintSampleLine( FILE* file, const SampleMgr* x )
{
   fprintf( file, selec_line,
      x->LatexName().c_str(),
      x->CrossSection().LatexFormat().c_str(),
      x->SelectionEfficiency().LatexFormat(2).c_str(),
      x->KFactor().LatexFormat().c_str(),
      x->ExpectedYield().LatexFormat(1).c_str()
   );
}

void PrintCount( FILE* file , const string& tag, const Parameter& x )
{
   fprintf( file, selec_line,
      tag.c_str(),
      "",
      "",
      "",
      x.LatexFormat(0).c_str()
   );
}

static const char simple_line[] = "%-30s & %35s &%35s\\\\ \n";

FILE* OpenSimpleFile( const string& tag )
{
   FILE* file = fopen( compare_namer.TexFileName("summary",{tag}).c_str() , "w" );
   fprintf( file, "\\begin{tabular}{|l|cc|}\n");
   fprintf( file, hline_line );
   fprintf( file , simple_line ,
      "Sample" ,
      "Cross section ($pb$)",
      "Expected Yield"
   );
   fprintf( file , hline_line );
   return file;
}

void PrintSimpleLine( FILE* file, const SampleGroup* x )
{
   fprintf( file , simple_line ,
      x->LatexName().c_str(),
      x->TotalCrossSection().LatexFormat(1).c_str(),
      x->ExpectedYield().LatexFormat(1).c_str()
   );
}

void PrintSimpleCount( FILE* file, const string& tag, const Parameter& x )
{
   fprintf( file , simple_line ,
      tag.c_str(),
      "",
      x.LatexFormat(1).c_str()
   );
}

static const char lumi_line[] = "%-55s %25s %10s %35s\\\\ \n";

FILE* OpenLumiFile( const string& tag )
{
   FILE* file = fopen( compare_namer.TexFileName("summary",{tag}).c_str() , "w" );
   fprintf( file, "\\begin{tabular}{|l|ccc|}\n");
   fprintf( file, hline_line );
   fprintf( file , lumi_line ,
      "Sample" ,
      "Cross section($pb$)",
      "K Factor",
      "Equiv. Luminosity ($pb^{-1}$)"
   );
   fprintf( file , hline_line );
   return file;
}

void PrintLumiLine( FILE* file , const SampleMgr* x )
{
   double equiv = x->OriginalEventCount();
   equiv /= x->CrossSection().CentralValue();
   equiv /= x->KFactor().CentralValue();
   fprintf( file , lumi_line ,
      x->LatexName().c_str(),
      x->CrossSection().LatexFormat().c_str(),
      x->KFactor().LatexFormat().c_str(),
      Parameter(equiv,0,0).LatexFormat().c_str()
   );
}

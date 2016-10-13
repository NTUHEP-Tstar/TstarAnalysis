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
InitGroupForTable( mgr::SampleGroup& group )
{
   for( const auto& sample : group.SampleList() ){
      InitSample( *sample );
      InitSampleForTable( *sample );
   }

   InitGroupString( group );
}


void
SummaryComplete(
   const vector<SampleGroup*>& siglist,
   const vector<SampleGroup*>& bkglist,
   const SampleGroup*          data )
{
   FILE* file = OpenSelecFile( "" );

   for( const auto& sig : siglist ){
      PrintSampleLine( file, sig->Sample() );
   }

   PrintHline( file );

   Parameter expyield( 0, 0, 0 );
   Parameter obsyield( data->EventsInFile(), 0, 0 );

   for( const auto& grp : bkglist ){
      for( const auto& smp : grp->SampleList() ){
         PrintSampleLine( file, smp.get() );
         expyield += smp->ExpectedYield() * GetWeightError( *smp );
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
   const vector<SampleGroup*>& siglist,
   const vector<SampleGroup*>& bkglist,
   const SampleGroup*          data )
{
   FILE* file = OpenSimpleFile( "brief" );
   Parameter expyield( 0, 0, 0 );
   Parameter obsyield( data->EventsInFile(), 0, 0 );

   for( const auto& grp : siglist ){
      PrintSimpleLine( file, grp );
   }

   PrintHline( file );

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
         PrintLumiLine( file, smp.get() );
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
   FILE* file = fopen( compare_namer.TexFileName( "summary", {tag} ).c_str(), "w" );
   fprintf( file, "\\begin{tabular}{|l|ccc|c|}\n" );
   PrintHline( file );
   fprintf( file, selecline,
      "Sample",
      "Cross Section ($pb$)",
      "Selection Efficiency",
      "Weight Errors",
      "Expected Yield"
      );
   PrintHline( file );

   return file;
}

/******************************************************************************/

void
PrintSampleLine( FILE* file, const SampleMgr* x )
{
   const string namecol = x->LatexName();
   const string xseccol = x->Name().find( "Tstar" ) != string::npos ?
                          Scientific( x->CrossSection(), 2 ) :
                          FloatingPoint( x->CrossSection(), -1 );
   const string effcol = Scientific( x->SelectionEfficiency()*x->KFactor(), 2 );

   const Parameter err = GetWeightError( *x );
   const string errcol = "+" + FloatingPoint( err.RelUpperError(), 3 )+ "/-" + FloatingPoint( err.RelLowerError(), 3 );

   const string expcol = FloatingPoint( x->ExpectedYield() * err, 0 );

   fprintf( file, selecline,
      namecol.c_str(),
      xseccol.c_str(),
      effcol.c_str(),
      errcol.c_str(),
      expcol.c_str()
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
static const char simpleline[] = "%-30s & %35s &%35s\\\\\n";

FILE*
OpenSimpleFile( const string& tag )
{
   FILE* file = fopen( compare_namer.TexFileName( "summary", {tag} ).c_str(), "w" );
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
PrintSimpleLine( FILE* file, const SampleGroup* x )
{
   Parameter expyield(0,0,0);
   for( const auto& smp : x->SampleList() ){
      expyield += smp->ExpectedYield() * GetWeightError( *smp );
   }

   fprintf( file, simpleline,
      x->LatexName().c_str(),
      Scientific( x->AvgSelectionEfficiency(), 2 ).c_str(),
      FloatingPoint( expyield, 0 ).c_str()
   );
}

/******************************************************************************/

void
PrintSimpleCount( FILE* file, const string& tag, const Parameter& x )
{
   fprintf( file, simpleline,
      tag.c_str(),
      "",
      FloatingPoint( x, 0 ).c_str()
   );
}

/*******************************************************************************
*   Helper functions for Lumi summary files
*******************************************************************************/
static const char lumi_line[] = "%-55s & %35s & %10s & %35s &  %20s\\\\\n";

FILE*
OpenLumiFile( const string& tag )
{
   FILE* file = fopen( compare_namer.TexFileName( "summary", {tag} ).c_str(), "w" );
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
PrintLumiLine( FILE* file, const SampleMgr* x )
{
   double equiv = GetOriginalEventCount( *x );
   equiv /= x->ExpectedYield();
   equiv /= x->KFactor().CentralValue();

   const string namecol = x->LatexName();
   const string xsec    = x->Name().find( "Tstar" ) != string::npos ?
   Scientific( x->CrossSection(), 2 ) :
   FloatingPoint( x->CrossSection(), -1  );
   const string kfaccol = FloatingPoint( x->KFactor(), -1 );
   const string xseccol = str( boost::format( "%s(%s)" )%xsec%x->GetCacheString( "XsecSource" ) );
   const string lumicol = FloatingPoint( Parameter( equiv, 0, 0 ), 2 ).c_str();
   const string gencol  = x->GetCacheString( "Generator" );

   fprintf(
      file, lumi_line,
      namecol.c_str(),
      xseccol.c_str(),
      kfaccol.c_str(),
      lumicol.c_str(),
      gencol.c_str()
   );
}

/*******************************************************************************
*   Helper functions for string parsing
*******************************************************************************/
void
InitSampleForTable( mgr::SampleMgr& sample )
{
   double eventweightsum     = 0;
   double eventweightupsum   = 0;
   double eventweightdownsum = 0;

   double btagweightsum   = 0;
   double puweightsum     = 0;
   double leptonweightsum = 0;
   double topptweightsum  = 0;
   double pdfweightsum    = 0;


   const auto& pdfidgroup = GetPdfIdGrouping( sample );

   fwlite::Handle<std::vector<pat::Jet> > jethandle;

   // counter
   unsigned evtcounter = 1;

   for( sample.Event().toBegin(); !sample.Event().atEnd(); ++sample.Event(), ++evtcounter ){
      const double btagweight     = GetBtagWeight( sample.Event() );
      const double btagweightup   = GetBtagWeightUp( sample.Event() );
      const double btagweightdown = GetBtagWeightDown( sample.Event() );

      const double puweight     = GetPileupWeight( sample.Event() );
      const double puweightup   = GetPileupWeightXsecup( sample.Event() );
      const double puweightdown = GetPileupWeightXsecdown( sample.Event() );

      const double leptonweight     = GetElectronWeight( sample.Event() ) * GetMuonWeight( sample.Event() );
      const double leptonweightup   = GetElectronWeightUp( sample.Event() ) * GetMuonWeightUp( sample.Event() );
      const double leptonweightdown = GetElectronWeightDown( sample.Event() ) * GetMuonWeightDown( sample.Event() );

      const double topptweight    = GetSampleEventTopPtWeight( sample, sample.Event() );
      const double pdfweighterr   = GetPdfWeightError( sample.Event(), pdfidgroup );
      const double scaleweighterr = GetScaleWeightError( sample.Event(), pdfidgroup );

      printf(
         "\rSample %s, Event %u/%llu...(%lf)",
         sample.Name().c_str(),
         evtcounter,
         sample.Event().size(),
         ( leptonweight*btagweight*puweight/GetEventWeight( sample.Event() ) )
      );
      fflush( stdout );

      const double sign            = GetEventWeight( sample.Event() ) > 0 ? 1 : -1;
      const double eventweight     = GetEventWeight( sample.Event() ) * topptweight;
      const double eventweightup   = btagweightup * puweightup * leptonweightup * topptweight * ( 1 + pdfweighterr ) * ( 1+scaleweighterr ) * sign;
      const double eventweightdown = btagweightdown* puweightdown * leptonweightdown * topptweight * ( 1 - pdfweighterr ) * ( 1-scaleweighterr )* sign;

      eventweightsum     += eventweight;
      eventweightupsum   += eventweightup;
      eventweightdownsum += eventweightdown;

      btagweightsum   += btagweight;
      puweightsum     += puweight;
      leptonweightsum += leptonweight;
      topptweightsum  += topptweight;
      pdfweightsum    += 1 + pdfweighterr;
   }

   evtcounter--;
   printf(
      " Done! Saving to event cache! AVG weight: btag:%lf, pu:%lf, lepton:%lf , toppt:%lf,  pdf:%lf\n",
      btagweightsum / evtcounter,
      puweightsum / evtcounter,
      leptonweightsum / evtcounter,
      topptweightsum / evtcounter,
      pdfweightsum / evtcounter
   );
   fflush( stdout );

   sample.AddCacheDouble( "EventWeightSum",     eventweightsum );
   sample.AddCacheDouble( "EventWeightUpSum",   eventweightupsum );
   sample.AddCacheDouble( "EventWeightDownSum", eventweightdownsum );
}

void
InitGroupString( mgr::SampleGroup& grp  )
{
   using namespace std;
   const mgr::ConfigReader grpcfg( compare_namer.MasterConfigFile() );

   if( !grpcfg.HasInstance( grp.Name() ) ){
      // Not found in config file, assuming it is single sample in default
      const string jsonfile = grpcfg.GetStaticString( "Default Json" );
      const string fullpath = mgr::SampleGroup::SampleCfgPrefix() + jsonfile;
      const mgr::ConfigReader samplecfg( fullpath );

      InitSampleString( *( grp.Sample() ), samplecfg );

   } else if( grpcfg.HasTag( grp.Name(), "Sample List" ) ){
      const string jsonfile
      = grpcfg.HasTag( grp.Name(), "Subset Json" ) ?
      grpcfg.GetString( grp.Name(), "Subset Json" ) :
      grpcfg.GetStaticString( "Default Json" );
      const string fullpath = mgr::SampleGroup::SampleCfgPrefix() + jsonfile;
      const mgr::ConfigReader samplecfg( fullpath );

      for( const auto& name : grpcfg.GetStringList( grp.Name(), "Sample List" ) ){
         InitSampleString( *( grp.Sample( name ) ), samplecfg );
      }
   } else if( grpcfg.HasTag( grp.Name(), "File List" ) ){

      for( const auto& json : grpcfg.GetStringList( grp.Name(), "File List" ) ){
         const mgr::ConfigReader samplecfg( mgr::SampleGroup::SampleCfgPrefix() + json );

         for( const auto& sampletag : samplecfg.GetInstanceList() ){
            InitSampleString( *( grp.Sample( sampletag ) ), samplecfg );
         }
      }
   } else if( grpcfg.HasTag( grp.Name(), "Single Sample" ) ){
      const string jsonfile = grpcfg.GetString( grp.Name(), "Single Sample" );
      const string fullpath = mgr::SampleGroup::SampleCfgPrefix() + jsonfile;
      const mgr::ConfigReader samplecfg( fullpath );
      InitSampleString( *( grp.Sample() ), samplecfg );
   }
}


void
InitSampleString( mgr::SampleMgr& sample, const mgr::ConfigReader& cfg )
{
   if( cfg.HasTag( sample.Name(), "Generator" ) ){
      sample.AddCacheString( "Generator", cfg.GetString( sample.Name(), "Generator" ) );
   }
   if( cfg.HasTag( sample.Name(), "Cross Section Source" ) ){
      sample.AddCacheString( "XsecSource", cfg.GetString( sample.Name(), "Cross Section Source" ) );
   }
}

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

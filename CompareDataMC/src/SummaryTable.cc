/*******************************************************************************
*
*  Filename    : SummaryTable.cc
*  Description : Generating latex summary tables
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/Maths/interface/Parameter.hpp"
#include "ManagerUtils/SampleMgr/interface/MultiFile.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleGroup.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/CompareDataMC/interface/MakeTable.hpp"

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include <boost/format.hpp>
#include <fstream>
#include <vector>

using namespace std;
using namespace mgr;
/*******************************************************************************
*   Static string and format settings
*******************************************************************************/
static const string hlinestr = "\\hline";
static boost::format samplelinefmt( "%-45s & %25s & %35s & %15s & %35s \\\\" );
static boost::format simplelinefmt( "%-40s & %25s &%20s\\\\" );
static boost::format lumilinefmt(   "%-55s & %35s & %10s & %35s &  %20s\\\\" );

/*******************************************************************************
*   Main control functions
*******************************************************************************/
void
SummaryComplete(
  const vector<SampleTableMgr>& siglist,
  const vector<SampleTableMgr>& bkglist,
  const SampleTableMgr&         data )
{
  ofstream selfile( compnamer.TexFileName( "summary" ) );
  selfile << "\\begin{tabular}{|l|ccc|c|}" << endl
          << hlinestr << endl
          << samplelinefmt
    % "Sample"
    % "Cross Section (pb)"
    % "Selection Efficiency"
    % "Weight Errors"
    % "Expected Yield"
          << endl
          << hlinestr << endl;

  for( const auto& sig : siglist ){
    selfile << SampleLine( sig.Sample() ) << endl;
  }

  selfile << hlinestr << endl;

  Parameter expyield( 0, 0, 0 );
  Parameter obsyield( data.ExpectedYield(), 0, 0 );

  for( const auto& grp : bkglist ){
    for( const auto& smp : grp.SampleList() ){
      selfile << SampleLine( smp ) << endl;
      const Parameter smpyield = smp.ExpectedYield()// double
                                 * Prod( smp.CrossSection().NormParam()// 1+- rel error
                                       , smp.SelectionEfficiency().NormParam()
                                       , GetWeightError( smp )
        );
      expyield += smpyield;
    }

    selfile << hlinestr << endl;
  }

  selfile << CountLine( "MC background", expyield ) << endl
          << hlinestr << endl
          << CountLine( "Data", obsyield ) << endl
          <<  hlinestr << endl
          << "\\end{tabular}" << endl;
  selfile.close();
}

/******************************************************************************/

void
SummaryBrief(
  const vector<SampleTableMgr>& siglist,
  const vector<SampleTableMgr>& bkglist,
  const SampleTableMgr&         data )
{
  ofstream brieffile( compnamer.TexFileName( "summary", "brief" ) );

  brieffile << "\\begin{tabular}{|l|cc|c|}" << endl
            << hlinestr << endl
            << simplelinefmt  % "Process" % "Average Efficiency" % "Expected Yield" << endl
            << hlinestr << endl;


  double expyield = 0;
  double obsyield = data.ExpectedYield();

  for( const auto& grp : siglist ){
    brieffile << SimpleLine( grp ) << endl;
  }

  brieffile << hlinestr << endl;

  for( const auto& grp : bkglist ){
    brieffile << SimpleLine( grp ) << endl;
    expyield += grp.ExpectedYield();
  }

  brieffile << hlinestr << endl
            << SimpleCount( "Bkg. Total", Parameter( expyield, 0, 0 ) ) << endl
            << hlinestr << endl
            << SimpleCount( "Data", Parameter( obsyield, 0, 0 ) ) << endl
            << hlinestr << endl
            << "\\end{tabular}" << endl;

  brieffile.close();
}

/******************************************************************************/

void
SummaryMCLumi(
  const vector<SampleTableMgr>& siglist,
  const vector<SampleTableMgr>& bkglist )
{
  ofstream lumifile( compnamer.TexFileName( "summary", "lumi" ) );

  lumifile <<  "\\begin{tabular}{|l|ccc|c|}" << endl
           << hlinestr << endl
           << lumilinefmt
    % "Process"
    % "Cross section ($pb$)"
    % "K Factor"
    % "Equiv. \\Lumi (\\pbinv)"
    % "Generator"
           << endl
           << hlinestr << endl;

  for( const auto& sig : siglist ){
    lumifile << LumiLine( sig.Sample() );
  }

  for( const auto& grp : bkglist ){
    lumifile << hlinestr << endl;

    for( const auto& smp : grp.SampleList() ){
      lumifile << LumiLine( smp ) << endl;
    }
  }

  lumifile << hlinestr << endl
           << "\\end{tabular}" << endl;

  lumifile.close();
}

/******************************************************************************/

string
SampleLine( const SampleMgr& x )
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
                                   , err.NormParam()
    );

  const string expcol =
    expyield.CentralValue() > 100 ?  FloatingPoint( expyield.CentralValue(), 0 ) :
    FloatingPoint( expyield.CentralValue(), 1 );

  return boost::str( samplelinefmt
    % namecol
    % AddMathBrace( xseccol )
    % AddMathBrace( effcol )
    % errcol
    % AddMathBrace( expcol )
    );
}

/******************************************************************************/

string
CountLine( const string& tag, const Parameter& x )
{
  return boost::str( samplelinefmt
    % tag.c_str()
    % ""
    % ""
    % ""
    % AddMathBrace( FloatingPoint( x, 0 ) )
    );
}

/*******************************************************************************
*   Helper functions for simplified summary
*******************************************************************************/

string
SimpleLine( const SampleTableMgr& x )
{
  Parameter expyield( 0, 0, 0 );

  for( const auto& smp : x.SampleList() ){
    expyield += smp.ExpectedYield()
                * smp.CrossSection().NormParam()
                * smp.SelectionEfficiency().NormParam()
                * GetWeightError( smp );
  }

  return boost::str( simplelinefmt
    % x.LatexName()
    % AddMathBrace( Scientific( x.AvgSelectionEfficiency().CentralValue(), 2 ) )
    % AddMathBrace( FloatingPoint( expyield.CentralValue(), 0 ) )
    );
}

/******************************************************************************/

string
SimpleCount( const string& tag, const Parameter& x )
{
  return boost::str( simplelinefmt
    % tag.c_str()
    % ""
    % AddMathBrace( FloatingPoint( x, 0 ) )
    );
}

/*******************************************************************************
*   Helper functions for Lumi summary files
*******************************************************************************/
string
LumiLine( const SampleMgr& x )
{
  double equiv = x.EffectiveLuminosity();

  const string namecol = x.LatexName();
  const string xsec    = x.Name().find( "Tstar" ) != string::npos ?
                         Scientific( x.CrossSection(), 2 ) :
                         XSecStr( x.CrossSection() );
  const string kfaccol = FloatingPoint( x.KFactor(), -1 );
  const string xseccol = str( boost::format( "%s(%s)" )%AddMathBrace( xsec )%x.GetCacheString( "XsecSource" ) );
  const string lumicol = Scientific( Parameter( equiv, 0, 0 ), 2 );
  const string gencol  = x.GetCacheString( "Generator" );

  return boost::str(
    lumilinefmt
    % namecol
    % xseccol
    % kfaccol
    % AddMathBrace( lumicol )
    % gencol
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

  err  = err.NormParam();
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

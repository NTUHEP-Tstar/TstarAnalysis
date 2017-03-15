/*******************************************************************************
*
*  Filename    : CompareHistMgr.cc
*  Description : Implementation of comparison manager
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/MassRecoCompare/interface/CompareHistMgr.hpp"

#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/MassRecoCompare/interface/Common.hpp"

#include <boost/format.hpp>
#include <iostream>

using namespace std;
using namespace tstar;
using namespace mgr;
using fwlite::Event;
using fwlite::Handle;

/*******************************************************************************
*   Main control flows
*******************************************************************************/
void
CompareHistMgr::define_hist()
{
  AddHist( "TstarMass",    "Reconstructed t* mass",           "GeV/c^{2}",  60,     0, 3000 );
  AddHist( "LepTstarMass", "Reconstructed leptonic t* mass",  "GeV/c^{2}",  60,     0, 3000 );
  AddHist( "HadTstarMass", "Reconstructed hadronic t* mass",  "GeV/c^{2}",  60,     0, 3000 );
  AddHist( "ChiSq",        "#chi^{2} of method",              "",           60,     0,  100 );
  AddHist( "LepTopMass",   "Reconstructed leptonic top mass", "GeV/c^{2}", 100,     0,  500 );
  AddHist( "HadTopMass",   "Reconstructed hadronic top mass", "GeV/c^{2}", 100,     0,  500 );
  AddHist( "HadWMass",     "Reconstructed hadronic W mass",   "GeV/c^{2}",  40,     0,  200 );
  AddHist( "TstarDiff",    "Difference in t* masses",         "GeV/c^{2}", 200, -1000, 1000 );
  AddHist( "LepTopDiff",   "Difference in leptonic top mass", "GeV/c^{2}", 100,  -200,  200 );
  AddHist( "HadTopDiff",   "Difference in hadronic top mass", "GeV/c^{2}", 100,  -200,  200 );
  AddHist( "HadWDiff",     "Difference in hadronic W mass",   "GeV/c^{2}", 100,  -200,  200 );

  AddHist2D(
    "JetMatchTotal",
    "Flavour from fit",
    "Flavour from MC Truth",
    5, 0, 5,// Xaxis  fitted result
    6, 0, 6// Yaxis  mc truth particle (must include unknown)
    );
  AddHist2D(
    "JetMatchPass",
    "Flavour from fit",
    "Flavour from MC Truth",
    5, 0, 5,// Xaxis  fitted result
    6, 0, 6// Yaxis  mc truth particle (must include unknown)
    );

  AddHist( "CorrPermTotal", "Number of correctly identified jets", "", 7, 0, 7 );
  AddHist( "CorrPermPass",  "Number of correctly identified jets", "", 7, 0, 7 );

  AddCorrPermMassHist( "TstarMass",  "Reco. t* Mass",               "GeV/c^{2}",  60, 0, 3000 );
  AddCorrPermMassHist( "ChiSq",      "Reco. #chi^{2} of Method",    "",           60, 0,  100 );
  AddCorrPermMassHist( "LepTopMass", "Reco. Leptonic Top Mass",     "GeV/c^{2}", 100, 0,  500 );
  AddCorrPermMassHist( "HadTopMass", "Reco. Hadronic Top Mass",     "GeV/c^{2}", 100, 0,  500 );
  AddCorrPermMassHist( "HadWMass",   "Reco. Hadronic W Boson Mass", "GeV/c^{2}",  40, 0,  200 );

}

/******************************************************************************/

void
CompareHistMgr::AddEvent( const fwlite::Event& ev )
{
  const string name        = Name();
  const double eventweight = GetEventWeight( ev );

  _resulthandle.getByLabel( ev, name.c_str(), LabelName( name ).c_str(), ProcessName().c_str() );

  if( !_resulthandle.isValid() ){
    cerr << "Warning! label: [" << Name() << "] gave invalid handle" << flush;
    return;
  }

  if( _resulthandle.ref().ChiSquare() < 0 ){ return; }// Early exit for unphysical results

  // Filling in basic histograms
  const double tstarmass    = _resulthandle.ref().TstarMass();
  const double leptstarmass = _resulthandle.ref().LeptonicTstar().M();
  const double hadtstarmass = _resulthandle.ref().HadronicTstar().M();
  const double leptopmass   = _resulthandle.ref().LeptonicTop().M();
  const double hadtopmass   = _resulthandle.ref().HadronicTop().M();
  const double hadwmass     = _resulthandle.ref().HadronicW().M();
  Hist( "TstarMass"     )->Fill( tstarmass, eventweight );
  Hist( "LepTstarMass"  )->Fill( leptstarmass, eventweight );
  Hist( "HadTstarMass"  )->Fill( hadtstarmass, eventweight );
  Hist( "ChiSq"         )->Fill( _resulthandle.ref().ChiSquare(), eventweight );
  Hist( "LepTopMass"    )->Fill( leptopmass, eventweight );
  Hist( "HadTopMass"    )->Fill( hadtopmass, eventweight );
  Hist( "HadWMass"      )->Fill( hadwmass, eventweight );
  Hist( "TstarDiff"     )->Fill( leptstarmass-hadtopmass, eventweight );
  Hist( "LepTopDiff"    )->Fill( leptopmass - 173.34, eventweight );
  Hist( "HadTopDiff"    )->Fill( hadtopmass - 173.34, eventweight );
  Hist( "HadWDiff"      )->Fill( hadwmass - 80.385, eventweight );

  // Filling 2d histogram for truth correctness
  for( const auto x : fitlabellist ){
    const int xval = GetBinPosition( x );

    for( const auto y : truthlabellist ){
      const int yval = GetBinPosition( y );
      Hist2D( "JetMatchTotal" )->Fill( xval, yval, eventweight );// Master fills all the bin according to weight
    }

    const int yval = GetBinPosition( _resulthandle.ref().GetParticle( x ).TypeFromTruth() );
    Hist2D( "JetMatchPass" )->Fill( xval, yval, eventweight );
  }

  const unsigned corr = NumCorrectAssign( _resulthandle.ref() );

  for( int i = 0; i <= 6; ++i ){
    Hist( "CorrPermTotal" )->Fill( i, eventweight );
  }

  Hist( "CorrPermPass" )->Fill( corr, eventweight );

  Hist( CorrPermMassHistName( "TstarMass", corr ) )->Fill( _resulthandle.ref().TstarMass(), eventweight );
  Hist( CorrPermMassHistName( "ChiSq", corr ) )->Fill( _resulthandle.ref().ChiSquare(), eventweight );
  Hist( CorrPermMassHistName( "LepTopMass", corr ) )->Fill( _resulthandle.ref().LeptonicTop().M(), eventweight );
  Hist( CorrPermMassHistName( "HadTopMass", corr ) )->Fill( _resulthandle.ref().HadronicTop().M(), eventweight );
  Hist( CorrPermMassHistName( "HadWMass", corr ) )->Fill( _resulthandle.ref().HadronicW().M(), eventweight );
}

/******************************************************************************/

int
CompareHistMgr::GetBinPosition( Particle_Label x )
{
  if( x == lepb_label ){
    return 0;
  } else if( x == lepg_label ){
    return 1;
  } else if( x == hadw1_label || x == hadw2_label ){
    return 2;
  } else if( x == hadb_label ){
    return 3;
  } else if( x == hadg_label ){
    return 4;
  } else {
    return 5;
  }
}

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
CompareHistMgr::CompareHistMgr( const string& name, const string& latex_name ) :
  Named( name ),
  HistMgr( name ),
  Hist2DMgr( name )
{
  SetLatexName( latex_name );
  SetFillStyle( 0 );
  define_hist();
}

/******************************************************************************/

CompareHistMgr::~CompareHistMgr()
{
}

/******************************************************************************/
string
CompareHistMgr::CorrPermMassHistName(
  const string& name,
  const int     numcorrperm
  )
{
  return str( boost::format( "%s%d" ) % name % numcorrperm );
}

/******************************************************************************/

void
CompareHistMgr::AddCorrPermMassHist(
  const string&  name,
  const string&  xtitle,
  const string&  unit,
  const unsigned bincount,
  const double   xmin,
  const double   xmax
  )
{
  for( int i = 0; i <= 6; ++i ){
    const string newname = CorrPermMassHistName( name, i );
    AddHist( newname, xtitle, unit, bincount, xmin, xmax );
  }
}

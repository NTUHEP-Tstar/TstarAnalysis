/*******************************************************************************
*
*  Filename    : ScaleHistMgr.cc
*  Description : Implementation for sample Mgr with histograms
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Handle.h"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"
#include "TstarAnalysis/TstarMassReco/interface/ScaleHistMgr.hpp"

#include "TFile.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
using mgr::SampleMgr;
using mgr::HistMgr;
using mgr::ConfigReader;

// ------------------------------------------------------------------------------
//   Main control flow
// ------------------------------------------------------------------------------
void
ScaleHistMgr::define_hist()
{
   //       Short tag        , X axis name, X axis unit, bin, min, max
   AddHist( "TstarMass",        "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_resup",  "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_resdw",  "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_jecup",  "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_jecdw",  "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_btagup", "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_btagdw", "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_puup",   "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_pudw",   "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_elup",   "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
   AddHist( "TstarMass_eldw",   "M_{t+g}", "GeV/c^{2}", 53, 350, 3000  );
}

void
ScaleHistMgr::fill_histograms( SampleMgr& sample )
{
   fwlite::Handle<RecoResult> chisqHandle;

   unsigned i = 0;

   for( const auto& filename : sample.GlobbedFileList() ){
      fwlite::Event ev( TFile::Open( filename.c_str() ) );

      for( ev.toBegin(); !ev.atEnd(); ++ev ){
         printf(
            "\rSample [%s|%s], Event[%u/%llu]...",
            Name().c_str(),
            sample.Name().c_str(),
            ++i,
            sample.Event().size()
            );
         fflush( stdout );

         chisqHandle.getByLabel( ev, "tstarMassReco", "ChiSquareResult", "TstarMassReco" );
         const double weight         = GetEventWeight( ev );
         const double btagweight     = GetBtagWeight( ev );
         const double btagweightup   = GetBtagWeightUp( ev );
         const double btagweightdown = GetBtagWeightDown( ev );
         const double puweight       = GetPileupWeight( ev );
         const double puweightup     = GetPileupWeight71260( ev );
         const double puweightdown   = GetPileupWeight62000( ev );
         const double elecweight     = GetElectronWeight( ev );
         const double elecweightup   = GetElectronWeightUp( ev );
         const double elecweightdown = GetElectronWeightDown( ev );

         if( chisqHandle->ChiSquare() > 0 ){// Only storing physical results
            // Central value plot
            Hist( "TstarMass" )->Fill( chisqHandle->TstarMass(), weight );

            // Jet corrections and resolution
            Hist( "TstarMass_resup" )->Fill( chisqHandle->ComputeFromPaticleList( tstar::res_up    ), weight );
            Hist( "TstarMass_resdw" )->Fill( chisqHandle->ComputeFromPaticleList( tstar::res_down  ), weight );
            Hist( "TstarMass_jecup" )->Fill( chisqHandle->ComputeFromPaticleList( tstar::corr_up   ), weight );
            Hist( "TstarMass_jecdw" )->Fill( chisqHandle->ComputeFromPaticleList( tstar::corr_down ), weight );

            // btag effects
            Hist( "TstarMass_btagup" )->Fill( chisqHandle->TstarMass(), weight * btagweightup / btagweight );
            Hist( "TstarMass_btagdw" )->Fill( chisqHandle->TstarMass(), weight * btagweightdown / btagweight );

            // Pile up effects
            Hist( "TstarMass_puup")->Fill( chisqHandle->TstarMass(), weight * puweightup / puweight );
            Hist( "TstarMass_pudw")->Fill( chisqHandle->TstarMass(), weight * puweightdown / puweight );

            // Electron Weight effects
            Hist( "TstarMass_elup")->Fill( chisqHandle->TstarMass(), weight * elecweightup / elecweight );
            Hist( "TstarMass_eldw")->Fill( chisqHandle->TstarMass(), weight * elecweightdown / elecweight );
         }
      }
   }

   cout << "Done!" << endl;
}


// ------------------------------------------------------------------------------
//   Constructor and desctructor
// ------------------------------------------------------------------------------
ScaleHistMgr::ScaleHistMgr( const string& name, const ConfigReader& cfg ) :
   Named( name ),
   SampleGroup( name, cfg  ),
   HistMgr( name )
{
   define_hist();

   for( auto& sample : SampleList() ){
      // Caching sample wide variables, see Common/src/InitSample.cc
      InitSample( *sample );

      // See above
      fill_histograms( *sample );
   }
}

ScaleHistMgr::~ScaleHistMgr(){}

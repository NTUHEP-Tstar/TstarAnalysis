/*******************************************************************************
 *
 *  Filename    : ScaleHistMgr.cc
 *  Description : Implementation for sample Mgr with histograms
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/ScaleHistMgr.hpp"
#include "DataFormats/FWLite/interface/Handle.h"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include <iostream>
#include <stdlib.h>
using namespace std;
using mgr::SampleMgr;
using mgr::HistMgr;
using mgr::ConfigReader;

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void ScaleHistMgr::define_hist()
{
   //       Short tag         , X axis name , X axis unit , bin, min, max
   AddHist( "TstarMass"       , "M_{t+g}"   , "GeV/c^{2}" , 50 , 0  , 50000  );
   AddHist( "TstarMass_resup" , "M_{t+g}"   , "GeV/c^{2}" , 50 , 0  , 50000  );
   AddHist( "TstarMass_resdw" , "M_{t+g}"   , "GeV/c^{2}" , 50 , 0  , 50000  );
   AddHist( "TstarMass_jecup" , "M_{t+g}"   , "GeV/c^{2}" , 50 , 0  , 50000  );
   AddHist( "TstarMass_jecdw" , "M_{t+g}"   , "GeV/c^{2}" , 50 , 0  , 50000  );
}

void ScaleHistMgr::fill_histograms( SampleMgr& sample )
{
   fwlite::Handle<RecoResult>            chisqHandle;

   unsigned i = 0;
   for( sample.Event().toBegin() ; !sample.Event().atEnd() ; ++sample.Event() ){
      printf(
         "\rSample [%s|%s], Event[%u/%llu]..." ,
         Name().c_str(),
         sample.Name().c_str(),
         ++i ,
         sample.Event().size()
      );
      fflush(stdout);

      chisqHandle.getByLabel( sample.Event(), "tstarMassReco" , "ChiSquareResult" , "TstarMassReco" );

      if( chisqHandle->ChiSquare() > 0 ){ // Only storing physical results
         Hist("TstarMass" )->Fill( chisqHandle->TstarMass()  );
         Hist("TstarMass_resup")->Fill( chisqHandle->ComputeFromPaticleList( tstar::res_up    )  );
         Hist("TstarMass_resdw")->Fill( chisqHandle->ComputeFromPaticleList( tstar::res_down  )  );
         Hist("TstarMass_jecup")->Fill( chisqHandle->ComputeFromPaticleList( tstar::corr_up   )  );
         Hist("TstarMass_jecdw")->Fill( chisqHandle->ComputeFromPaticleList( tstar::corr_down )  );
      }

   }
   cout << "Done!" << endl;
}


//------------------------------------------------------------------------------
//   Constructor and desctructor
//------------------------------------------------------------------------------
ScaleHistMgr::ScaleHistMgr( const string& name, const ConfigReader& cfg ):
   Named( name ),
   SampleGroup( name, cfg  ),
   HistMgr( name )
{
   define_hist();
   for( auto& sample : SampleList() ){
      fill_histograms(*sample);
   }
}

ScaleHistMgr::~ScaleHistMgr() {}

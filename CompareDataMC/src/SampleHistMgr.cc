/*******************************************************************************
 *
 *  Filename    : SampleHistMgr.cc
 *  Description : Implementation for sample Mgr with histograms
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

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
void SampleHistMgr::define_hist()
{
   //       Short tag   , X axis name                 , X axis unit , bin, min  , max
   AddHist( "LepPt"     , "Lepton p_{T}"              , "GeV/c"     , 48 , 20   , 500.  );
   AddHist( "LepEta"    , "Lepton #eta"               , ""          , 75 , -2.5 , 5.0   );
   AddHist( "Jet1Pt"    , "First Leading Jet p_{T}"   , "GeV/c"     , 60 , 30   , 1000. );
   AddHist( "Jet2Pt"    , "Second Leading Jet p_{T}"  , "GeV/c"     , 60 , 30   , 1000. );
   AddHist( "Jet1Eta"   , "First Leading Jet #eta"    , ""          , 75 , -2.5 , 5.0   );
   AddHist( "Jet2Eta"   , "Second Leading Jet #eta"   , ""          , 75 , -2.5 , 5.0   );
   AddHist( "JetNum"    , "Number of Jets"            , ""          , 7  , 5    , 12    );
   AddHist( "MET"       , "Missing p_{T}"             , "GeV"       , 50 , 0    , 500.  );
   AddHist( "METPhi"    , "Missing p_{T} #phi"        , ""          , 96 , -3.2 , 6.4   );
   AddHist( "Jet3Pt"    , "Third Jet p_{T}"           , "GeV/c"     , 60 , 30   , 700.  );
   AddHist( "Jet4Pt"    , "Fourth Jet p_{T}"          , "GeV/c"     , 60 , 30   , 600.  );
   AddHist( "Jet5Pt"    , "Fifth Jet p_{T}"           , "GeV/c"     , 60 , 30   , 400.  );
   AddHist( "Jet6Pt"    , "Sixth Jet p_{T}"           , "GeV/c"     , 60 , 30   , 400.  );
   AddHist( "TstarMass" , "M_{t+g}"                   , "GeV/c^{2}" , 50 , 0    , 2000  );
   AddHist( "TstarZoom" , "M_{t+g}"                   , "GeV/c^{2}" , 50 , 350  , 2500  );
   AddHist( "ChiSq"     , "#chi^{2}"                  , ""          , 50 , 0    , 10000 );
   AddHist( "VtxNum"    , "Number of primary vertex"  , ""          , 50 , 0    , 50    );
   AddHist( "VtxNumNW"  , "Number of primary vertex(no weighting)"  , ""          , 50 , 0    , 50    );
   AddHist( "LepGluonPt", "Leptonic Gluon Jet p_{T}"  , "GeV/c"     , 60 , 30   , 1000. );
   AddHist( "HadGluonPt", "Hadronic Gluon Jet p_{T}"  , "GeV/c"     , 60 , 30   , 1000. );
}

void SampleHistMgr::fill_histograms( SampleMgr& sample )
{
   fwlite::Handle<double>                weightHandle;
   fwlite::Handle<double>                puwHandle;
   fwlite::Handle<vector<pat::MET>>      metHandle;
   fwlite::Handle<vector<reco::Vertex>>  vtxHandle;
   fwlite::Handle<vector<pat::Jet>>      jetHandle;
   fwlite::Handle<vector<pat::Muon>>     muonHandle;
   fwlite::Handle<vector<pat::Electron>> electronHandle;
   fwlite::Handle<LHEEventProduct>       lheHandle;
   fwlite::Handle<RecoResult>            chisqHandle;

   double weight_sum = 0 ;
   unsigned i = 1;
   // Looping over events
   for( sample.Event().toBegin() ; !sample.Event().atEnd() ; ++sample.Event() , ++i ){
      printf(
         "\rSample [%s|%s], Event[%u/%llu]..." ,
         Name().c_str(),
         sample.Name().c_str(),
         i ,
         sample.Event().size()
      );
      fflush(stdout);

      metHandle.getByLabel     ( sample.Event(), "slimmedMETs"    );
      vtxHandle.getByLabel     ( sample.Event(), "offlineSlimmedPrimaryVertices" );
      jetHandle.getByLabel     ( sample.Event(), "skimmedPatJets" );
      muonHandle.getByLabel    ( sample.Event(), "skimmedPatMuons" );
      electronHandle.getByLabel( sample.Event(), "skimmedPatElectrons" );
      chisqHandle.getByLabel   ( sample.Event(), "tstarMassReco" , "ChiSquareResult" , "TstarMassReco" );

      double total_weight = 1. ;
      double pileup_weight = 1.;
      if( !sample.IsRealData() ){
         try{
            weightHandle.getByLabel( sample.Event() , "EventWeight", "EventWeight", "TstarMassReco" );
            puwHandle.getByLabel( sample.Event() , "EventWeight", "PileupWeight" , "TstarMassReco"  );

            if( *weightHandle < 2.0 && *weightHandle > -2.0 ){
               total_weight = *weightHandle;
            } else {
               fprintf( stderr , "Strange weight found! %lf\n", *weightHandle );
               fflush(stderr);
            }

            if( *puwHandle < 2.0 && *puwHandle > -2.0 && *puwHandle != 0. ){
               pileup_weight = *puwHandle;
            }
         } catch ( std::exception e ){

         }
      }

      weight_sum += total_weight;

      for( const auto& mu : *muonHandle.product() ){
         Hist("LepPt")->Fill( mu.pt() , total_weight );
         Hist("LepEta")->Fill( mu.eta() , total_weight );
      }
      for( const auto& el : *electronHandle.product() ){
         Hist("LepPt")->Fill( el.pt() , total_weight );
         Hist("LepEta")->Fill( el.eta(), total_weight );
      }

      Hist("JetNum")->Fill( jetHandle->size()       , total_weight );
      Hist("Jet1Pt")->Fill( jetHandle->at(0).pt()   , total_weight );
      Hist("Jet1Eta")->Fill( jetHandle->at(0).eta() , total_weight );
      Hist("Jet2Pt")->Fill( jetHandle->at(1).pt()   , total_weight );
      Hist("Jet2Eta")->Fill( jetHandle->at(1).eta() , total_weight );
      Hist("Jet3Pt")->Fill( jetHandle->at(2).pt()   , total_weight );
      Hist("Jet4Pt")->Fill( jetHandle->at(3).pt()   , total_weight );
      Hist("Jet5Pt")->Fill( jetHandle->at(4).pt()   , total_weight );
      Hist("Jet6Pt")->Fill( jetHandle->at(5).pt()   , total_weight );

      Hist("MET")->Fill( metHandle->front().pt()     , total_weight );
      Hist("METPhi")->Fill( metHandle->front().phi() , total_weight );
      Hist("VtxNum")->Fill( vtxHandle->size()        , total_weight );
      Hist("VtxNumNW")->Fill( vtxHandle->size()      , total_weight/pileup_weight );

      if( chisqHandle->ChiSquare() > 0 ){ // Only sotring physical results
         Hist("TstarMass" )->Fill( chisqHandle->TstarMass() , total_weight );
         Hist("ChiSq"     )->Fill( chisqHandle->ChiSquare() , total_weight );
         Hist("LepGluonPt")->Fill( chisqHandle->LeptonicGluon().Pt() , total_weight  );
         Hist("HadGluonPt")->Fill( chisqHandle->HadronicGluon().Pt() , total_weight  );

         if( chisqHandle->TstarMass() > 350 ){
            Hist("TstarZoom" )->Fill( chisqHandle->TstarMass() , total_weight );
         }
      }
   }
   cout << "Done!"
   << " Sum Weight:"  << weight_sum
   << " Event Count:" << sample.SelectedEventCount()
   << endl;

   // Weighting events according to expected yield
   const double expyeild = sample.ExpectedYield().CentralValue();
   const double scale    = expyeild / weight_sum;
   for( const auto& histname : AvailableHistList() ){
      Hist(histname)->Scale( scale );
   }
}


//------------------------------------------------------------------------------
//   Constructor and desctructor
//------------------------------------------------------------------------------
SampleHistMgr::SampleHistMgr( const string& name, const ConfigReader& cfg ):
Named( name ),
SampleGroup( name, cfg  ),
HistMgr( name )
{
   define_hist();
   for( auto& sample : SampleList() ){
      fill_histograms(*sample);
   }
}

SampleHistMgr::~SampleHistMgr() {}

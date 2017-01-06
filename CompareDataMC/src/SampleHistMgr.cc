/*******************************************************************************
*
*  Filename    : SampleHistMgr.cc
*  Description : Implementation for sample Mgr with histograms
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "TstarAnalysis/CompareDataMC/interface/SampleHistMgr.hpp"

#include "ManagerUtils/PhysUtils/interface/ObjectExtendedVars.hpp"
#include "ManagerUtils/SampleMgr/interface/MultiFile.hpp"
#include "ManagerUtils/SampleMgr/interface/SampleMgrLoader.hpp"
#include "TstarAnalysis/Common/interface/BTagChecker.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/CompareDataMC/interface/Compare_Common.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "TFile.h"
#include <boost/format.hpp>
#include <iostream>

using namespace std;
using mgr::SampleMgr;
using mgr::HistMgr;
using mgr::ConfigReader;


/******************************************************************************/

void
SampleHistMgr::define_hist()
{
   //       Short tag            , X axis name                                       , X axis unit,bin, min  , max
   AddHist( "LepPt",             "Lepton p_{T}",                                        "GeV/c",     48,  20,    500 );
   AddHist( "LepEta",            "Lepton #eta",                                         "",          75,   -2.5, 5.0 );
   AddHist( "MET",               "Missing transverse energy",                           "GeV",       50,   0,    500 );
   AddHist( "METPhi",            "Missing transverse energy #phi",                      "",          96,   -3.2, 6.4 );
   AddHist( "BJetNum",           "Number of b tagged jets",                             "",          11,   4,     15 );
   AddHist( "JetNum",            "Number of jets",                                      "",          15,   0,     14 );
   AddHist( "JetPt",             "Jet p_{T}",                                           "GeV/c",     60,   0,   1000 );
   AddHist( "Jet1Pt",            "First Leading Jet p_{T}",                             "GeV/c",     60,   0,   1000 );
   AddHist( "Jet2Pt",            "Second Leading Jet p_{T}",                            "GeV/c",     60,   0,   1000 );
   AddHist( "Jet3Pt",            "Third Jet p_{T}",                                     "GeV/c",     60,   0,    700 );
   AddHist( "Jet4Pt",            "Fourth Jet p_{T}",                                    "GeV/c",     60,   0,    600 );
   AddHist( "JetEta",            "Jet #eta",                                            "",          75,   -2.5, 5.0 );
   AddHist( "Jet1Eta",           "First Leading Jet #eta",                              "",          75,   -2.5, 5.0 );
   AddHist( "Jet2Eta",           "Second Leading Jet #eta",                             "",          75,   -2.5, 5.0 );

   // Tstar Variables
   AddHist( "TstarMass",         "M_{t+g}",                                             "GeV/c^{2}", 50,   0,   2000 );
   AddHist( "TstarZoom",         "M_{t+g}",                                             "GeV/c^{2}", 50, 350,   2500 );
   AddHist( "ChiSq",             "#chi^{2}",                                            "",          50,   0,  10000 );
   AddHist( "LepGluonPt",        "Leptonic Gluon Jet p_{T}",                            "GeV/c",     60,  30,  1000. );
   AddHist( "HadGluonPt",        "Hadronic Gluon Jet p_{T}",                            "GeV/c",     60,  30,  1000. );

   // Primary vertexts
   AddHist( "VtxNum",            "Number of primary vertex",                            "",          50,   0,     50 );
   AddHist( "VtxNumBestFit",     "Number of primary vertex (#sigma_{min}=62mb)",        "",          50,   0,     50 );
   AddHist( "VtxNumUp",          "Number of primary vertex (#sigma_{min}+4.6%)",        "",          50,   0,     50 );
   AddHist( "VtxNumDown",        "Number of primary vertex (#sigma_{min}-4.6%)",        "",          50,   0,     50 );
   AddHist( "VtxNumNW",          "Number of primary vertex (no PU weight)",             "",          50,   0,     50 );

   // Pile up effects
   AddHist( "Jet1Pt_PuUp",       "First Leading Jet p_{T} (#sigma_{min}+4.6%)",         "GeV/c",     60,  30,   1000 );
   AddHist( "Jet1Pt_PuDown",     "First Leading Jet p_{T} (#sigma_{min}-4.6%)",         "GeV/c",     60,  30,   1000 );
   AddHist( "JetNum_PuUp",       "Number of Jets (#sigma_{min}+4.6%)",                  "GeV/c^{2}", 11,   4,     15 );
   AddHist( "JetNum_PuDown",     "Number of Jets (#sigma_{min}-4.6%)",                  "GeV/c^{2}", 11,   4,     15 );
   AddHist( "TstarMass_PuUp",    "M_{t+g} (#sigma_{min}+4.6%)",                         "GeV/c^{2}", 50,   0,   2000 );
   AddHist( "TstarMass_PuDown",  "M_{t+g} (#sigma_{min}-4.6%)",                         "GeV/c^{2}", 50,   0,   2000 );

   // Effects of TopPtWeight
   AddHist( "MET_NoTopPt",       "Missing transverse momentum (w/o top p_{T} weights)", "GeV",       50,   0,    500 );
   AddHist( "JetNum_NoTopPt",    "Number of jets (w/o top p_{T} weights)",              "",          11,   4,     15 );
   AddHist( "Jet1Pt_NoTopPt",    "Leading Jet p_{T} (w/o top p_{T} weights)",           "GeV/c",     60,  30,   1000 );
   AddHist( "TstarMass_NoTopPt", "M_{t+g} (w/o top p_{T} weights)",                     "GeV/c^{2}", 50,   0,   2000 );

   // Effects of Tighter electron selection
   AddHist( "LepNonePt",         "selection lepton p_{T} (w/o lepton weights)",         "GeV/c",     48,  20,    500 );
   AddHist( "LepNoneEta",        "selection lepton #eta (w/o lepton weight)",           "",          75,   -2.5, 5.0 );
   AddHist( "LepPtGsf",          "Lepton p_{T} (RECO weight only)",                     "GeV/c",     48,  20,    500 );
   AddHist( "LepEtaGsf",         "Lepton #eta (RECO weight only)",                      "",          75,   -2.5, 5.0 );
   AddHist( "LepPtGT",           "Lepton p_{T} (RECO+Trigger weight only)",             "GeV/c",     48,  20,    500 );
   AddHist( "LepEtaGT",          "Lepton #eta (RECO+Trigger weight only)",              "",          75,   -2.5, 5.0 );
   AddHist( "LepPtGC",           "Lepton p_{T} (RECO+CUT weight only)",                 "GeV/c",     48,  20,    500 );
   AddHist( "LepEtaGC",          "Lepton #eta (RECO+CUT weight only)",                  "",          75,   -2.5, 5.0 );
   AddHist( "LepPtAll",          "Lepton p_{T} (RECO+CUT+Trigger weight)",              "GeV/c",     48,  20,    500 );
   AddHist( "LepEtaAll",         "Lepton #eta (RECO+CUT+Trigger weight)",               "",          75,   -2.5, 5.0 );

   // Muon Isolation factor
   AddHist( "MuPfIso",           "Muon P.F. Isolation",                                 "",         100,   0,    1.5 );

   AddHist( "Jet1Pt_NoLep",      "Leading Jet p_{T} (w/o lepton weight)",               "GeV/c",     60,  30,   1000 );
   AddHist( "TstarMass_NoLep",   "M_{t+g} (w/o lepton weight)",                         "GeV/c^{2}", 50,   0,   2000 );
}

/******************************************************************************/

void
SampleHistMgr::FillFromSample( SampleMgr& sample )
{
   fwlite::Handle<vector<pat::MET> > metHandle;
   fwlite::Handle<vector<reco::Vertex> > vtxHandle;
   fwlite::Handle<vector<pat::Jet> > jetHandle;
   fwlite::Handle<vector<pat::Muon> > muonHandle;
   fwlite::Handle<vector<pat::Electron> > electronHandle;
   fwlite::Handle<RecoResult> chisqHandle;

   const double sampleweight = sample.IsRealData() ?
                               1. : sample.CrossSection().CentralValue() / sample.OriginalEventCount();

   unsigned i = 1;
   boost::format processform( "\rSample [%s|%s] , Event[%u/%llu]..." );


   // Looping over events
   mgr::MultiFileEvent myevt( sample.GlobbedFileList() );
   cout << processform % Name() % sample.Name() % i % myevt.size() << flush;

   for( myevt.toBegin(); !myevt.atEnd(); ++myevt, ++i ){
      const auto& ev = myevt.Base();

      cout << processform % Name() % sample.Name() % i % myevt.size() << flush;

      const double pileup_weight    = GetPileupWeight( ev );
      const double pileup_weight_bf = GetPileupWeightBestFit( ev );
      const double puweight_up      = GetPileupWeightXsecup( ev );
      const double puweight_down    = GetPileupWeightXsecdown( ev );
      const double elecweight       = GetElectronWeight( ev );
      const double elecrecoweight   = GetElectronTrackWeight( ev );
      const double electrgweight    = GetElectronTriggerWeight( ev );
      const double eleccutweight    = GetElectronCutWeight( ev );
      const double muonweight       = GetMuonWeight( ev );
      const double btagweight       = GetBtagWeight( ev );
      const double topptweight      = GetSampleEventTopPtWeight( sample, ev );

      const double total_weight = GetEventWeight( ev ) * sampleweight * topptweight;

      metHandle.getByLabel( ev, "slimmedMETs"    );
      vtxHandle.getByLabel( ev, "offlineSlimmedPrimaryVertices" );
      jetHandle.getByLabel( ev, "skimmedPatJets" );
      muonHandle.getByLabel( ev, "skimmedPatMuons" );
      electronHandle.getByLabel( ev, "skimmedPatElectrons" );
      chisqHandle.getByLabel( ev, "tstarMassReco", "ChiSquareResult", "TstarMassReco" );

      Hist( "JetNum"  )->Fill( jetHandle->size(), total_weight );
      Hist( "Jet1Pt"  )->Fill( jetHandle->at( 0 ).pt(), total_weight );
      Hist( "Jet1Eta" )->Fill( jetHandle->at( 0 ).eta(), total_weight );
      Hist( "Jet2Pt"  )->Fill( jetHandle->at( 1 ).pt(), total_weight );
      Hist( "Jet2Eta" )->Fill( jetHandle->at( 1 ).eta(), total_weight );
      Hist( "Jet3Pt"  )->Fill( jetHandle->at( 2 ).pt(), total_weight );
      Hist( "Jet4Pt"  )->Fill( jetHandle->at( 3 ).pt(), total_weight );


      Hist( "JetNum_NoTopPt" )->Fill( jetHandle->size(), total_weight / topptweight );
      Hist( "Jet1Pt_NoTopPt" )->Fill( jetHandle->at( 0 ).pt(), total_weight / topptweight );

      Hist( "Jet1Pt_PuUp"   )->Fill( jetHandle->at( 0 ).pt(), total_weight/pileup_weight * puweight_up );
      Hist( "Jet1Pt_PuDown" )->Fill( jetHandle->at( 0 ).pt(), total_weight/pileup_weight * puweight_down );
      Hist( "JetNum_PuUp"   )->Fill( jetHandle->size(), total_weight / pileup_weight * puweight_up );
      Hist( "JetNum_PuDown" )->Fill( jetHandle->size(), total_weight / pileup_weight * puweight_down );

      Hist( "Jet1Pt_NoLep"  )->Fill( jetHandle->at( 0 ).pt(), total_weight / elecweight / muonweight );

      int bjetnum = 0;

      for( const auto& jet : jetHandle.ref() ){
         Hist( "JetPt"  )->Fill( jet.pt(), total_weight );
         Hist( "JetEta" )->Fill( jet.eta(), total_weight );
         if( jet.bDiscriminator( BTagChecker::DISCRIMTAG ) > BTagChecker::MEDIUMWP_VAL ){
            bjetnum++;
         }
      }

      Hist( "BJetNum" )->Fill( bjetnum, total_weight );

      for( const auto& mu : *muonHandle ){
         Hist( "LepPt"      )->Fill( mu.pt(), total_weight );
         Hist( "LepEta"     )->Fill( mu.eta(), total_weight );
         Hist( "MuPfIso"    )->Fill( MuPfIso(mu), total_weight );
         Hist( "LepNonePt"  )->Fill( mu.pt(), total_weight / muonweight );
         Hist( "LepNoneEta" )->Fill( mu.eta(), total_weight / muonweight );
      }

      for( const auto& el : *electronHandle ){
         Hist( "LepPt"      )->Fill( el.pt(),  total_weight );
         Hist( "LepEta"     )->Fill( el.eta(), total_weight );
         Hist( "LepNonePt"  )->Fill( el.pt(),  total_weight / elecweight );
         Hist( "LepNoneEta" )->Fill( el.eta(), total_weight / elecweight );
         Hist( "LepPtGsf"   )->Fill( el.pt(),  total_weight / elecweight * elecrecoweight );
         Hist( "LepEtaGsf"  )->Fill( el.eta(), total_weight / elecweight * elecrecoweight );
         Hist( "LepPtGT"    )->Fill( el.pt(),  total_weight / elecweight * elecrecoweight * electrgweight );
         Hist( "LepEtaGT"   )->Fill( el.eta(), total_weight / elecweight * elecrecoweight * electrgweight );
         Hist( "LepPtGC"    )->Fill( el.pt(),  total_weight / elecweight * elecrecoweight * eleccutweight );
         Hist( "LepEtaGC"   )->Fill( el.eta(), total_weight / elecweight * elecrecoweight * eleccutweight );
         Hist( "LepPtAll"   )->Fill( el.pt(),  total_weight / elecweight * elecrecoweight * eleccutweight * electrgweight );
         Hist( "LepEtaAll"  )->Fill( el.eta(), total_weight / elecweight * elecrecoweight * eleccutweight * electrgweight );
      }

      cout << pileup_weight << " "
           << elecweight << " "
           << muonweight << " "
           << topptweight << " "
           << electrgweight << " "
           << btagweight
           << flush;

      Hist( "MET" )->Fill( metHandle->front().pt(), total_weight );
      Hist( "METPhi" )->Fill( metHandle->front().phi(), total_weight );
      Hist( "MET_NoTopPt" )->Fill( metHandle->front().pt(), total_weight / topptweight );

      Hist( "VtxNum" )->Fill( vtxHandle->size(), total_weight );
      Hist( "VtxNumBestFit" )->Fill( vtxHandle->size(), total_weight / pileup_weight * pileup_weight_bf );
      Hist( "VtxNumNW" )->Fill( vtxHandle->size(), total_weight/pileup_weight );
      Hist( "VtxNumUp" )->Fill( vtxHandle->size(), total_weight/pileup_weight * puweight_up );
      Hist( "VtxNumDown" )->Fill( vtxHandle->size(), total_weight/pileup_weight * puweight_down );

      if( chisqHandle.isValid() && chisqHandle.ref().ChiSquare() > 0 ){
         // Potentially missing
         Hist( "TstarMass" )->Fill( chisqHandle->TstarMass(), total_weight );
         Hist( "ChiSq"     )->Fill( chisqHandle->ChiSquare(), total_weight );
         Hist( "LepGluonPt" )->Fill( chisqHandle->LeptonicGluon().Pt(), total_weight  );
         Hist( "HadGluonPt" )->Fill( chisqHandle->HadronicGluon().Pt(), total_weight  );

         Hist( "TstarMass_PuUp" )->Fill( chisqHandle->TstarMass(), total_weight/pileup_weight * puweight_up );
         Hist( "TstarMass_PuDown" )->Fill( chisqHandle->TstarMass(), total_weight/pileup_weight * puweight_down );
         Hist( "TstarMass_NoTopPt" )->Fill( chisqHandle->TstarMass(), total_weight / topptweight );
         Hist( "TstarMass_NoLep" )->Fill( chisqHandle->TstarMass(), total_weight / elecweight );

         if( chisqHandle->TstarMass() > 350 ){
            Hist( "TstarZoom" )->Fill( chisqHandle->TstarMass(), total_weight );
         }
      }
   }

   cout << "Done!" << endl;
}


/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
SampleHistMgr::SampleHistMgr( const string& name, const ConfigReader& cfg  ) :
   Named( name ),
   SampleGroup( name, cfg ),
   HistMgr( name )
{
   define_hist();
}


/******************************************************************************/

void
SampleHistMgr::FillFromSample()
{
   for( auto& sample : SampleList() ){
      InitSampleFromEDM( sample );
      mgr::SaveCacheToFile( sample, SampleCacheFile( sample ) );
      FillFromSample( sample );
   }

   SaveToFile( compnamer.CustomFileName( "root", {"histcache"} ) );
}

/******************************************************************************/

void
SampleHistMgr::LoadFromFile()
{
   for( auto& sample : SampleList() ){
      mgr::LoadCacheFromFile( sample, SampleCacheFile( sample ) );
   }

   // calling from histmgr function
   HistMgr::LoadFromFile( compnamer.CustomFileName( "root", {"histcache"} ) );
}

SampleHistMgr::~SampleHistMgr(){}

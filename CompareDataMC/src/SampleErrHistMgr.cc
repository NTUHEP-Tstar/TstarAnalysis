/*******************************************************************************
*
*  Filename    : SampleErrHistMgr.cc
*  Description : Filling functions for the error histograms
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "TstarAnalysis/CompareDataMC/interface/SampleErrHistMgr.hpp"

#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "TstarAnalysis/Common/interface/ComputeSelectionEff.hpp"
#include "TstarAnalysis/Common/interface/GetEventWeight.hpp"
#include "TstarAnalysis/Common/interface/InitSample.hpp"
#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "TFile.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
using mgr::SampleMgr;
using mgr::HistMgr;
using mgr::ConfigReader;

using namespace std;

extern const vector<string> histnamelist = {
   "LepPt",
   "LepEta",
   "JetNum",
   "Jet1Pt",
   "Jet1Eta",
   "Jet2Pt",
   "Jet2Eta",
   "MET",
   "TstarMass"
};

extern const vector<ErrorSource> histerrlist = {
   { "jec",    "Jet energy correction",    ""               },
   { "jer",    "Jet energy resolution",    ""               },
   { "pu",     "Pileup #sigma_{mini}",     "4.6%%"          },
   { "btag",   "b-tag scale factor",       ""               },
   { "lepton", "lepton scale factor",      ""               },
   { "pdf",    "PDF uncertainty",          ""               },
   { "scale",  "QCD scale variation",      ""               }
};


void
SampleErrHistMgr::define_hist()
{
   AddErrHists( "LepPt",     "Lepton p_{T}",             "GeV/c",     48, 20,    500 );
   AddErrHists( "LepEta",    "Lepton #eta",              "",          75,  -2.5, 5.0 );
   AddErrHists( "Jet1Pt",    "First Leading Jet p_{T}",  "GeV/c",     60, 30,   1000 );
   AddErrHists( "Jet1Eta",   "First Leading Jet #eta",   "",          75,  -2.5, 5.0 );
   AddErrHists( "Jet2Pt",    "Second Leading Jet p_{T}", "GeV/c",     60, 30,   1000 );
   AddErrHists( "Jet2Eta",   "Second Leading Jet #eta",  "",          75,  -2.5, 5.0 );
   AddErrHists( "JetNum",    "Number of Jets",           "",          10,  5,     15 );
   AddErrHists( "MET",       "Missing p_{T}",            "GeV",       50,  0,    500 );
   AddErrHists( "TstarMass", "M_{t+g}",                  "GeV/c^{2}", 50,  0,   3000 );
}

void
SampleErrHistMgr::fill_histograms( mgr::SampleMgr& sample )
{
   fwlite::Handle<vector<pat::MET> > metHandle;
   fwlite::Handle<vector<reco::Vertex> > vtxHandle;
   fwlite::Handle<vector<pat::Jet> > jetHandle;
   fwlite::Handle<vector<pat::Muon> > muonHandle;
   fwlite::Handle<vector<pat::Electron> > electronHandle;
   fwlite::Handle<LHEEventProduct> lheHandle;
   fwlite::Handle<RecoResult> chisqHandle;


   const auto& pdfidgroup = GetPdfIdGrouping( sample );
   const double sampleweight = GetSampleWeight( sample );

   unsigned i = 1;

   printf(
      "\rSample [%s|%s], Event[%u/%llu]...",
      Name().c_str(),
      sample.Name().c_str(),
      i,
      sample.Event().size()
      );
   fflush( stdout );

   // Looping over events
   for( const auto& file : sample.GlobbedFileList() ){
      fwlite::Event ev( TFile::Open( file.c_str() ) );

      for( ev.toBegin(); !ev.atEnd(); ++ev, ++i ){
         printf(
            "\rSample [%s|%s], Event[%u/%llu]...(%lf)",
            Name().c_str(),
            sample.Name().c_str(),
            i,
            sample.Event().size(),
            GetSampleEventTopPtWeight( sample, ev )
            );
         fflush( stdout );

         const double eventweight = GetEventWeight( ev );
         const double topptweight = GetSampleEventTopPtWeight( sample, ev );
         const double totalweight = sampleweight * eventweight * topptweight;

         metHandle.getByLabel( ev, "slimmedMETs"    );
         vtxHandle.getByLabel( ev, "offlineSlimmedPrimaryVertices" );
         jetHandle.getByLabel( ev, "skimmedPatJets" );
         muonHandle.getByLabel( ev, "skimmedPatMuons" );
         electronHandle.getByLabel( ev, "skimmedPatElectrons" );
         chisqHandle.getByLabel( ev, "tstarMassReco", "ChiSquareResult", "TstarMassReco" );

         for( const auto& el : electronHandle.ref() ){
            FillWeightErrHists( "LepPt",  el.pt(),  sampleweight, sample, ev, pdfidgroup, true );
            FillWeightErrHists( "LepEta", el.eta(), sampleweight, sample, ev, pdfidgroup, true );
         }

         for( const auto& mu : muonHandle.ref() ){
            FillWeightErrHists( "LepPt",  mu.pt(),  sampleweight, sample, ev, pdfidgroup, true );
            FillWeightErrHists( "LepEta", mu.eta(), sampleweight, sample, ev, pdfidgroup, true );
         }

         FillWeightErrHists( "MET",     metHandle->front().pt(),  sampleweight, sample, ev, pdfidgroup, true );
         FillWeightErrHists( "JetNum",  jetHandle->size(),        sampleweight, sample, ev, pdfidgroup, true );
         FillWeightErrHists( "Jet1Eta", jetHandle->at( 0 ).eta(), sampleweight, sample, ev, pdfidgroup, true );
         FillWeightErrHists( "Jet2Eta", jetHandle->at( 1 ).eta(), sampleweight, sample, ev, pdfidgroup, true );

         FillWeightErrHists( "Jet1Pt",  jetHandle->at( 0 ).pt(),  sampleweight, sample, ev, pdfidgroup, false );
         const double jet1scale = jetHandle->at( 0 ).userFloat( "jecunc" );
         const double jet1res   = jetHandle->at( 0 ).userFloat( "respt" );
         Hist( "Jet1Ptjecup" )->Fill( jetHandle->at( 0 ).pt() * ( 1+jet1scale ), totalweight );
         Hist( "Jet1Ptjecdown" )->Fill( jetHandle->at( 0 ).pt() * ( 1-jet1scale ), totalweight );
         Hist( "Jet1Ptjerup" )->Fill( jetHandle->at( 0 ).pt() * ( 1+jet1res ), totalweight );
         Hist( "Jet1Ptjerdown" )->Fill( jetHandle->at( 0 ).pt() * ( 1-jet1res ), totalweight );

         FillWeightErrHists( "Jet2Pt", jetHandle->at( 1 ).pt(), sampleweight, sample, ev, pdfidgroup, false );
         const double jet2scale = jetHandle->at( 1 ).userFloat( "jecunc" );
         const double jet2res   = jetHandle->at( 1 ).userFloat( "respt" );
         Hist( "Jet2Ptjecup" )->Fill( jetHandle->at( 1 ).pt() * ( 1+jet2scale ), totalweight );
         Hist( "Jet2Ptjecdown" )->Fill( jetHandle->at( 1 ).pt() * ( 1-jet2scale ), totalweight );
         Hist( "Jet2Ptjerup" )->Fill( jetHandle->at( 1 ).pt() * ( 1+jet2res ), totalweight );
         Hist( "Jet2Ptjerdown" )->Fill( jetHandle->at( 1 ).pt() * ( 1-jet2res ), totalweight );

         FillWeightErrHists( "TstarMass", chisqHandle->TstarMass(), sampleweight, sample, ev, pdfidgroup, false );
         Hist( "TstarMassjecup" )->Fill( chisqHandle->ComputeFromPaticleList( tstar::corr_up ), totalweight );
         Hist( "TstarMassjecdown" )->Fill( chisqHandle->ComputeFromPaticleList( tstar::corr_down ), totalweight );
         Hist( "TstarMassjerup" )->Fill( chisqHandle->ComputeFromPaticleList( tstar::res_up ), totalweight );
         Hist( "TstarMassjerdown" )->Fill( chisqHandle->ComputeFromPaticleList( tstar::res_down ), totalweight );
      }

   }

   cout << "Done!" << endl;

}


/*******************************************************************************
*   Extended histogram decalration, naming and filling conventions
*******************************************************************************/
void
SampleErrHistMgr::AddErrHists(
   const string& centralhistname,
   const string& xlabel,
   const string& xunit,
   const int     bin_size,
   const double  x_min,
   const double  x_max
   )
{
   AddHist( centralhistname, xlabel, xunit, bin_size, x_min, x_max );

   for( const auto& errname : histerrlist ){
      AddHist( centralhistname+errname.tag+"up",   xlabel, xunit, bin_size, x_min, x_max );
      AddHist( centralhistname+errname.tag+"down", xlabel, xunit, bin_size, x_min, x_max );
   }
}

void
SampleErrHistMgr::FillWeightErrHists(
   const std::string&               centralhistname,
   const double                     fillvalue,
   const double                     sampleweight,
   const mgr::SampleMgr&            sample,
   const fwlite::EventBase&         ev,
   const vector<vector<unsigned> >& pdfidgoup,
   const bool                       filljec
   )
{
   const double eventweight    = GetEventWeight( ev );
   const double puweight       = GetPileupWeight( ev );
   const double puweightup     = std::max( GetPileupWeight( ev ), std::max( GetPileupWeightXsecup( ev ), GetPileupWeightXsecdown( ev ) ) );
   const double puweightdown   = std::min( GetPileupWeight( ev ), std::min( GetPileupWeightXsecup( ev ), GetPileupWeightXsecdown( ev ) ) );
   const double btagweight     = GetBtagWeight( ev );
   const double btagweightup   = GetBtagWeightUp( ev );
   const double btagweightdown = GetBtagWeightDown( ev );
   const double elecweight     = GetElectronWeight( ev );
   const double elecweightup   = GetElectronWeightUp( ev );
   const double elecweightdown = GetElectronWeightDown( ev );
   const double muonweight     = GetMuonWeight( ev );
   const double muonweightup   = GetMuonWeightUp( ev );
   const double muonweightdown = GetMuonWeightDown( ev );
   const double pdfweighterr   = GetPdfWeightError( ev, pdfidgoup );
   const double scaleweighterr = GetScaleWeightError( ev, pdfidgoup );
   const double topptweight    = GetSampleEventTopPtWeight( sample, ev );

   const double totalweight = sampleweight * eventweight * topptweight;
   Hist( centralhistname )->Fill( fillvalue, totalweight );
   Hist( centralhistname + "puup" )->Fill( fillvalue, totalweight * puweightup / puweight );
   Hist( centralhistname + "pudown" )->Fill( fillvalue, totalweight * puweightdown / puweight );
   Hist( centralhistname + "btagup" )->Fill( fillvalue, totalweight * btagweightup / btagweight );
   Hist( centralhistname + "btagdown" )->Fill( fillvalue, totalweight * btagweightdown / btagweight );
   Hist( centralhistname + "leptonup" )->Fill( fillvalue, totalweight * elecweightup * muonweightup / elecweight / muonweight );
   Hist( centralhistname + "leptondown" )->Fill( fillvalue, totalweight * elecweightdown * muonweightdown / elecweight / muonweight );
   Hist( centralhistname + "pdfup" )->Fill( fillvalue, totalweight * ( 1 + pdfweighterr ) );
   Hist( centralhistname + "pdfdown" )->Fill( fillvalue, totalweight * ( 1 - pdfweighterr ) );
   Hist( centralhistname + "scaleup" )->Fill( fillvalue, totalweight * ( 1 + scaleweighterr ) );
   Hist( centralhistname + "scaledown" )->Fill( fillvalue, totalweight * ( 1 - scaleweighterr ) );


   if( filljec ){
      Hist( centralhistname + "jecup" )->Fill( fillvalue, totalweight );
      Hist( centralhistname + "jecdown" )->Fill( fillvalue, totalweight );
      Hist( centralhistname + "jerup" )->Fill( fillvalue, totalweight  );
      Hist( centralhistname + "jerdown" )->Fill( fillvalue, totalweight );
   }
}

/*******************************************************************************
*   Constructor and Desctructor
*******************************************************************************/
SampleErrHistMgr::SampleErrHistMgr( const string& name, const ConfigReader& cfg ) :
   Named( name ),
   SampleGroup( name, cfg ),
   HistMgr( name )
{
   define_hist();

   for( auto& sample : SampleList() ){
      // Caching sample wide objects, see Common/src/InitSample.cc
      InitSample( *sample );

      // Filling in historgram, see above
      fill_histograms( *sample );
   }
}

SampleErrHistMgr::~SampleErrHistMgr(){}

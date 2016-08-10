/*******************************************************************************
 *
 *  Filename    : CompareHistMgr.cc
 *  Description : Implementation of comparison manager
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/CompareHistMgr.hpp"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"

#include <boost/algorithm/string.hpp>
#include <iostream>

using namespace std;
using namespace tstar;
using fwlite::Event;
using fwlite::Handle;
//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
CompareHistMgr::CompareHistMgr(
   const string& name,
   const string& latex_name
):
   Named(name), // From virtual inheritance
   HistMgr(name)
{
   SetLatexName(latex_name);
   SetFillStyle(0);
   define_hist();

   _match_map = new TH2D( (Name()+ "JetMatchMap").c_str(), (Name()+"JetMatchMap").c_str(),
      5 , 0 , 5 , // Xaxis  fitted result
      6 , 0 , 6   // Yaxis  mc truth particle (must include unknown)
   );

}

CompareHistMgr::~CompareHistMgr()
{
   delete _match_map;
}

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
int GetBinPosition( Particle_Label );

void CompareHistMgr::define_hist()
{
   AddHist( "TstarMass"  , "Reco. t* Mass"                  , "GeV/c^{2}", 60, 0, 3000 );
   AddHist( "ChiSq"      , "Reco. #chi^{2} of Method"       , ""         , 60, 0, 100  );
   AddHist( "LepTopMass" , "Reco. Leptonic Top Mass"        , "GeV/c^{2}", 100, 0, 500  );
   AddHist( "HadTopMass" , "Reco. Hadronic Top Mass"        , "GeV/c^{2}", 100, 0, 500  );
   AddHist( "HadWMass"   , "Reco. Hadronic W Boson Mass"    , "GeV/c^{2}", 40,  0, 200  );
   AddHist( "LepPtDiff"  , "Diff. in Fitted/Gen Lepton p_{T}"        , "GeV/c", 80,  -200, 200 );
   AddHist( "LepBPtDiff" , "Diff. in Fitted/Gen Leptonic b-tag p_{T}", "GeV/c", 80,  -200, 200 );
   AddHist( "LepGPtDiff" , "Diff. in Fitted/Gen Leptonic gluon p_{T}", "GeV/c", 80,  -200, 200 );
   AddHist( "HadBPtDiff" , "Diff. in Fitted/Gen Hadronic b-tag p_{T}", "GeV/c", 80,  -200, 200 );
   AddHist( "HadGPtDiff" , "Diff. in Fitted/Gen Hadronic gluon p_{T}", "GeV/c", 80,  -200, 200 );
   AddHist( "NeuPz"      , "Solved Neutrino P_{z}"                   , "GeV/c", 40,     0, 400 );
}

void CompareHistMgr::AddEvent( const fwlite::Event& ev )
{
   const string module = boost::starts_with( Name() , "ChiSq" )?
      "ChiSquareResult" : "HitFitResult" ;
   const string process = "HitFitCompare";
   _result_handle.getByLabel( ev, Name().c_str() , module.c_str(), process.c_str() );
   const RecoResult& result = *(_result_handle);
   Hist( "TstarMass"  )->Fill( result.TstarMass() );
   Hist( "ChiSq"      )->Fill( result.ChiSquare() );
   Hist( "LepTopMass" )->Fill( result.LeptonicTop().M()   );
   Hist( "HadTopMass" )->Fill( result.HadronicTop().M()   );
   Hist( "HadWMass"   )->Fill( result.HadronicW().M()   );
   Hist( "LepPtDiff"  )->Fill( result.Lepton(fitted).Pt()        - result.Lepton(original).Pt()        );
   Hist( "LepBPtDiff" )->Fill( result.LeptonicBJet(fitted).Pt()  - result.LeptonicBJet(original).Pt()  );
   Hist( "LepGPtDiff" )->Fill( result.LeptonicGluon(fitted).Pt() - result.LeptonicGluon(original).Pt() );
   Hist( "HadBPtDiff" )->Fill( result.HadronicBJet(fitted).Pt()  - result.HadronicBJet(original).Pt()  );
   Hist( "HadGPtDiff" )->Fill( result.HadronicGluon(fitted).Pt() - result.HadronicGluon(original).Pt() );
   Hist( "NeuPz"      )->Fill( result.Neutrino(fitted).Pz()   );

   _match_map->Fill( GetBinPosition(lepb_label ), GetBinPosition(result.GetParticle(lepb_label) .TypeFromTruth()) );
   _match_map->Fill( GetBinPosition(lepg_label ), GetBinPosition(result.GetParticle(lepg_label) .TypeFromTruth()) );
   _match_map->Fill( GetBinPosition(hadw1_label), GetBinPosition(result.GetParticle(hadw1_label).TypeFromTruth()) , 0.5);
   _match_map->Fill( GetBinPosition(hadw2_label), GetBinPosition(result.GetParticle(hadw2_label).TypeFromTruth()) , 0.5);
   _match_map->Fill( GetBinPosition(hadb_label ), GetBinPosition(result.GetParticle(hadb_label) .TypeFromTruth())  );
   _match_map->Fill( GetBinPosition(hadg_label ), GetBinPosition(result.GetParticle(hadg_label) .TypeFromTruth())  );
}


int GetBinPosition( Particle_Label x )
{
   if( x == lepb_label ){
      return 0;
   } else if( x == lepg_label ){
      return 1;
   } else if( x == hadw1_label || x == hadw2_label ){
      return 2;
   } else if( x == hadb_label ){
      return 3;
   } else if( x== hadg_label ){
      return 4;
   } else {
      return 5;
   }
}

int CompareHistMgr::EventCount() const
{
   static const string histname = AvailableHistList().front();
   return Hist(histname)->GetEntries();
}

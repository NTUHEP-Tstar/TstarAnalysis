/*******************************************************************************
 *
 *  Filename    : HitFitter.cc
 *  Description : Implementations of HitFitter class
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/HitFitter.hpp"
#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hpp"

#include "TopQuarkAnalysis/TopHitFit/interface/fourvec.h"
#include "TopQuarkAnalysis/TopHitFit/interface/Defaults_Text.h"

#include <iostream>
#include <algorithm>
using namespace std;
//------------------------------------------------------------------------------
//   Constant variables
//------------------------------------------------------------------------------
#define DEFAULT_W_MASS    80.4
#define DEFAULT_TOP_MASS   0.0
static const edm::FileInPath defaultFitter( "TstarAnalysis/TstarMassReco/data/top_gluon.txt" );
static const edm::FileInPath defaultElecFile( "TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt" );
static const edm::FileInPath defaultMuonFile( "TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt" );
static const edm::FileInPath defaultLJetFile( "TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt" );
static const edm::FileInPath defaultBJetFile( "TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt" );

//------------------------------------------------------------------------------
//   Constructor and Destructor
//------------------------------------------------------------------------------
HitFitter::HitFitter( const edm::ParameterSet& iConfig ):
   _max_jets( iConfig.getUntrackedParameter<int>("MaxJet" , 6 ) ),
   _max_required_b_jet( iConfig.getUntrackedParameter<int>("ReqBJet" , 1 ) ),
   _met( NULL ),
   _muon( NULL ),
   _elec( NULL ),
   _debug( iConfig.getUntrackedParameter<int>( "Debug" , 0 ) )
{
   const edm::FileInPath fitterFile = iConfig.getUntrackedParameter<edm::FileInPath>( "fitterConfig" , defaultFitter );
   double fitter_lepWMass = iConfig.getUntrackedParameter<double>( "fittingLeptonicWMass" ,  DEFAULT_W_MASS );
   double fitter_hadWMass = iConfig.getUntrackedParameter<double>( "fittingHadronicWMass" ,  DEFAULT_W_MASS );
   double fitter_topMass  = iConfig.getUntrackedParameter<double>( "fittingTopMass" ,  DEFAULT_TOP_MASS );
   _top_fitter = new hitfit::Top_Fit(
      hitfit::Defaults_Text(fitterFile.fullPath()) ,
      fitter_lepWMass ,
      fitter_hadWMass ,
      fitter_topMass
   );


   if( _debug ) { cout << "Getting Resolutions..." << endl; }
   const edm::FileInPath elecResolutionFile = iConfig.getUntrackedParameter<edm::FileInPath>( "electronResolution" , defaultElecFile );
   const edm::FileInPath muonResolutionFile = iConfig.getUntrackedParameter<edm::FileInPath>( "muonResolution"     , defaultMuonFile );
   const edm::FileInPath ljetResolutionFile = iConfig.getUntrackedParameter<edm::FileInPath>( "lightJetResolution" , defaultLJetFile );
   const edm::FileInPath bjetResolutionFile = iConfig.getUntrackedParameter<edm::FileInPath>( "bJetResolution"     , defaultBJetFile );
   _electronResolution = new hitfit::EtaDepResolution( elecResolutionFile.fullPath() );
   _muonResolution     = new hitfit::EtaDepResolution( muonResolutionFile.fullPath() );
   _lightJetResolution = new hitfit::EtaDepResolution( ljetResolutionFile.fullPath() );
   _bJetResolution     = new hitfit::EtaDepResolution( bjetResolutionFile.fullPath() );
   _met_KtResolution   = new hitfit::Resolution("0,0,12");
}

HitFitter::~HitFitter()
{
   delete _top_fitter;
   delete _electronResolution;
   delete _muonResolution;
   delete _lightJetResolution;
   delete _bJetResolution;
   delete _met_KtResolution;
}

//------------------------------------------------------------------------------
//   Main control - Runing HitFit permutations
//------------------------------------------------------------------------------

void HitFitter::RunPermutations()
{
   // Dummy variables for storing fitting results;
   bool   solveNeutrino;
   double fittedWHadronMass;
   double fittedTopMass;
   double fittedTopMass2;
   double topMassUnc;
   double chiSquare;
   hitfit::Column_Vector pullx;
   hitfit::Column_Vector pully;

   // Dummy event only handling MET and Lepton
   // Jets are handled in the permutation loop
   hitfit::Lepjets_Event hitFitEventTemplate( 0,0 );

   //----- Setting up objects -----
   SetHitFitTemplate( hitFitEventTemplate );

   //----- Setting up Jet Labels  -------------------------------------------------
   const unsigned numberOfJets = _btagJetList.size() + _lightJetList.size();
   const unsigned hitfitJetCount = (numberOfJets < _max_jets ) ? numberOfJets : _max_jets ;
   vector<int> jet_type_list( hitfitJetCount , hitfit::unknown_label ) ;
   jet_type_list[0] = hitfit::lepb_label;
   jet_type_list[1] = hitfit::hadb_label;
   jet_type_list[2] = hitfit::hadw1_label;
   jet_type_list[3] = hitfit::hadw2_label;
   jet_type_list[4] = hitfit::lepg_label;
   jet_type_list[5] = hitfit::hadg_label;


   //----- Permute jet labels  ---------------------------------------------------
   do {
      if( !CheckBTagOrder( jet_type_list ) ) { continue; }

      hitfit::Lepjets_Event hitFitEvent = hitFitEventTemplate;
      unsigned i = 0 ;
      for( const auto jet : _btagJetList ){
         AddHitFitJet( hitFitEvent, jet, jet_type_list[i]); ++i; }
      for( const auto jet : _lightJetList ){
         AddHitFitJet( hitFitEvent, jet, jet_type_list[i]); ++i;
         if( i >= hitfitJetCount ){ break; }
      }
      hitFitEvent.set_jet_types( jet_type_list );

      // Running Hit Fit
      for( int i = 0 ; i < 2 ; ++i ){
         solveNeutrino = i;
         chiSquare = -999.;
         try {
            chiSquare = _top_fitter->fit_one_perm(
               hitFitEvent, solveNeutrino,
               fittedWHadronMass, fittedTopMass, fittedTopMass2, topMassUnc,
               pullx, pully
            );
         } catch( std::exception e ){
            continue;
         }
         if( chiSquare > 1. ){
            AddResult(
               fittedTopMass2,
               chiSquare,
               hitFitEvent
            );
         }
      }
   } while ( next_permutation( jet_type_list.begin() , jet_type_list.end() ) );
}

void HitFitter::ClearAll()
{
   _results_list.clear();
   _btagJetList.clear();
   _lightJetList.clear();
   _met  = NULL;
   _elec = NULL;
   _muon = NULL;
}

const RecoResult& HitFitter::BestResult() const
{
   static RecoResult __null_return__;
   __null_return__._chiSquare = -1;

   int index = -1;
   double min_chiSq = 100000000.;
   for( unsigned i = 0 ; i < _results_list.size() ; ++i  ){
      if( _results_list[i].ChiSquare() < min_chiSq ){
         min_chiSq = _results_list[i].ChiSquare();
         index = i;
      }
   }

   if( index >= 0 && (unsigned)index < _results_list.size() ){
      return _results_list[index];
   } else {
      return __null_return__;
   }
}

void HitFitter::AddResult(
   const double tstarmass,
   const double chisq,
   const hitfit::Lepjets_Event& ev)
{
   using namespace tstar;
   RecoResult new_result;
   new_result._tstarMass = tstarmass;
   new_result._chiSquare = chisq;

   // Lepton
   FitParticle new_lepton = _muon != NULL?
      MakeResultMuon( _muon ) :
      MakeResultElectron( _elec ) ;
   new_lepton.P4(fitted) = ConvertToRoot( ev.lep(0).p() );
   new_result.AddParticle( new_lepton );

   // Neutrino
   FitParticle new_met ;
   new_met = MakeResultMET( _met );
   new_met.P4(fitted) = ConvertToRoot( ev.met() );
   new_result.AddParticle( new_met );

   const vector<int>& jet_type_list = ev.jet_types();

   unsigned i = 0;
   for( const auto& jet : _btagJetList ){
      Particle_Label jet_type = Particle_Label(jet_type_list[i]);
      if( jet_type == unknown_label ) { continue; }
      FitParticle  new_jet    = MakeResultJet(jet,jet_type);
      new_jet.P4(fitted)      = ConvertToRoot( ev.sum(jet_type));
      new_result.AddParticle( new_jet );
      ++i;
   }
   for( const auto& jet : _lightJetList ){
      Particle_Label jet_type = Particle_Label(jet_type_list[i]);
      if( jet_type == unknown_label ) { continue; }
      FitParticle  new_jet    = MakeResultJet(jet,jet_type);
      new_jet.P4(fitted)      = ConvertToRoot( ev.sum(jet_type));
      new_result.AddParticle( new_jet );
      ++i;
   }

   _results_list.push_back( new_result );
}



//------------------------------------------------------------------------------
//   Translator private functions
//------------------------------------------------------------------------------
void HitFitter::SetMET( const pat::MET* met )
{
   _met = met;
}

void HitFitter::SetMuon( const pat::Muon* mu )
{
   _muon = mu;
}

void HitFitter::SetElectron( const pat::Electron* elec )
{
   _elec = elec;
}

void HitFitter::AddBTagJet( const pat::Jet* jet  )
{
   if( _btagJetList.size() < _max_required_b_jet ){
      _btagJetList.push_back( jet );
   } else {
      _lightJetList.push_back( jet );
   }
}

extern bool SortJet( const pat::Jet*, const pat::Jet* ); // See ChiSquareSolver.cc
void HitFitter::AddLightJet( const pat::Jet* jet )
{
   const unsigned target_lightjet_size = _max_jets - _btagJetList.size();
   _lightJetList.push_back( jet );

   // Truncating Jets to avoid
   stable_sort( _lightJetList.begin(), _lightJetList.end() , SortJet );
   if( _lightJetList.size() > target_lightjet_size ){
      _lightJetList.resize( target_lightjet_size );
   }
}

void HitFitter::SetHitFitTemplate( hitfit::Lepjets_Event& hitFitEvent ) const
{
   hitFitEvent.met()   = hitfit::Fourvec( _met->px(), _met->py() , 0 , _met->pt() );
   hitFitEvent.kt_res() = *( _met_KtResolution );

   if( _muon != NULL ) { // Muon
      double lep_eta = _muon->eta() ; // Required! For type conversion
      hitfit::Fourvec lep_p4( _muon->px() , _muon->py() , _muon->pz() , _muon->energy() );
      hitfit::Vector_Resolution lep_resolution = _muonResolution->GetResolution( lep_eta );
      hitFitEvent.add_lep( hitfit::Lepjets_Event_Lep(lep_p4 , 0 , lep_resolution) ); //label = 0 for primary lepton
   } else if( _elec != NULL ){
      double lep_eta = _elec->eta() ; // Required! For type conversion
      hitfit::Fourvec lep_p4( _elec->px() , _elec->py() , _elec->pz() , _elec->energy() );
      hitfit::Vector_Resolution lep_resolution = _electronResolution->GetResolution( lep_eta );
      hitFitEvent.add_lep( hitfit::Lepjets_Event_Lep(lep_p4 , 0 , lep_resolution) ); //label = 0 for primary lepton
   }
}

void HitFitter::AddHitFitJet( hitfit::Lepjets_Event& hitFitEvent, const pat::Jet* jet , const int type ) const
{
   double jet_eta = jet->eta();
   hitfit::Fourvec jetp4( jet->px() , jet->py() , jet->pz(), jet->energy() );
   hitfit::Vector_Resolution jet_resolution ;
   if( type == hitfit::hadb_label || type == hitfit::lepb_label ){
      jet_resolution = _bJetResolution->GetResolution( jet_eta );
   } else {
      jet_resolution = _lightJetResolution->GetResolution( jet_eta );
   }
   hitFitEvent.add_jet( hitfit::Lepjets_Event_Jet(jetp4 , type , jet_resolution) );
}

bool HitFitter::CheckBTagOrder( const std::vector<int>& jet_type_list ) const
{
   for( unsigned i = 0 ; i < _btagJetList.size(); ++i ){
      if( jet_type_list[i] != hitfit::hadb_label && jet_type_list[i] != hitfit::lepb_label ) {
         return false;
      }
   }
   return true;
}

/*******************************************************************************
 *
 *  Filename    : ChiSquareSolver.h
 *  Description : Implementations for the ChiSquare Mass solver
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include "TstarAnalysis/TstarMassReco/interface/ChiSquareSolver.hh"
#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hh"

#include <algorithm>
#include <iostream>
using namespace std;

//------------------------------------------------------------------------------
//   Defining constants
//------------------------------------------------------------------------------
#define TOP_MASS    173.34
#define TOP_WIDTH   2.00
#define W_MASS      80.385
#define W_WIDTH     2.085
#define TSTAR_WIDTH 10
#define DEFAULT_REQUIRED_BJETS_MATCHING 1
#define DEFAULT_MAX_JETS_TO_RUN 6
bool SortJet( const pat::Jet* x, const pat::Jet* y ){ return (x->pt() > y->pt()); }

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
ChiSquareSolver::ChiSquareSolver(const edm::ParameterSet& iConfig):
   _debug( iConfig.getUntrackedParameter<int>( "Debug" , 0 ) ),
   _max_jets( iConfig.getUntrackedParameter<int>("MaxJet" , DEFAULT_MAX_JETS_TO_RUN )),
   _req_b_jets( iConfig.getUntrackedParameter<int>("ReqBJet", DEFAULT_REQUIRED_BJETS_MATCHING ))
{}

ChiSquareSolver::~ChiSquareSolver(){}

//------------------------------------------------------------------------------
//   Main Control Flow - Running Permutations
//------------------------------------------------------------------------------
void ChiSquareSolver::RunPermutations()
{
   double chiSquare;
   double tstarMass;
   TLorentzVector lep_w;
   TLorentzVector lep_t;
   TLorentzVector lep_tstar;
   TLorentzVector had_w;
   TLorentzVector had_t;
   TLorentzVector had_tstar;
   if( _debug ) { cout << "Running all chi square permutations" << endl; }

   unsigned config = 0;
   do{
      cout << "\r[ChiSq] Running config " << config << flush;
      if( _debug > 2 ) {
         cout << "\t[ChiSq Jet Pt] " << flush;
         for( const auto& vec: _jetList ){
            cout << vec->pt() << " " << flush;
         }
         cout << endl;
      }
      if( !CheckPermutation() ){ continue; }
      had_w     = had_q1() + had_q2();
      had_t     = had_w    + had_b();
      had_tstar = had_t    + had_g();

      for( unsigned i = 0 ; i < 2 ; ++i ){
         lep_w     = ConvertToRoot(*_lepton) + _neutrino[i] ;
         lep_t     = lep_w   + lep_b();
         lep_tstar = lep_t   + lep_g();

         chiSquare =
            (( had_w.M()     - W_MASS        ) * ( had_w.M()     - W_MASS        )) / ( W_WIDTH     * W_WIDTH     )
            + (( had_t.M()     - TOP_MASS      ) * ( had_t.M()     - TOP_MASS      )) / ( TOP_WIDTH   * TOP_WIDTH   )
            + (( lep_t.M()     - TOP_MASS      ) * ( lep_t.M()     - TOP_MASS      )) / ( TOP_WIDTH   * TOP_WIDTH   )
            + (( lep_tstar.M() - had_tstar.M() ) * ( lep_tstar.M() - had_tstar.M() )) / ( TSTAR_WIDTH * TSTAR_WIDTH ) ;

         tstarMass = (lep_tstar.M() + had_tstar.M()) / 2.;
         AddResult( tstarMass , chiSquare , i );
         if( _debug > 2 ) { cout << "\t " << tstarMass << " "<< chiSquare << endl; }
      }
      ++config;
   } while( next_permutation( _jetList.begin() , _jetList.end(), SortJet ) );
   cout << "Finished!" << endl;
}

void ChiSquareSolver::AddResult(const double tstar_mass,const double chi_square, const unsigned neu_index )
{
   RecoResult new_result;
   new_result._tstarMass = tstar_mass ;
   new_result._chiSquare = chi_square ;

   // Lepton
   new_result.AddParticle( MakeResultLepton( _lepton ) );

   // Neutrino
   FitParticle new_met = MakeResultMET( _met );
   new_met.FittedP4() = _neutrino[neu_index];
   new_result.AddParticle( new_met);

   // Jets
   const pat::Jet* had_b_jet  = _jetList[0];
   new_result.AddParticle( MakeResultJet( had_b_jet, hadb_label ) );

   const pat::Jet* lep_b_jet  = _jetList[1];
   new_result.AddParticle( MakeResultJet( lep_b_jet, lepb_label ) );

   const pat::Jet* had_g_jet  = _jetList[2];
   new_result.AddParticle( MakeResultJet( had_g_jet, hadg_label ) );

   const pat::Jet* had_w1_jet  = _jetList[3];
   new_result.AddParticle( MakeResultJet( had_w1_jet, hadw1_label ) );

   const pat::Jet* had_w2_jet  = _jetList[4];
   new_result.AddParticle( MakeResultJet( had_w2_jet, hadw2_label ) );

   const pat::Jet* lep_g_jet  = _jetList[5];
   new_result.AddParticle( MakeResultJet( lep_g_jet, lepg_label ) );

   _resultsList.push_back( new_result );
}


const RecoResult& ChiSquareSolver::BestResult() const
{
   if( _debug ) { cout << "Getting Best Results..." << endl; }
   static RecoResult __null_result__;
   __null_result__._chiSquare = -1000;
   int index = -1;
   double min_chiSq = 100000000.;
   for( unsigned i = 0 ; i < _resultsList.size() ; ++i  ){
      if( _resultsList[i].ChiSquare() < min_chiSq ){
         min_chiSq = _resultsList[i].ChiSquare();
         index = i;
      }
   }

   if( _debug ){
      cout << "Found best results at " << index
      << " with chi^2 value: " << min_chiSq << endl;
   }
   if( index != -1 ){
      return _resultsList[index];
   }
   return __null_result__;
}

//------------------------------------------------------------------------------
//   Setting functions
//------------------------------------------------------------------------------


void ChiSquareSolver::SetMET( const pat::MET* x )
{
   _met = x;
}

void ChiSquareSolver::SetLepton( const reco::RecoCandidate* x )
{
   _lepton = x;
   solveNeutrino();
}

void ChiSquareSolver::AddJet( const pat::Jet* jet )
{
   _jetList.push_back( jet );
   stable_sort( _jetList.begin() , _jetList.end() , SortJet );
   // Truncating in size to avoid large run time.
   if( _jetList.size() > _max_jets ){
      _jetList.resize( _max_jets );
   }
}

void ChiSquareSolver::ClearAll()
{
   _jetList.clear();
   _resultsList.clear();
}


//------------------------------------------------------------------------------
//   Helper Private member functions
//------------------------------------------------------------------------------
void ChiSquareSolver::solveNeutrino()
{
   double _alpha_ , _beta_    , _gamma_          ;
   double _a_     , _b_       , _c_       , _d_  ;
   double _lx_    , _ly_      , _lz_      , _lE_ ;
   double _npx_   , _npy_     , _npz_     , _nE_ ;
   double _met_   ;

   _met_  = _met->pt();
   _npx_  = _met->px();
   _npy_  = _met->py();

   _lx_   = _lepton->px()     ;
   _ly_   = _lepton->py()     ;
   _lz_   = _lepton->pz()     ;
   _lE_   = _lepton->energy() ;

   _alpha_ = _npx_ + _lx_ ;
   _beta_  = _npy_ + _ly_ ;
   _gamma_ = W_MASS*W_MASS  - _met_*_met_ - _lE_*_lE_
   + _alpha_*_alpha_ + _beta_*_beta_ + _lz_*_lz_ ;

   _a_ =  4. *( _lE_*_lE_- _lz_*_lz_ );
   _b_ = -4. * _gamma_ * _lz_;
   _c_ =  4. * _lE_*_lE_ * _met_*_met_ - _gamma_*_gamma_ ;
   _d_ = _b_ * _b_ - 4. * _a_ * _c_;

   if( _d_ < 0 ) {
      _npz_ = -1. * _b_ / ( 2.*_a_ ) ;
      _nE_  = sqrt( _npx_ * _npx_  + _npy_ * _npy_ + _npz_ * _npz_ ) ;
      _neutrino[0] = TLorentzVector( _npx_ , _npy_ , _npz_ , _nE_ ) ;
      _neutrino[1] = _neutrino[0];
   } else {
      _npz_ = ( -1. * _b_ + sqrt( _d_ ) )/ ( 2.*_a_ ) ;
      _nE_  = sqrt(_npx_ * _npx_  + _npy_ * _npy_ + _npz_ * _npz_ );
      _neutrino[0] = TLorentzVector( _npx_ , _npy_ , _npz_ , _nE_ ) ;
      _npz_ = ( -1. * _b_ - sqrt( _d_ ) )/ ( 2.*_a_ ) ;
      _nE_  = sqrt( _npx_ * _npx_  + _npy_ * _npy_ + _npz_ * _npz_) ;
      _neutrino[1] = TLorentzVector( _npx_ , _npy_ , _npz_ , _nE_ ) ;
   }
   if( _debug > 1 ){
      cout << "Lepton: " << _lepton->pt() << endl;
      cout << "Neutrino:" << _neutrino[0].Pt() << endl;
   }
}

bool IsBtagged( const pat::Jet* x )
{
   return ( x->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") > 0.86 );
}

bool ChiSquareSolver::CheckPermutation() const
{
   unsigned bjets_in_list = 0 ;
   for( const auto& jet : _jetList ){
      if( IsBtagged(jet)  ){
         ++bjets_in_list ;
      }
   }

   unsigned matched_b_jets = 0;
   if( IsBtagged(_jetList[0]) ) ++matched_b_jets;
   if( IsBtagged(_jetList[1]) ) ++matched_b_jets;

   if( matched_b_jets < _req_b_jets ){
      if( matched_b_jets >= bjets_in_list ){
         return true;
      } else {
         return false;
      }
   } else {
      return true;
   }
}

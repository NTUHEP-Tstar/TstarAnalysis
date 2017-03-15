/*******************************************************************************
*
*  Filename    : ChiSquareSolver.h
*  Description : Implementations for the ChiSquare Mass solver
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "ManagerUtils/PhysUtils/interface/ObjectExtendedMomentum.hpp"
#include "ManagerUtils/SysUtils/interface/PathUtils.hpp"
#include "TstarAnalysis/TstarMassReco/interface/ChiSquareSolver.hpp"
#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hpp"

#include <algorithm>
#include <cfloat>
#include <iostream>
using namespace std;

/*******************************************************************************
*   Defining constants and helper function
*******************************************************************************/
bool
SortJet( const pat::Jet* x, const pat::Jet* y ){ return x->pt() > y->pt();  }

/*******************************************************************************
*   Constructor and Destructor
*******************************************************************************/

ChiSquareSolver::ChiSquareSolver( const edm::ParameterSet& iConfig ) :
  _bcheck( "check", GETFILEPATH( iConfig, "btagfile" ) ),
  _debug(             iConfig.getUntrackedParameter<int>( "Debug", 0 ) ),
  _max_jets(          iConfig.getUntrackedParameter<int>( "MaxJet", 1 ) ),
  _req_b_jets(        iConfig.getUntrackedParameter<int>( "ReqBJet", 6 ) ),
  _gluonjet_maxorder( iConfig.getUntrackedParameter<int>( "GluonMaxOrder", -1 ) ),
  _bjet_maxorder(     iConfig.getUntrackedParameter<int>( "BQuarkMaxOrder", -1 ) ),
  _topmass(           iConfig.getUntrackedParameter<double>( "TopMass", 173.34 ) ),
  _wmass(             iConfig.getUntrackedParameter<double>( "WMass", 80.385 ) ),
  _topwidth(          iConfig.getUntrackedParameter<double>( "TopWidth", 2.00 ) ),
  _wwidth(            iConfig.getUntrackedParameter<double>( "WWidth", 2.085 ) ),
  _tstarwidth(        iConfig.getUntrackedParameter<double>( "TstarWidth", 10 ) )
{}

/******************************************************************************/

ChiSquareSolver::~ChiSquareSolver(){}

/*******************************************************************************
*   Running all permutations
*******************************************************************************/
void
ChiSquareSolver::RunPermutations()
{
  double chiSquare = 0.0;
  double tstarMass = 0.0;
  TLorentzVector lep_w;
  TLorentzVector lep_t;
  TLorentzVector lep_tstar;
  TLorentzVector had_w;
  TLorentzVector had_t;
  TLorentzVector had_tstar;
  TLorentzVector lepton;

  if( _muon ){ lepton = mgr::GetLorentzVector( *_muon, "" ); }
  if( _electron ){ lepton = mgr::GetLorentzVector( *_electron, "" ); }

  do { // Running jet permutations
    if( !CheckPermutation() ){ continue; }
    had_w     = had_q1() + had_q2();
    had_t     = had_w    + had_b();
    had_tstar = had_t    + had_g();

    for( unsigned i = 0; i < 2; ++i ){
      lep_w     = lepton + _neutrino[i];
      lep_t     = lep_w  + lep_b();
      lep_tstar = lep_t  + lep_g();

      chiSquare =
        ( ( had_w.M() - _wmass   ) * ( had_w.M() - _wmass   ) ) / ( _wwidth*_wwidth   )
        + ( ( had_t.M() - _topmass ) * ( had_t.M() - _topmass ) ) / ( _topwidth*_topwidth )
        + ( ( lep_t.M() - _topmass ) * ( lep_t.M() - _topmass ) ) / ( _topwidth*_topwidth )
        + ( ( lep_tstar.M() - had_tstar.M() ) * ( lep_tstar.M() - had_tstar.M() ) ) / ( _tstarwidth * _tstarwidth );

      tstarMass = ( lep_tstar.M() + had_tstar.M() ) / 2.;
      AddResult( tstarMass, chiSquare, i );
    }
  } while( next_permutation( _jetlist.begin(), _jetlist.end(), SortJet ) );
}

/******************************************************************************/

void
ChiSquareSolver::AddResult( const double tstar_mass, const double chi_square, const unsigned neu_index )
{
  using namespace tstar;

  RecoResult new_result;
  new_result._tstarMass = tstar_mass;
  new_result._chiSquare = chi_square;

  // Lepton
  FitParticle new_lep;
  if( _muon ){ new_lep = MakeResultMuon( _muon ); }
  if( _electron ){ new_lep = MakeResultElectron( _electron ); }
  new_result.AddParticle( new_lep );


  // Jets
  const pat::Jet* had_b_jet = _jetlist[0];
  new_result.AddParticle( MakeResultJet( had_b_jet, hadb_label ) );

  const pat::Jet* lep_b_jet = _jetlist[1];
  new_result.AddParticle( MakeResultJet( lep_b_jet, lepb_label ) );

  const pat::Jet* had_g_jet = _jetlist[2];
  new_result.AddParticle( MakeResultJet( had_g_jet, hadg_label ) );

  const pat::Jet* had_w1_jet = _jetlist[3];
  new_result.AddParticle( MakeResultJet( had_w1_jet, hadw1_label ) );

  const pat::Jet* had_w2_jet = _jetlist[4];
  new_result.AddParticle( MakeResultJet( had_w2_jet, hadw2_label ) );

  const pat::Jet* lep_g_jet = _jetlist[5];
  new_result.AddParticle( MakeResultJet( lep_g_jet, lepg_label ) );

  // Neutrino
  double metscaleup;
  double metscaledown;
  GetJESMET( _jetlist, metscaleup, metscaledown );
  FitParticle new_met = MakeResultMET( _met, _neutrino[neu_index], metscaleup, metscaledown );
  new_result.AddParticle( new_met );

  _resultsList.push_back( new_result );
}

/******************************************************************************/

const RecoResult&
ChiSquareSolver::BestResult() const
{
  static RecoResult __null_result__;
  __null_result__._chiSquare = -1000;
  int index        = -1;
  double min_chiSq = DBL_MAX;

  for( unsigned i = 0; i < _resultsList.size(); ++i  ){
    if( _resultsList[i].ChiSquare() < min_chiSq ){
      min_chiSq = _resultsList[i].ChiSquare();
      index     = i;
    }
  }

  if( index != -1 ){
    return _resultsList[index];
  } else {
    cerr << "Warning! minimum chi square is at limit! Storing a dummy result!" << endl;
    return __null_result__;
  }
}

/*******************************************************************************
*   Setting functions
*******************************************************************************/
void
ChiSquareSolver::SetMET( const pat::MET* x )
{
  _met = x;
}

/******************************************************************************/

void
ChiSquareSolver::SetMuon( const pat::Muon* x )
{
  _muon     = x;
  _electron = NULL;
  solveNeutrino();
}

/******************************************************************************/

void
ChiSquareSolver::SetElectron( const pat::Electron* x )
{
  _muon     = NULL;
  _electron = x;
  solveNeutrino();
}

/******************************************************************************/

void
ChiSquareSolver::AddJet( const pat::Jet* jet )
{
  _jetlist.push_back( jet );
  stable_sort( _jetlist.begin(), _jetlist.end(), SortJet );
  // Truncating in size to avoid large run time.
  if( _jetlist.size() > _max_jets ){
    _jetlist.resize( _max_jets );
  }
}

/******************************************************************************/

void
ChiSquareSolver::ClearAll()
{
  _met      = NULL;
  _muon     = NULL;
  _electron = NULL;
  _jetlist.clear();
  _resultsList.clear();
}


/*******************************************************************************
*   Helper private functions
*******************************************************************************/
void
ChiSquareSolver::solveNeutrino()
{
  double _alpha_, _beta_, _gamma_;
  double _a_, _b_, _c_, _d_;
  double _lx_, _ly_, _lz_, _lE_;
  double _npx_, _npy_, _npz_, _nE_;
  double _met_;

  _met_ = _met->pt();
  _npx_ = _met->px();
  _npy_ = _met->py();

  if( _muon ){
    _lx_ = _muon->px();
    _ly_ = _muon->py();
    _lz_ = _muon->pz();
    _lE_ = _muon->energy();
  } else {
    _lx_ = _electron->px();
    _ly_ = _electron->py();
    _lz_ = _electron->pz();
    _lE_ = _electron->energy();
  }

  _alpha_ = _npx_ + _lx_;
  _beta_  = _npy_ + _ly_;
  _gamma_ = _wmass*_wmass  - _met_*_met_ - _lE_*_lE_
            + _alpha_*_alpha_ + _beta_*_beta_ + _lz_*_lz_;

  _a_ = 4. *( _lE_*_lE_- _lz_*_lz_ );
  _b_ = -4. * _gamma_ * _lz_;
  _c_ = 4. * _lE_*_lE_ * _met_*_met_ - _gamma_*_gamma_;
  _d_ = _b_ * _b_ - 4. * _a_ * _c_;

  if( _d_ < 0 ){
    _npz_        = -1. * _b_ / ( 2.*_a_ );
    _nE_         = sqrt( _npx_ * _npx_  + _npy_ * _npy_ + _npz_ * _npz_ );
    _neutrino[0] = TLorentzVector( _npx_, _npy_, _npz_, _nE_ );
    _neutrino[1] = _neutrino[0];
  } else {
    _npz_        = ( -1. * _b_ + sqrt( _d_ ) )/ ( 2.*_a_ );
    _nE_         = sqrt( _npx_ * _npx_  + _npy_ * _npy_ + _npz_ * _npz_ );
    _neutrino[0] = TLorentzVector( _npx_, _npy_, _npz_, _nE_ );
    _npz_        = ( -1. * _b_ - sqrt( _d_ ) )/ ( 2.*_a_ );
    _nE_         = sqrt( _npx_ * _npx_  + _npy_ * _npy_ + _npz_ * _npz_ );
    _neutrino[1] = TLorentzVector( _npx_, _npy_, _npz_, _nE_ );
  }
}

/******************************************************************************/

bool
ChiSquareSolver::IsBtagged( const pat::Jet* x ) const
{
  return _bcheck.PassMedium( *x );
}

/******************************************************************************/

bool
ChiSquareSolver::CheckPermutation() const
{
  unsigned bjets_in_list = 0;
  unsigned matched_b_jets = 0;
  unsigned lepgluon_order = 0;
  unsigned hadgluon_order = 0;
  unsigned lepb_order = 0;
  unsigned hadb_order = 0;

  // Looping over jets
  for( const auto& jet : _jetlist ){
    if( IsBtagged( jet ) ){ ++bjets_in_list; }
    if( mgr::GetLorentzVector( *jet, "ResP4" ).Pt() > lep_g().Pt() + 0.001 ){ ++lepgluon_order; }
    if( mgr::GetLorentzVector( *jet, "ResP4" ).Pt() > had_g().Pt() + 0.001 ){ ++hadgluon_order; }
    if( mgr::GetLorentzVector( *jet, "ResP4" ).Pt() > lep_b().Pt() + 0.001 ){ ++lepb_order; }
    if( mgr::GetLorentzVector( *jet, "ResP4" ).Pt() > had_b().Pt() + 0.001 ){ ++hadb_order; }

  }

  if( IsBtagged( _jetlist[0] ) ){ ++matched_b_jets; }
  if( IsBtagged( _jetlist[1] ) ){ ++matched_b_jets; }

  // B jet requirements
  if( matched_b_jets < _req_b_jets && matched_b_jets < bjets_in_list ){
    return false;
  }

  // Jet order requirments
  if( lepgluon_order >= _gluonjet_maxorder || hadgluon_order >= _gluonjet_maxorder ){
    return false;
  }
  if( lepb_order >= _bjet_maxorder || hadb_order >= _bjet_maxorder ){
    return false;
  }

  // Pass every requirements
  return true;
}

/*******************************************************************************
*   Jet Four momentum objects
*******************************************************************************/
TLorentzVector
ChiSquareSolver::had_b()  const { return mgr::GetLorentzVector( *_jetlist[0], "ResP4" ); }

TLorentzVector
ChiSquareSolver::lep_b()  const { return mgr::GetLorentzVector( *_jetlist[1], "ResP4" ); }

TLorentzVector
ChiSquareSolver::had_g()  const { return mgr::GetLorentzVector( *_jetlist[2], "ResP4" ); }

TLorentzVector
ChiSquareSolver::had_q1() const { return mgr::GetLorentzVector( *_jetlist[3], "ResP4" ); }

TLorentzVector
ChiSquareSolver::had_q2() const { return mgr::GetLorentzVector( *_jetlist[4], "ResP4" ); }

TLorentzVector
ChiSquareSolver::lep_g()  const { return mgr::GetLorentzVector( *_jetlist[5], "ResP4" ); }

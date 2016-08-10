/*******************************************************************************
 *
 *  Filename    : ConstrainChiSq.cc
 *  Description : Constrain fit on best chi square sorting result
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
*******************************************************************************/
#include <memory>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "TstarAnalysis/RootFormat/interface/RecoResult.hpp"

#include "TstarAnalysis/TstarMassReco/interface/RecoUtils.hpp"
#include "TstarAnalysis/ModifiedHitFit/interface/Lepjets_Event.h"
#include "TstarAnalysis/ModifiedHitFit/interface/Top_Fit.h"
#include "TopQuarkAnalysis/TopHitFit/interface/EtaDepResolution.h"
#include "TopQuarkAnalysis/TopHitFit/interface/Top_Decaykin.h"

//------------------------------------------------------------------------------
//   Static variables
//------------------------------------------------------------------------------
#define DEFAULT_W_MASS    80.4
#define DEFAULT_TOP_MASS   0.0
static const edm::FileInPath fitfile( "TstarAnalysis/TstarMassReco/data/top_gluon.txt" );
static const edm::FileInPath elecres( "TopQuarkAnalysis/TopHitFit/data/resolution/tqafElectronResolution.txt" );
static const edm::FileInPath muonres( "TopQuarkAnalysis/TopHitFit/data/resolution/tqafMuonResolution.txt" );
static const edm::FileInPath ljetres( "TopQuarkAnalysis/TopHitFit/data/resolution/tqafUdscJetResolution.txt" );
static const edm::FileInPath bjetres( "TopQuarkAnalysis/TopHitFit/data/resolution/tqafBJetResolution.txt" );

using namespace tstar;

//------------------------------------------------------------------------------
//   Class Definition
//------------------------------------------------------------------------------
class ConstrainChiSq : public edm::EDProducer {
public:
   ConstrainChiSq(const edm::ParameterSet& );
   virtual ~ConstrainChiSq();

private:
   virtual void produce( edm::Event&, const edm::EventSetup& );

   const edm::EDGetToken    _chisqsrc;
   edm::Handle<RecoResult>  _recoHandle;

   hitfit::Top_Fit                 _top_fitter;
   hitfit::EtaDepResolution        _electronResolution;  // Read from external file
   hitfit::EtaDepResolution        _muonResolution;      // Read from external file
   hitfit::EtaDepResolution        _lightJetResolution;  // Read from external file
   hitfit::EtaDepResolution        _bJetResolution;      // Read from external file
   hitfit::Resolution              _met_KtResolution;    // Constant for the time being

};

//------------------------------------------------------------------------------
//   Constructor and Desctructor
//------------------------------------------------------------------------------
ConstrainChiSq::ConstrainChiSq( const edm::ParameterSet& iConfig ):
   _chisqsrc( consumes<RecoResult>(iConfig.getParameter<edm::InputTag>("chisqsrc")) ),
   _top_fitter(
      hitfit::Defaults_Text(fitfile.fullPath()) ,
      iConfig.getUntrackedParameter<double>( "fittingLeptonicWMass" ,  DEFAULT_W_MASS ),
      iConfig.getUntrackedParameter<double>( "fittingHadronicWMass" ,  DEFAULT_W_MASS ),
      iConfig.getUntrackedParameter<double>( "fittingTopMass" ,  DEFAULT_TOP_MASS )
   ),
   _electronResolution( elecres.fullPath() ),
   _muonResolution    ( muonres.fullPath() ),
   _lightJetResolution( ljetres.fullPath() ),
   _bJetResolution    ( bjetres.fullPath() ),
   _met_KtResolution  ("0,0,12")
{
   produces<RecoResult>();
}

ConstrainChiSq::~ConstrainChiSq()
{

}


//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
void ConstrainChiSq::produce( edm::Event& iEvent, const edm::EventSetup& )
{
   using hitfit::Lepjets_Event;
   using hitfit::Fourvec;
   using hitfit::Vector_Resolution;
   using hitfit::Column_Vector;

   iEvent.getByToken( _chisqsrc  , _recoHandle  );
   std::auto_ptr<RecoResult>  conreco( new RecoResult );
   *conreco = *(_recoHandle );

   // Variables for running hit fit
   bool   solveNeutrino;
   double fittedWHadronMass;
   double fittedTopMass;
   double fittedTopMass2;
   double topMassUnc;
   Column_Vector pullx;
   Column_Vector pully;
   Lepjets_Event hitFitEvent( 0,0 );

   // Making hitfit Event
   for( const auto& particle : conreco->ParticleList() ){
      TLorentzVector p4 = particle.P4();
      double eta = p4.Eta();
      if( particle.TypeFromFit() == electron_label ){
         Fourvec           fit_p4( p4.Px(), p4.Py(), p4.Pz(), p4.Energy() );
         Vector_Resolution res = _electronResolution.GetResolution( eta );
         hitFitEvent.add_lep( hitfit::Lepjets_Event_Lep(fit_p4 , 0 , res) );
      }
      else if( particle.TypeFromFit() == muon_label ){
         Fourvec           fit_p4( p4.Px(), p4.Py(), p4.Pz(), p4.Energy() );
         Vector_Resolution res = _muonResolution.GetResolution( eta );
         hitFitEvent.add_lep( hitfit::Lepjets_Event_Lep(fit_p4 , 0 , res) );
      }
      else if( particle.TypeFromFit() == neutrino_label ){
         hitFitEvent.met()    = Fourvec( p4.Px(), p4.Py(), 0 , p4.Pt() );
         hitFitEvent.kt_res() = _met_KtResolution;
      }
      else {
         Fourvec jetp4( p4.Px() , p4.Py() , p4.Pz(), p4.Energy() );
         Vector_Resolution jet_resolution ;
         if( particle.TypeFromFit()==lepb_label || particle.TypeFromFit()==hadb_label ){
            jet_resolution = _bJetResolution.GetResolution( eta );
         } else {
            jet_resolution = _lightJetResolution.GetResolution( eta );
         }
         hitFitEvent.add_jet( hitfit::Lepjets_Event_Jet(jetp4 , particle.TypeFromFit() , jet_resolution) );
      }
   }

   _top_fitter.fit_one_perm(
      hitFitEvent, solveNeutrino,
      fittedWHadronMass, fittedTopMass, fittedTopMass2, topMassUnc,
      pullx, pully
   );

   conreco->GetParticle(electron_label).P4(fitted)=ConvertToRoot( hitFitEvent.lep(0).p() );
   conreco->GetParticle(muon_label    ).P4(fitted)=ConvertToRoot( hitFitEvent.lep(0).p() );
   conreco->GetParticle(neutrino_label).P4(fitted)=ConvertToRoot( hitFitEvent.met() );
   for( int i = lepb_label ; i <= hadg_label ; ++i ){
      if( i == higgs_label ) { continue; }
      Particle_Label lab = (Particle_Label)(i);
      conreco->GetParticle(lab).P4(fitted) = ConvertToRoot( hitFitEvent.sum(i) );
   }
   conreco->TstarMass() = conreco->ComputeFromPaticleList();

   iEvent.put( conreco );
}

DEFINE_FWK_MODULE(ConstrainChiSq);

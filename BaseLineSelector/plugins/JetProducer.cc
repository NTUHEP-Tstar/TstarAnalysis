/*******************************************************************************
*
*  Filename    : JetProducer.cc
*  Description : Main control flow and FW interaction
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*  For helper functions, see src/JetSelection.cc
*
*******************************************************************************/
#include "TstarAnalysis/BaseLineSelector/interface/JetProducer.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/TypeDef.hpp"
#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"

using namespace tstar;
JetProducer::JetProducer( const edm::ParameterSet& iConfig ) :
   _jetsrc( GETTOKEN( iConfig , JetList, "jetsrc" ) ),
   _muonsrc( GETTOKEN( iConfig, MuonList, "muonsrc" ) ),
   _electronsrc( GETTOKEN( iConfig, ElectronList, "electronsrc" ) ),
   _rhosrc( GETTOKEN( iConfig, double, "rhosrc" ) ),
   _minjet( iConfig.getParameter<int>("minjet") )
{
   produces<JetList>();
}

JetProducer::~JetProducer()
{
}

// ------------------------------------------------------------------------------
//   Main control flow
// ------------------------------------------------------------------------------
bool
JetProducer::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   iEvent.getByToken( _jetsrc,      _jetHandle      );
   iEvent.getByToken( _muonsrc,     _muonHandle     );
   iEvent.getByToken( _electronsrc, _electronHandle );
   iEvent.getByToken( _rhosrc,      _rhoHandle      );

   // Getting jet Energy Correction information
   iSetup.get<JetCorrectionsRecord>().get( "AK4PF", _jetcorHandle );// Tailored for AK4 Jets
   auto& jetcor = *_jetcorHandle;
   _jecunc.reset( new JetCorrectionUncertainty( jetcor["Uncertainty"] ) );

   // Getting Jet Resolution information
   _jetptres.reset( new JME::JetResolution( JME::JetResolution::get( iSetup, "AK4PFchs_pt" ) ) );
   _jetphires.reset( new JME::JetResolution( JME::JetResolution::get( iSetup, "AK4PFchs_phi" ) ) );
   _jetressf.reset( new JME::JetResolutionScaleFactor( JME::JetResolutionScaleFactor::get( iSetup, "AK4PFchs" ) ) );

   // New JetList to store jet objects
   std::auto_ptr<JetList> selectedJets( new JetList );

   for( const auto& jet : *_jetHandle ){
      // Skipping minimal jets to avoid selection effecting results
      if( jet.pt() < 20 || fabs( jet.eta() ) > 3.0 ){ continue; }

      // Main selection, see src/JetSelection.cc
      if( IsSelectedJet( jet, iEvent.isRealData() ) ){
         selectedJets->push_back( jet );
      }
   }

   // Jet Selection criteria
   if( selectedJets->size() < _minjet ){ return false; }

   // Caching and storing jets
   for( auto& jet : *selectedJets ){
      AddJetVariables( jet, iEvent.isRealData() );
   }
   iEvent.put( selectedJets );

   // Passing the final results : passed jet selection criteria
   return true;
}

// ------------------------------------------------------------------------------
//   EDM Plugin requirements
// ------------------------------------------------------------------------------
void
JetProducer::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
   // The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( JetProducer );

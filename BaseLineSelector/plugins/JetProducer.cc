/*******************************************************************************
 *
 *  Filename    : JetProducer.cc
 *  Description : Main control flow and FW interaction Definition
 *  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 *
 *  For helper functions, see src/JetSelection.cc
 *
*******************************************************************************/
#include "TstarAnalysis/BaseLineSelector/interface/JetProducer.hpp"

//------------------------------------------------------------------------------
//   Constructor and destructor
//------------------------------------------------------------------------------
JetProducer::JetProducer(const edm::ParameterSet& iConfig):
   _jetsrc     ( consumes<JetList>     (iConfig.getParameter<edm::InputTag>("jetsrc"))),
   _muonsrc    ( consumes<MuonList>    (iConfig.getParameter<edm::InputTag>("muonsrc"))),
   _electronsrc( consumes<ElectronList>(iConfig.getParameter<edm::InputTag>("electronsrc"))),
   _rhosrc     ( consumes<double>      (iConfig.getParameter<edm::InputTag>("rhosrc"))),
   _btagcheck  ("check" , edm::FileInPath("TstarAnalysis/Common/data/CSVv2_ichep.csv").fullPath() )
{
   produces<JetList> ();
}

JetProducer::~JetProducer()
{
}

//------------------------------------------------------------------------------
//   Main control flow
//------------------------------------------------------------------------------
bool JetProducer::filter( edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   iEvent.getByToken( _jetsrc      , _jetHandle      );
   iEvent.getByToken( _muonsrc     , _muonHandle     );
   iEvent.getByToken( _electronsrc , _electronHandle );
   iEvent.getByToken( _rhosrc      , _rhoHandle      );

   std::auto_ptr<JetList> selectedJets( new JetList );

   // Getting jet Energy Correction information
   iSetup.get<JetCorrectionsRecord>().get("AK4PF",_jetcorHandle);  // Tailored for AK4 Jets
   _jecunc.reset( new JetCorrectionUncertainty( (*_jetcorHandle)["Uncertainty"] ) );
   // Getting Jet Resolution information
   _jetptres .reset( new JME::JetResolution( JME::JetResolution::get(iSetup, "AK4PFchs_pt") ) );
   _jetphires.reset( new JME::JetResolution( JME::JetResolution::get(iSetup, "AK4PFchs_phi") ) );
   _jetressf .reset( new JME::JetResolutionScaleFactor( JME::JetResolutionScaleFactor::get(iSetup, "AK4PFchs") ) );

   unsigned num_bjets = 0;
   for( const auto& jet : *_jetHandle ) {
      // Skipping minimal jets to avoid selection effecting results
      if( jet.pt() < 20 || fabs(jet.eta()) > 3.0 ) { continue; }

      // Main selection, see src/JetSelection.cc
      if( IsSelectedJet(jet,iEvent.isRealData())  ){
         selectedJets->push_back(jet);
         if( _btagcheck.PassMedium(jet) && selectedJets->size() <= 6 ){
            ++num_bjets; // only counting number of b-jets in leading six jets
         }
      }
   }

   // Jet Selection criteria
   if( selectedJets->size() < 6 ){ return false; }
   if( num_bjets < 1 ){ return false; }

   // Caching and storing jets
   for( auto& jet : *selectedJets ){
      AddJetVariables( jet, iEvent.isRealData() );
   }
   iEvent.put( selectedJets );

   // Passing the final results : passed jet selection criteria
   return true;
}



//------------------------------------------------------------------------------
//   EDM Plugin requirements
//------------------------------------------------------------------------------

void
JetProducer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
   //The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(JetProducer);

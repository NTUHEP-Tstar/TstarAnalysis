/*******************************************************************************
*
*  Filename    : ElectronProducer.cc
*  Description : Defining function directly related with edm file interaction
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/ElectronProducer.hpp"
#include "TstarAnalysis/BaseLineSelector/interface/TypeDef.hpp"

using namespace tstar;
ElectronProducer::ElectronProducer( const edm::ParameterSet& iconf ) :
   _electronsrc( GETTOKEN( iconf, ElectronList, "electronsrc" ) ),
   _vetoMapToken( GETTOKEN( iconf, edm::ValueMap<bool>, "vetoMap" ) ),
   _looseMapToken( GETTOKEN( iconf, edm::ValueMap<bool>, "looseMap" ) ),
   _mediumMapToken( GETTOKEN( iconf, edm::ValueMap<bool>, "mediumMap" ) ),
   _tightMapToken( GETTOKEN( iconf, edm::ValueMap<bool>, "tightMap" ) ),
   _packedsrc( GETTOKEN( iconf, PackedCandList, "packedsrc" ) ),
   _hltsrc( GETTOKEN( iconf, TriggerResults, "hltsrc " ) ),
   _triggerobjsrc( GETTOKEN( iconf, TriggerObjList, "trgobjsrc" ) ),
   _reqtrigger( iconf.getParameter<std::string>( "reqtrigger" ) )
{
   produces<ElectronList>();
}

ElectronProducer::~ElectronProducer()
{
}

bool
ElectronProducer::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   iEvent.getByToken( _electronsrc,    _electronHandle  );
   iEvent.getByToken( _vetoMapToken,   _vetoMapHandle   );
   iEvent.getByToken( _looseMapToken,  _looseMapHandle  );
   iEvent.getByToken( _mediumMapToken, _mediumMapHandle );
   iEvent.getByToken( _tightMapToken,  _tightMapHandle  );
   iEvent.getByToken( _packedsrc,      _packedHandle    );
   iEvent.getByToken( _hltsrc,         _hltHandle       );
   iEvent.getByToken( _triggerobjsrc,  _trigobjHandle   );

   std::auto_ptr<ElectronList> selectedElectrons( new ElectronList );

   for( size_t i = 0; i < _electronHandle->size(); ++i ){
      pat::Electron el = _electronHandle->at( i );
      const edm::Ptr<pat::Electron> elPtr( _electronHandle, i );
      if( IsSelectedElectron( el, elPtr, iEvent ) ){
         if( selectedElectrons->empty() ){
            selectedElectrons->push_back( el );
         } else {
            return false;
         }
      } else if( IsVetoElectron( el, elPtr, iEvent ) ){
         return false;
      }
   }

   // Adding variable to final electron
   if( selectedElectrons->size() == 1 ){
      AddElectronVariables( selectedElectrons->at( 0 ), iEvent );
   } else if( selectedElectrons->size() > 1 ){
      return false;
   }

   iEvent.put( selectedElectrons );

   return true;
}



// ------------------------------------------------------------------------------
//   EDM Plugin requirements
// ------------------------------------------------------------------------------

void
ElectronProducer::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
   // The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( ElectronProducer );

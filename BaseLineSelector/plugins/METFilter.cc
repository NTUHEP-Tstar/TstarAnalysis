/*******************************************************************************
*
*  Filename    : METFilter
*  Description : Filtering by METFilter
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"
#include <memory>

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include <iostream>
#include <vector>

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"


using namespace std;

class METFilter : public edm::stream::EDFilter<>
{
public:
   explicit
   METFilter( const edm::ParameterSet& );
   ~METFilter();
   static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
   virtual bool filter( edm::Event&, const edm::EventSetup& ) override;

   // Data Members
   const edm::EDGetToken _badmusrc;
   const edm::EDGetToken _badchhadsrc;
   const edm::EDGetToken _recotrgsrc;
   edm::Handle<bool> _badmuhandle;
   edm::Handle<bool> _badchhadhandle;
   edm::Handle<edm::TriggerResults> _recotrghandle;

   const vector<string> _checklist;
};

METFilter::METFilter( const edm::ParameterSet& iconf ) :
   _badmusrc( GETTOKEN( iconf, bool, "badmusrc" ) ),
   _badchhadsrc( GETTOKEN( iconf, bool, "badchhadsrc" ) ),
   _recotrgsrc( GETTOKEN( iconf, edm::TriggerResults, "recotrgsrc" ) ),
   _checklist( // taken from
      // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#What_is_available_in_MiniAOD
{
   "Flag_HBHENoiseFilter",
   "Flag_HBHENoiseIsoFilter",
   "Flag_EcalDeadCellTriggerPrimitiveFilter",
   "Flag_goodVertices",
   "Flag_eeBadScFilter",
   "Flag_globalTightHalo2016Filter"
} )
{
}

bool
METFilter::filter( edm::Event& iEvent, const edm::EventSetup& iSetup )
{
   iEvent.getByToken( _badmusrc,    _badmuhandle );
   iEvent.getByToken( _badchhadsrc, _badchhadhandle );
   iEvent.getByToken( _recotrgsrc,  _recotrghandle );

   // Additional recipe found here:
   // https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2#How_to_run_the_Bad_Charged_Hadro
   if( !*_badmuhandle ){ return false; }
   if( !*_badchhadhandle ){ return false; }

   const auto& triggernamelist = iEvent.triggerNames( *_recotrghandle );

   for( const auto& check : _checklist ){
      const unsigned index = triggernamelist.triggerIndex( check );
      if( index == triggernamelist.size() ){ continue; }// skipping For MC samples
      // Traditional trigger handling
      if( !_recotrghandle->wasrun( index ) ){ return false; }
      if( !_recotrghandle->accept( index ) ){ return false; }
      if( _recotrghandle->error( index ) ){ return false; }
   }

   return true;
}


/*******************************************************************************
*   Generic Plugin requirements
*******************************************************************************/
METFilter::~METFilter()
{
}


void
METFilter::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
   // The following says we do not know what parameters are allowed so do no validation
   // Please change this to state exactly what you do use, even if it is no parameters
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault( desc );
}

// define this as a plug-in
DEFINE_FWK_MODULE( METFilter );

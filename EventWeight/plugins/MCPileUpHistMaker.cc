/*******************************************************************************
*
*  Filename    : MCPileUpHistMaker.cc
*  Description : Filling a histrogram from a MC sample
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

#include "ManagerUtils/EDMUtils/interface/PluginAlias.hpp"

#include "TH1D.h"
#include <iostream>
#include <memory>
#include <vector>

/*******************************************************************************
*   Class definitions
*******************************************************************************/
class MCPileUpHistMaker :
   public edm::one::EDAnalyzer<edm::one::SharedResources>
{
public:
   explicit
   MCPileUpHistMaker( const edm::ParameterSet& );
   ~MCPileUpHistMaker();

   static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

private:
   virtual void beginJob() override;
   virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
   virtual void endJob() override;

   // ----------member data ---------------------------
   const edm::EDGetToken _lhesrc;
   const edm::EDGetToken _pusrc;
   edm::Handle<LHEEventProduct> _lhehandle;
   edm::Handle<std::vector<PileupSummaryInfo> > _puhandle;

   TH1D* _pileuphist;
};

/*******************************************************************************
*   Constructor and destructor
*******************************************************************************/
MCPileUpHistMaker::MCPileUpHistMaker( const edm::ParameterSet& iConfig ) :
   _lhesrc( GETTOKEN( iConfig, LHEEventProduct, "lhesrc" ) ),
   _pusrc( GETTOKEN( iConfig, std::vector<PileupSummaryInfo>, "pusrc" ) )
{
   edm::Service<TFileService> _fs;
   _pileuphist = _fs->make<TH1D>( "pu", "pu", 50, 0, 50 );
}


MCPileUpHistMaker::~MCPileUpHistMaker()
{
   std::cout << _pileuphist->Integral() << std::endl;
}

/*******************************************************************************
*   Main control flow
*******************************************************************************/
void
MCPileUpHistMaker::analyze(
   const edm::Event&      iEvent,
   const edm::EventSetup& iSetup )
{
   iEvent.getByToken( _lhesrc, _lhehandle );
   iEvent.getByToken( _pusrc,  _puhandle  );

   const double weight =
      ( _lhehandle.isValid() && _lhehandle->hepeup().XWGTUP > 0  ) ?
      +1 : -1;
   const unsigned pu = _puhandle->at( 0 ).getPU_NumInteractions();

   _pileuphist->Fill( pu, weight );
}


/******************************************************************************/

void
MCPileUpHistMaker::beginJob()
{
}

/******************************************************************************/

void
MCPileUpHistMaker::endJob()
{
   std::cout << _pileuphist->Integral() << std::endl;
}

/******************************************************************************/

void
MCPileUpHistMaker::fillDescriptions( edm::ConfigurationDescriptions& descriptions )
{
   edm::ParameterSetDescription desc;
   desc.setUnknown();
   descriptions.addDefault( desc );
}

/******************************************************************************/

DEFINE_FWK_MODULE( MCPileUpHistMaker );

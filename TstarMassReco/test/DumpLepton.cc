/*******************************************************************************
*
*  Filename    : DumpLepton.cc
*  Description : Dumping lepton contents
*  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
*
*******************************************************************************/
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/FWLite/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"

#include "TFile.h"

#include <iostream>
#include <vector>

using namespace std;

int
main( int argc, char* argv[] )
{
  fwlite::Event event( TFile::Open( "./signal_input.root" ) );
  fwlite::Handle<vector<pat::Electron> > elec_handle;
  fwlite::Handle<vector<pat::Muon> > muon_handle;

  unsigned i = 0;

  for( event.toBegin(); !event.atEnd() && i < 100; ++event, ++i ){
    cout << "Event [" << i << "]" << endl;
    elec_handle.getByLabel( event, "skimmedPatElectrons" );
    muon_handle.getByLabel( event, "skimmedPatMuons" );
    const auto& elec_list = *( elec_handle );
    const auto& muon_list = *( muon_handle );

    cout << elec_list.size() << endl;
    cout << muon_list.size() << endl;
  }

  return 0;
}

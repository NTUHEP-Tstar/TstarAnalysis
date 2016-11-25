#*******************************************************************************
 #
 #  Filename    : Producer_cfi.py
 #  Description : Defining preset values for baseline object selection Producers
 #  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
 #
 #  Find magic filter labels from https://cmsweb.cern.ch/confdb/
 #
#*******************************************************************************

import FWCore.ParameterSet.Config as cms

#-------------------------------------------------------------------------
#   Muon Settings
#-------------------------------------------------------------------------
selectedMuons = cms.EDFilter(
    "MuonProducer",
    vertexsrc  = cms.InputTag("offlineSlimmedPrimaryVertices"),
    muonsrc    = cms.InputTag("slimmedMuons"),
    packedsrc  = cms.InputTag("packedPFCandidates"),
    rhosrc     = cms.InputTag("fixedGridRhoFastjetAll"),
)

skimmedPatMuons = cms.EDFilter(
    "PATMuonSelector",
    src=cms.InputTag("selectedMuons"),
    cut=cms.string("")
)

#-------------------------------------------------------------------------
#   Electron Settings
#-------------------------------------------------------------------------
selectedElectrons = cms.EDFilter(
    "ElectronProducer",
    electronsrc = cms.InputTag("slimmedElectrons"),
    packedsrc   = cms.InputTag("packedPFCandidates"),
    vertexsrc   = cms.InputTag("offlineSlimmedPrimaryVertices"),
    vetoMap     = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-veto" ),
    looseMap    = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-loose" ),
    mediumMap   = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-medium" ),
    tightMap    = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-V1-tight" ),
    heepMap     = cms.InputTag( "egmGsfElectronIDs:heepElectronID-HEEPV60"),
    hltMap      = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronHLTPreselection-Summer16-V1" ),
)

skimmedPatElectrons = cms.EDFilter(
    "PATElectronSelector",
    src=cms.InputTag("selectedElectrons"),
    cut=cms.string("")
)

#-------------------------------------------------------------------------
#   Jet Settings
#-------------------------------------------------------------------------
selectedJets = cms.EDFilter(
    "JetProducer",
    jetsrc      = cms.InputTag("slimmedJets"),
    muonsrc     = cms.InputTag("selectedMuons"),
    electronsrc = cms.InputTag("selectedElectrons"),
    rhosrc      = cms.InputTag("fixedGridRhoFastjetAll"),
    minjet      = cms.int32(4)
)

skimmedPatJets = cms.EDFilter(
    "PATJetSelector",
    src=cms.InputTag("selectedJets"),
    cut=cms.string("")
)

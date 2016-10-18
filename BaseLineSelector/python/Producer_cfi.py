#*******************************************************************************
 #
 #  Filename    : Producer_cfi.py
 #  Description : Defining preset values for Producers
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
    hltsrc     = cms.InputTag("TriggerResults::HLT"),
    trgobjsrc  = cms.InputTag("selectedPatTrigger"),
    reqtrigger = cms.string('HLT_IsoMu27_v*'),
    reqfilter  = cms.string( "hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p09"),
    runtrigger = cms.bool(False) # Disable trigger filter by default
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
    hltsrc      = cms.InputTag("TriggerResults::HLT"),
    trgobjsrc   = cms.InputTag("selectedPatTrigger"),
    vetoMap     = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-veto" ),
    looseMap    = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-loose" ),
    mediumMap   = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-medium" ),
    tightMap    = cms.InputTag( "egmGsfElectronIDs:cutBasedElectronID-Spring15-25ns-V1-standalone-tight" ),
    reqtrigger  = cms.string("HLT_Ele27_WPTight_Gsf_v*"),  # requireing no trigger by default
    reqfilter   = cms.string('hltEle27WPTightGsfTrackIsoFilter'),
    runtrigger  = cms.bool(False)
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

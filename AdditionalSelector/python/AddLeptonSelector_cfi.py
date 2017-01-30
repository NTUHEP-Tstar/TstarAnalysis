import FWCore.ParameterSet.Config as cms

ElectronSelector = cms.EDFilter(
    'AddElectronSelector',
    rhosrc      = cms.InputTag("fixedGridRhoFastjetAll"),
    electronsrc = cms.InputTag("skimmedPatElectrons"),
    hltsrc      = cms.InputTag("TriggerResults::HLT"),
    trgobjsrc   = cms.InputTag("selectedPatTrigger"),
    reqtrigger  = cms.VPSet(
        cms.PSet(
            hltname    = cms.string('HLT_Ele32_eta2p1_WPTight_Gsf_v*'),
            filtername = cms.string('hltEle32WPTightGsfTrackIsoFilter'),
        )
    )
)


MuonSelector = cms.EDFilter(
    'AddMuonSelector',
    muonsrc    = cms.InputTag("skimmedPatMuons"),
    hltsrc     = cms.InputTag("TriggerResults::HLT"),
    trgobjsrc  = cms.InputTag("selectedPatTrigger"),
    reqtrigger = cms.VPSet(
        cms.PSet(
            hltname    = cms.string('HLT_IsoMu24_v*'),
            filtername = cms.string( "hltL3crIsoL1sMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p09"),
        ),
        cms.PSet(
            hltname    = cms.string('HLT_IsoTkMu24_v*'),
            filtername = cms.string('hltL3fL1sMu22L1f0Tkf24QL3trkIsoFiltered0p09')
        )
    )
)

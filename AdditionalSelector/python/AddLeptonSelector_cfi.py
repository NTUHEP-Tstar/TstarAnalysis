import FWCore.ParameterSet.Config as cms

ElectronSelector = cms.EDFilter(
    'AddElectronSelector',
    rhosrc      = cms.InputTag("fixedGridRhoFastjetAll"),
    electronsrc = cms.InputTag("skimmedPatElectrons"),
    hltsrc      = cms.InputTag("TriggerResults::HLT"),
    trgobjsrc   = cms.InputTag("selectedPatTrigger"),
    reqtrigger  = cms.VPSet(
        cms.PSet(
            hltname    = cms.string('HLT_Ele27_WPTight_Gsf_v*'),
            filtername = cms.string('hltEle27WPTightGsfTrackIsoFilter'),
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
            hltname    = cms.string('HLT_IsoMu27_v*'),
            filtername = cms.string( "hltL3crIsoL1sMu22Or25L1f0L2f10QL3f27QL3trkIsoFiltered0p09"),
        ),
        cms.PSet(
            hltname = cms.string('HLT_IsoTkMu27_v*'),
            filtername = cms.string('hltL3fL1sMu22Or25L1f0Tkf27QL3trkIsoFiltered0p09')
        )
    )
)

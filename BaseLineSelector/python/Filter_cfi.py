import FWCore.ParameterSet.Config as cms

tstarFilter = cms.EDFilter(
    "BaseLineSelector",
    muonsrc     = cms.InputTag( "selectedMuons"  ),
    electronsrc = cms.InputTag( "selectedElectrons" ),
    jetsrc      = cms.InputTag( "selectedJets" ),
)

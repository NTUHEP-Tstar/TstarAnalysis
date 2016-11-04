import FWCore.ParameterSet.Config as cms

EventWeightNoBtagNoTop = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("PileupWeight"),
        cms.InputTag("ElectronWeight", "ElectronWeight"),
        cms.InputTag("MuonWeight", "MuonWeight"),
        cms.InputTag("PileupWeight", "PileupWeight")
    )
)

EventWeightNoTop = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("PileupWeight"),
        cms.InputTag("ElectronWeight", "ElectronWeight"),
        cms.InputTag("MuonWeight", "MuonWeight"),
        cms.InputTag("PileupWeight", "PileupWeight"),
        cms.InputTag("BtagWeight", "BtagWeight")
    )
)

EventWeightNoBtag = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("PileupWeight"),
        cms.InputTag("ElectronWeight", "ElectronWeight"),
        cms.InputTag("MuonWeight", "MuonWeight"),
        cms.InputTag("PileupWeight", "PileupWeight"),
        cms.InputTag("BtagWeight", "BtagWeight"),
        cms.InputTag("TopPtWeight", "TopPtWeight")
    )
)

EventWeight = cms.EDProducer(
    "WeightProdSum",
    weightlist=cms.VInputTag(
        cms.InputTag("PileupWeight"),
        cms.InputTag("ElectronWeight", "ElectronWeight"),
        cms.InputTag("MuonWeight", "MuonWeight"),
        cms.InputTag("PileupWeight", "PileupWeight"),
        cms.InputTag("BtagWeight", "BtagWeight"),
        cms.InputTag("TopPtWeight", "TopPtWeight")
    )
)

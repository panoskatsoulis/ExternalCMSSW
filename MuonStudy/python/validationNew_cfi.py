import FWCore.ParameterSet.Config as cms

validation = cms.EDAnalyzer(
    "Validation",
    system = cms.string("BMTF"),
    phiHits = cms.InputTag("bmtfDigis"),
    etaHits = cms.InputTag("bmtfDigis"),
    muonsData = cms.InputTag("bmtfDigis:BMTF"),
    muonsEmu = cms.InputTag("simBmtfDigis:BMTF"),
    muonsReco = cms.InputTag("muons")
)

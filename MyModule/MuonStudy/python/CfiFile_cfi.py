import FWCore.ParameterSet.Config as cms

muonStudy = cms.EDAnalyzer(
    "MuonStudy",
    system = cms.string("BMTF"),
    phiHits = cms.InputTag("bmtfDigis"),
    etaHits = cms.InputTag("bmtfDigis"),
    muonsData = cms.InputTag("bmtfDigis:BMTF"),
    muonsEmu = cms.InputTag("simBmtfDigis:BMTF")
)

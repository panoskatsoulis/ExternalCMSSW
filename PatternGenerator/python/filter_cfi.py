import FWCore.ParameterSet.Config as cms

evfilter = cms.EDFilter(
    "eventFLTR",
    muonsData = cms.InputTag("bmtfDigis:BMTF"),
    muonsEmu = cms.InputTag("simKBmtfDigis:BMTF"),
)

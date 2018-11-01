import FWCore.ParameterSet.Config as cms

process = cms.Process('test')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


#########  INPUT FILES  #####################
import glob, os

events = 20000

dataset = "/ExpressPhysics/Run2018D-Express-v1/FEVT"
gTag = "101X_dataRun2_HLT_v7"
legacyTag = "BMTF"
kalmanTag = "BMTF2"
lumiBegin = "20"
lumiEnd = "60"

query_type = "single" # single/multiple
run = 325099#if single query
run1 = 323390
run2 = 323399

das_queries = []
files = cms.untracked.vstring()

if query_type == "single" : #### SINGLE DAS query
    das_query = 'dasgoclient --query="file dataset='+dataset+' run='+str(run)+'"'
    print ("Querying files for the Run " + str(run) + '...')
    query_out = os.popen(das_query)
    files += cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in query_out)
    das_queries.append(das_query)
elif query_type == "multiple" : #### Multiple DAS queries to form the files vector
    for run_i in range(run1, run2+1):
        das_query = 'dasgoclient --query="file dataset='+dataset+' run='+str(run_i)+'"'
        print ("Querying files for the Run " + str(run_i) + '...')
        query_out = os.popen(das_query)
        files += cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in query_out)
        das_queries.append(das_query)
else :
    print("query_type for DAS unknown.")
    print("query_type: "+query_type)
    exit(1)

print files
print ("DAS queries performed:")
print das_queries

process.source = cms.Source (
    #"NewEventStreamFileReader",
    "PoolSource",
    fileNames = files,
    #cms.untracked.vstring (
        #skipEvents=cms.untracked.uint32(options.skipEvents)

#    lumisToProcess = cms.untracked.VLuminosityBlockRange(str(str(run)+':'+lumiBegin+'-'+str(run)+':'+lumiEnd if query_type == "single" else ''))
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(events)
)

# Message Logger
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(500)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

# Global Tag
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, gTag, '')

# TFileService for output
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string('validation_run'+str(run if query_type == "single" else 'Multiple')
                          +'_'+str(events if events >= 0 else 'all')+'Events.root')
    )

# load Unpacker
process.load('EventFilter.L1TRawToDigi.bmtfDigis_cfi')
#process.load("EventFilter.L1TXRawToDigi.twinMuxStage2Digis_cfi")

# load Emulator
process.load('L1Trigger.L1TMuonBarrel.simKBmtfStubs_cfi')
process.simKBmtfStubs.srcPhi = cms.InputTag("bmtfDigis")
process.simKBmtfStubs.srcTheta = cms.InputTag("bmtfDigis")
process.load('L1Trigger.L1TMuonBarrel.simKBmtfDigis_cfi')#Kalman
process.load('L1Trigger.L1TMuonBarrel.simBmtfDigis_cfi')#BMTF
process.load('L1Trigger.L1TMuonBarrel.fakeBmtfParams_cff')
process.simBmtfDigis.DTDigi_Source = cms.InputTag("bmtfDigis")
process.simBmtfDigis.DTDigi_Theta_Source = cms.InputTag("bmtfDigis")
process.esProd = cms.EDAnalyzer("EventSetupRecordDataGetter",
   toGet = cms.VPSet(
      cms.PSet(record = cms.string('L1TMuonBarrelParamsRcd'),
               data = cms.vstring('L1TMuonBarrelParams'))
                   ),
   verbose = cms.untracked.bool(True)
)

# Configure Emulator's masks
masked = '111111111111'
no = '000000000000'
process.fakeBmtfParams.mask_phtf_st1 = cms.vstring(masked, no, no, no, no, no, masked)
process.fakeBmtfParams.mask_phtf_st2 = cms.vstring(no, no, no, no, no, no, no)
process.fakeBmtfParams.mask_phtf_st3 = cms.vstring(no, no, no, no, no, no, no)
process.fakeBmtfParams.mask_phtf_st4 = cms.vstring(no, no, no, no, no, no, no)
process.fakeBmtfParams.mask_ettf_st1 = cms.vstring(no, no, no, no, no, no, no)
process.fakeBmtfParams.mask_ettf_st2 = cms.vstring(no, no, no, no, no, no, no)
process.fakeBmtfParams.mask_ettf_st3 = cms.vstring(no, no, no, no, no, no, no)


# load Validator
process.load('ExternalCMSSW.MuonStudy.validationNew_cfi')
process.validation.muonsData = cms.InputTag("bmtfDigis:"+legacyTag)
process.validation2 = process.validation.clone(
    system = cms.string("KMTF"),
    muonsData = cms.InputTag("bmtfDigis:"+kalmanTag),
    muonsEmu = cms.InputTag("simKBmtfDigis:BMTF")
    )
#process.muonStudy.phiHits = cms.InputTag("twinMuxStage2Digis:PhIn")#info to be used only for print out
#process.muonStudy.etaHits = cms.InputTag("twinMuxStage2Digis:ThIn")#info to be used only for print out


# Path and EndPath definitions
process.path = cms.Path(
    process.bmtfDigis              #unpack BMTF
    #+process.twinMuxStage2Digis   #unpack TM
    +process.esProd                #event setup creation
    +process.simBmtfDigis          #emulation
    +process.simKBmtfStubs
    +process.simKBmtfDigis
    +process.validation            #BMTF validation
    +process.validation2           #KMTF validation
)

# Output definition
process.output = cms.OutputModule(
    "PoolOutputModule",
#    splitLevel = cms.untracked.int32(0),
#    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = cms.untracked.vstring("drop *",
                                           "keep *_*_*_test"),
    fileName = cms.untracked.string('output.root'),
#    dataset = cms.untracked.PSet(
#        filterName = cms.untracked.string(''),
#        dataTier = cms.untracked.string('')
#    )
)

process.out = cms.EndPath(
#    process.output
)

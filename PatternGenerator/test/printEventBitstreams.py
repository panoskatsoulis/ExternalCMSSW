import FWCore.ParameterSet.Config as cms

process = cms.Process('test')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

#Setup FWK for multithreaded - The Validator is not thread-safe
process.options = cms.untracked.PSet()
# process.options.numberOfThreads=cms.untracked.uint32(8)
# process.options.numberOfStreams=cms.untracked.uint32(0)

#########  INPUT FILES  #####################
import glob, os

events = 100000
print_freq = 200
dataset = "/Muon0/Run2023C-v1/RAW"
run = "368670"
gTag = "_dummy_"
legacyTag = "BMTF2"
kalmanTag = "BMTF"
lumiBegin = "1"
lumiEnd = "1000"


#### SINGLE DAS query
das_query = 'dasgoclient --query="file dataset='+dataset+' run='+run+'"'
files = os.popen(das_query)
files = cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in files)
print(files)
print(das_query)

##### Multiple DAS queries to form the files vector
# files1 = os.popen('dasgoclient --query="file dataset=/ZeroBias/Run2018D-v1/RAW run=320612"')
# files1 = cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in files1)

# files2 = os.popen('dasgoclient --query="file dataset=/JetHT/Run2018D-v1/RAW run=320612"')
# files2 = cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in files2)

# files3 = os.popen('dasgoclient --query="file dataset=/MET/Run2018D-v1/RAW run=320612"')
# files3 = cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in files3)

# files4 = os.popen('dasgoclient --query="file dataset=/DoubleMuon/Run2018D-v1/RAW run=320612"')
# files4 = cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in files4)

# files = files1 + files2 + files3 + files4
# print files

process.source = cms.Source (
    #"NewEventStreamFileReader",
    #fileNames = files2 + files1
    "PoolSource",
    fileNames = files,
    #cms.untracked.vstring (
        #skipEvents=cms.untracked.uint32(options.skipEvents)

    lumisToProcess = cms.untracked.VLuminosityBlockRange(run+':'+lumiBegin+'-'+run+':'+lumiEnd)
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(events)
)

# Message Logger
process.load("FWCore.MessageLogger.MessageLogger_cfi")

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(print_freq)
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

#process.dumpRaw = cms.EDAnalyzer(
#    "DumpFEDRawDataProduct",
    #label = cms.untracked.string("caloStage1Raw"),
    #feds = cms.untracked.vint32 ( 1352 ),
    #dumpPayload = cms.untracked.bool ( options.dumpRaw )
#)

# enable debug message logging for our modules
# process.MessageLogger = cms.Service(
#     "MessageLogger",
#     threshold  = cms.untracked.string('INFO'),
#     categories = cms.untracked.vstring('L1T'),
# #    debugModules = cms.untracked.vstring(
# #        'mp7BufferDumpToRaw',
# #        'l1tDigis',
# #        'caloStage1Digis'
# #    )
# )

# Global Tag
#from Configuration.AlCa.GlobalTag import GlobalTag
#process.GlobalTag = GlobalTag(process.GlobalTag, gTag, '')

# TFileService for output
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string('validation_run'+run+'_'+str(events if events >= 0 else 'all')+'Events.root')
    )

# load Unpacker
process.load('EventFilter.L1TRawToDigi.bmtfDigis_cfi')

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
process.validation.doEfficiency = cms.bool(False)

process.validation2 = process.validation.clone(
    system = cms.string("KMTF"),
    muonsData = cms.InputTag("bmtfDigis:"+kalmanTag),
    muonsEmu = cms.InputTag("simKBmtfDigis:BMTF")
    )
#process.validation.phiHits = cms.InputTag("twinMuxStage2Digis:PhIn")#info to be used only for print out
#process.validation.etaHits = cms.InputTag("twinMuxStage2Digis:ThIn")#info to be used only for print out

# load filter & dumpRaw
process.load('ExternalCMSSW.PatternGenerator.filter_cfi')
process.load('ExternalCMSSW.PatternGenerator.dumpRaw_cfi')

## packer (not needed for patterns)
# process.load('EventFilter.L1TRawToDigi.bmtfStage2Raw_cfi')
# process.bmtfStage2Raw.InputLabel = cms.InputTag("simKBmtfDigis:BMTF")
# process.bmtfStage2Raw.InputLabel2 = cms.InputTag("bmtfDigis")

# Path and EndPath definitions
process.path = cms.Path(
    process.bmtfDigis              #unpack BMTF              # FEDCollection ------> PhiHits, EtaHits (links 0-35), RegMuonCand (DATA) (61, 62)
    #+process.twinMuxStage2Digis   #unpack TM
    +process.esProd                #event setup creation
    #+process.simBmtfDigis          #emulation
    +process.simKBmtfStubs
    +process.simKBmtfDigis                                   # PhiHits, EtaHits ----------> RegMuonCand (EMU)
    +process.evfilter                                        # RegMuonCand(EMU), RegMuonCand(DATA) --------------> BOOL (to continue processing or drop the event)
    #+process.validation            #BMTF validation
    +process.validation2           #KMTF validation          # RegMuonCand(EMU), RegMuonCand(DATA) --------------> validation plots
    +process.dumpRaw                                         # PhiHits, EtaHits --------> Patterns
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

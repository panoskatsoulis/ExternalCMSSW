import FWCore.ParameterSet.Config as cms

process = cms.Process('test')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
#process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
#process.load('Configuration.Geometry.GeometryDB_cff')
#process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
#process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')


#########  INPUT FILES  #####################
import glob, time, os

events = 200000
run = "321887"
lumiBegin = "1"
lumiEnd = "500"

#run 314859, 314856, 314874
# files = (glob.glob("/eos/cms/store/express/Commissioning2018/ExpressPhysics/FEVT/Express-v1/000/314/859/00000/*.root") + 
#          glob.glob("/eos/cms/store/express/Commissioning2018/ExpressPhysics/FEVT/Express-v1/000/314/865/00000/*.root") +
#          glob.glob("/eos/cms/store/express/Commissioning2018/ExpressPhysics/FEVT/Express-v1/000/314/874/00000/*.root") ) 
# files = cms.untracked.vstring('file://'+_file for _file in files)
# print files

#run 314859 local
# files1 = cms.untracked.vstring('file:///afs/cern.ch/user/k/kpanos/work/run314859_Collisions/run314859_ls000'+str(i)+'_streamPhysicsZeroBias1_StorageManager.dat' for i in range (1,9))
# files2 = cms.untracked.vstring('file:///afs/cern.ch/user/k/kpanos/work/run314859_Collisions/run314859_ls00'+str(i)+'_streamPhysicsZeroBias1_StorageManager.dat' for i in range (10,20))


#### SINGLE DAS query
#files = os.popen('dasgoclient --query="file dataset=/Cosmics/Run2018D-v1/RAW run=320830"')
#files = os.popen('dasgoclient --query="file dataset=/Cosmics/Run2018D-v1/RAW run=320829"')
files = os.popen('dasgoclient --query="file dataset=/ExpressPhysics/Run2018D-Express-v1/FEVT run=321887"')
#files = os.popen('dasgoclient --query="file dataset=/ExpressCosmics/Run2018D-Express-v1/FEVT run=321151"')
files = cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in files)
print files
#files = cms.untracked.vstring('file:///afs/cern.ch/user/k/kpanos/public/Cosmics_2018A_316810_RAW.root')

# files = glob.glob("/eos/cms/store/data/Run2018D/Cosmics/RAW/v1/000/320/830/00000/*")
# files = cms.untracked.vstring('file://'+_file for _file in files)
# print files

# files = glob.glob("/afs/cern.ch/user/k/kpanos/work/run316810/*.dat") 
#files = cms.untracked.vstring('file://'+_file for _file in files)
# print files


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

process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(500)
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
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '101X_dataRun2_HLT_v7', '')

# TFileService for output
process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string('validation_run'+run+'_'+str(events if events >= 0 else 'all')+'Events.root')
    )

# load Unpacker
process.load('EventFilter.L1TRawToDigi.bmtfDigis_cfi')
#process.bmtfDigis.InputLabel = cms.InputTag("hltFEDSelector")
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
process.load('MyModule.MuonStudy.CfiFile_cfi')
process.muonStudy2 = process.muonStudy.clone(
    system = cms.string("KMTF"),
    muonsData = cms.InputTag("bmtfDigis:kBMTF"),
    muonsEmu = cms.InputTag("simKBmtfDigis:BMTF")
    )
#process.muonStudy.phiHits = cms.InputTag("twinMuxStage2Digis:PhIn")#info to be used only for print out
#process.muonStudy.etaHits = cms.InputTag("twinMuxStage2Digis:ThIn")#info to be used only for print out

# load Ntuplizers
# process.load("UIoannina.TrUpS.EVRProducer_cfi")#Event INFO
# process.load("UIoannina.TrUpS.L1Producer_cfi")#BMTF ntuplizer UIoannina
# process.load("L1Trigger.L1TNtuples.l1MuonRecoTree_cfi")#RECO ntuplizer Official

# Configure Ntuplizers
##--->L1
# process.L1TProducer.bmtfOutputDigis = cms.InputTag("bmtfDigis:BMTF")
# process.L1TProducer.bmtfInputPhDigis = cms.InputTag("bmtfDigis")
# process.L1TProducer.bmtfInputThDigis = cms.InputTag("bmtfDigis")
##--->L1EMU
# process.L1TProducerEmulator = process.L1TProducer.clone(
 #   bmtfOutputDigis = cms.InputTag("simBmtfDigis:BMTF") )
##--->RECO-data
#process.l1MuonRecoTree.muonTag = cms.untracked.InputTag("muons")
##--->RECO-L1EMU
# process.l1MuonRecoTree_emu = process.l1MuonRecoTree.clone(
#     muonTag = cms.untracked.InputTag("simBmtfDigis:BMTF") )

# Path and EndPath definitions
process.path = cms.Path(
    process.bmtfDigis              #unpack BMTF
    #+process.twinMuxStage2Digis   #unpack TM
    +process.esProd                #event setup creation
    +process.simBmtfDigis          #emulation
    +process.simKBmtfStubs
    +process.simKBmtfDigis
    +process.muonStudy             #BMTF validation
    +process.muonStudy2            #KMTF validation
    ##Ntuplizers
    # +process.EVRTProducer        #event info
    # +process.L1TProducer         #L1
    # +process.L1TProducerEmulator #L1EMU
    # +process.l1MuonRecoTree      #RECO-L1
    # +process.l1MuonRecoTree_emu  #RECO-L1EMU
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

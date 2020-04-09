import FWCore.ParameterSet.Config as cms

process = cms.Process('validation')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

#########  VarParsing  ######################
import FWCore.ParameterSet.VarParsing as VarParsing
args = VarParsing.VarParsing("analysis")
args.register("dataset", "/ExpressPhysics/Run2018D-Express-v1/FEVT",
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.string,
              "Dataset for the DAS query.")
args.register("gTag", "101X_dataRun2_HLT_v7",
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.string,
              "Global Tag to be used.")
args.register("sysTag", "L1TMuonBarrelParams_Stage2v1_hlt",
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.string,
              "System Tag to be used (instead of Global Tag).")
args.register("legacyTag", "BMTF",
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.string,
              "Collection to used by the Validator for the Legacy Algo. ('BMTF': Triggering | 'BMTF2': Secondary)")
args.register("kalmanTag", "BMTF2",
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.string,
              "Collection to used by the Validator for the Kalman Algo. ('BMTF': Triggering | 'BMTF2': Secondary)")
args.register("queryType", "single",
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.string,
              "DAS query type. ('single': one run | 'multiple': more runs | 'file:...': load local file)")
args.register("events", 20000,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.int,
              "Number of events to run over.")
args.register("run", 325113,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.int,
              "Run to process (for single queries).")
args.register("run1", 323390,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.int,
              "First run to process (for multiple queries).")
args.register("run2", 323399,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.int,
              "Last run to process (for multiple queries).")
args.register("isMC", False,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.bool,
              "If true, the given dataset is a MC.")
args.register("doEff", False,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.bool,
              "If true, calculates the BMTF efficiency using the T&P method.")
args.register("limitQuery", 0,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.int,
              "Use this limit for the DAS query. (only single queries)")
args.register("siteQuery", 'all',
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.string,
              "Use this site for the DAS query. (only single queries)")
args.register("addFakeParams", True,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.bool,
              "This bool adds in the process the bmtfFakeParams.")
args.register("getFromCondDB", False,
              VarParsing.VarParsing.multiplicity.singleton,
              VarParsing.VarParsing.varType.bool,
              "This bool indicates that the CondDB will be used.")
args.parseArguments()

if args.addFakeParams and args.getFromCondDB:
    print "The L1TMuonBarrelRcd can be either fetched form CondDB or from the fakeParams script."
    quit(1)

#########  INPUT FILES  #####################
import glob, os

events = args.events

dataset = args.dataset
gTag = args.gTag
legacyTag = args.legacyTag
kalmanTag = args.kalmanTag
lumiBegin = "20" ### NOT FUNCTIONAL
lumiEnd = "60" ### NOT FUNCTIONAL

query_type = args.queryType # single/multiple
run = args.run # if single query
run1 = args.run1
run2 = args.run2
isMC = args.isMC
limitQuery = args.limitQuery
siteQuery = args.siteQuery

das_queries = []
files = cms.untracked.vstring()

if query_type == "single" : #### Single DAS query
    print("Will perform a 'single' DAS query.")
    das_query = 'dasgoclient --query="file dataset='+dataset
    if siteQuery != 'all':
        das_query += ' site='+str(siteQuery)
    if not isMC:
        das_query += ' run='+str(run)+'"'
    else:
        das_query += '"'
    if limitQuery != 0:
        das_query += ' -limit '+str(limitQuery)
    print("Querying files for the Run " + str(run) + '...')
    query_out = os.popen(das_query)
    files += cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in query_out)
#    files += cms.untracked.vstring('root://grid02.physics.uoi.gr/'+_file.strip() for _file in query_out)
    das_queries.append(das_query)
elif query_type == "multiple" : #### Multiple DAS queries to form the files vector
    print("Will perform a 'multiple' DAS query.")
    if isMC:
        print("Multiple DAS query is unsupported for MC samples.")
        quit(1)
    if limitQuery != 0:
        print("Multiple DAS query does not support query limits.")
        quit(1)
    if siteQuery != 'all':
        print("Multiple DAS query does not support query sites.")
        quit(1)
    for run_i in range(run1, run2+1):
        das_query = 'dasgoclient --query="file dataset='+dataset+' run='+str(run_i)+'"'
        print ("Querying files for the Run " + str(run_i) + '...')
        query_out = os.popen(das_query)
        files += cms.untracked.vstring('root://xrootd-cms.infn.it/'+_file.strip() for _file in query_out)
        das_queries.append(das_query)
elif query_type.startswith('file:'):
    print("Will form the files vector from local files.")
    _fs = query_type.replace('file:','').split(';')
    files += cms.untracked.vstring('file:'+_file.strip() for _file in _fs)
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

# # Global Tag
# from Configuration.AlCa.GlobalTag import GlobalTag
# process.GlobalTag = GlobalTag(globaltag=gTag,
#                               conditions='')

if args.getFromCondDB:
    from CondCore.CondDB.CondDB_cfi import CondDB
    CondDB.connect = cms.string("frontier://FrontierPrep/CMS_CONDITIONS")
    process.l1conddb = cms.ESSource("PoolDBESSource", CondDB,
                                    toGet   = cms.VPSet(
                                        cms.PSet(
                                            record = cms.string('L1TMuonBarrelParamsRcd'),
                                            tag = cms.string(args.sysTag)
                                        )
                                    )# toGet
                                )# ESSource


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
process.simBmtfDigis.DTDigi_Source = cms.InputTag("bmtfDigis")
process.simBmtfDigis.DTDigi_Theta_Source = cms.InputTag("bmtfDigis")

# Configure Emulator's masks
if args.addFakeParams:
    process.load('L1Trigger.L1TMuonBarrel.fakeBmtfParams_cff')
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
process.validation.doEfficiency = cms.bool(args.doEff)
process.validation2 = process.validation.clone(
    system = cms.string("KMTF"),
    doEfficiency = cms.bool(args.doEff),
    muonsData = cms.InputTag("bmtfDigis:"+kalmanTag),
    muonsEmu = cms.InputTag("simKBmtfDigis:BMTF")
)
#process.muonStudy.phiHits = cms.InputTag("twinMuxStage2Digis:PhIn")#info to be used only for print out
#process.muonStudy.etaHits = cms.InputTag("twinMuxStage2Digis:ThIn")#info to be used only for print out

# Setup the Rcd Getter
# process.esProd = cms.EDAnalyzer("EventSetupRecordDataGetter",
#    toGet = cms.VPSet(
#       cms.PSet(record = cms.string('L1TMuonBarrelParamsO2ORcd'),
#                data = cms.vstring('L1TMuonBarrelParams'),
#                tag = cms.string("L1TMuonBarrelParams_Stage2v1_hlt"), ## 101X_dataRun2_HLT_v7
#                )
#    ),
#    verbose = cms.untracked.bool(True)
# )


# Setup the L1TMuonBarrelParams Viewer
process.l1brl = cms.EDAnalyzer("L1TMuonBarrelParamsViewer")

# Path and EndPath definitions
process.path = cms.Path(
    #process.esProd                #event setup creation
    process.bmtfDigis              #unpack BMTF
    #+process.twinMuxStage2Digis   #unpack TM
    +process.simBmtfDigis          #emulation
    #+process.simKBmtfStubs
    #+process.simKBmtfDigis
    +process.validation            #BMTF validation
    # +process.validation2           #KMTF validation
    # +process.l1brl                 #MuonBarrelParams Viewer (needs to be at the end of the chain cause the es params will be fetched once they asked.)
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

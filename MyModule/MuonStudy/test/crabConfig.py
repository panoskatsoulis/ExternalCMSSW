
from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

run='PU140_inputRateStudy_fromBxSeperationTime_perStation'

RUN_NUMBER='1' ##run over mc dataset

config.General.requestName = 'sim_'+run
config.General.workArea = 'sim_'+run
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.allowUndistributedCMSSW = True
#config.JobType.maxMemoryMB = 3072
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '/afs/cern.ch/user/k/kpanos/work/CMSSW_releases/CMSSW_9_2_0/src/PackerAnalyzer/PackerCheck/python/ConfFile_cfg.py'
config.JobType.inputFiles = ['/afs/cern.ch/user/k/kpanos/work/CMSSW_releases/CMSSW_9_2_0/src/PackerAnalyzer/']

config.Data.inputDataset = '/SingleMu_FlatPt-8to100/PhaseIISpring17D-PU140_90X_upgrade2023_realistic_v9-v1/GEN-SIM-DIGI-RAW'
#config.Data.secondaryInputDataset=
config.Data.inputDBS = 'global'
config.Data.splitting = 'FileBased' #'LumiBased'
config.Data.unitsPerJob = 50
config.Data.runRange = RUN_NUMBER
config.Data.outLFNDirBase = '/store/user/kpanos/'
config.Data.publication = False

config.Site.storageSite = 'T2_GR_Ioannina' 
config.Site.blacklist = ['T2_US_Vanderbilt']

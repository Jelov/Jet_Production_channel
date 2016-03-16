from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'MC_bJet50_HiForest_allB'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runForestAOD_pp_MC_75X_bjet50.py'

#config.Data.outputPrimaryDataset = 'bJet200'
config.Data.inputDataset = '/Pythia6_TuneZ2_5020GeV/mnguyen-Pythia6_bjet50_TuneZ2_5020GeV_RECO_v2-24bc0f2c47ba7a24fbd3e8494fe06297/USER'
config.Data.inputDBS ='phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 3
#NJOBS = 10  # This is not a configuration parameter, but an auxiliary variable that we use in the next line.
#config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
#config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = 'MC_bJet50_HiForest_allB'

config.Site.storageSite = 'T2_US_Purdue'  # or T2_CH_CERN 
#config.Site.whitelist = ['T2_US_Purdue']


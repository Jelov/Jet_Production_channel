from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

config.General.requestName = 'MC_bJet120_HiForest_test4'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'runForestAOD_pp_MC_75X_bjet120.py'

#config.Data.outputPrimaryDataset = 'bJet200'
#config.Data.inputDataset = '/Pythia8_bjet30_5020GeV_GEN-SIM/mnguyen-Pythia8_bjet30_5020GeV_RECO_75X_mcRun2_asymptotic_ppAt5TeV_v3-eca985b12211699dc9125db438586ff6/USER'
config.Data.inputDataset = '/Pythia8_bjet120_5020GeV_GEN-SIM/mnguyen-Pythia8_bjet120_5020GeV_RECO_75X_mcRun2_asymptotic_ppAt5TeV_v3-eca985b12211699dc9125db438586ff6/USER'

config.Data.inputDBS ='phys03'
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 30
#NJOBS = 10  # This is not a configuration parameter, but an auxiliary variable that we use in the next line.
#config.Data.totalUnits = config.Data.unitsPerJob * NJOBS
#config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = 'MC_bJet120_HiForest'

config.Site.storageSite = 'T2_US_Purdue'  # or T2_CH_CERN 
#config.Site.whitelist = ['T2_US_Purdue']


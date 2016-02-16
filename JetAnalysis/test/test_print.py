
import FWCore.ParameterSet.Config as cms

process = cms.Process("test")

process.source = cms.Source("PoolSource",
                            duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
                            fileNames = cms.untracked.vstring(
  #                              "root://cmsxrootd.fnal.gov//store/user/dgulhan/PYTHIA_QCD_TuneCUETP8M1_cfi_GEN_SIM_5020GeV/PYTHIA_QCD120_TuneCUETP8M1_cfi_RECODEBUGpp_757p1_timeslew_HcalRespCorrs_v4_00_mc/151215_074107/0000/step3_1.root",
	#                            "file:/home/peng43/scratch/Jet_FlavorID/CMSSW_7_5_8/src/HeavyIonsAnalysis/JetAnalysis/test/sample_input/PYTHIA_QCD120_TuneCUETP8M1_cfi_RECODEBUGpp_757p1_timeslew_HcalRespCorrs_v4_00_mc_pp/step3_1.root",
	"file:/home/peng43/scratch/CMSSW_7_5_8/src/step3_DIGI_L1_DIGI2RAW_HLT_RAW2DIGI_L1Reco_RECO.root",
	#"file:/home/peng43/scratch/Jet_FlavorID/CMSSW_7_5_8/src/HeavyIonsAnalysis/JetAnalysis/test/sample_input/pp_bjet_backup/step3_1.root",

                            ),
                             skipEvents = cms.untracked.uint32(50)


	)

process.maxEvents = cms.untracked.PSet(
  	  input = cms.untracked.int32(1))

process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi") 

process.printTree = cms.EDAnalyzer("ParticleListDrawer",
 	maxEventsToPrint = cms.untracked.int32(1),
  printVertex = cms.untracked.bool(False),
  printOnlyHardInteraction = cms.untracked.bool(False),
  src = cms.InputTag("genParticles")
	)

process.printTreeDrawer = cms.EDAnalyzer("ParticleTreeDrawer",
                                   src = cms.InputTag("genParticles"),
                                   printP4 = cms.untracked.bool(False),
                                   printPtEtaPhi = cms.untracked.bool(False),
                                   printVertex = cms.untracked.bool(False),
                                   printStatus = cms.untracked.bool(False),
                                   printIndex = cms.untracked.bool(False),
                                   status = cms.untracked.vint32( )
                                   )


process.p = cms.Path( process.printTree + process.printTreeDrawer )


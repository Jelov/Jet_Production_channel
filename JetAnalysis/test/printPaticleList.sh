#! /bin/bash
outputname="pp_bjet50_pythia6_evtgen_particleList_"
savepath="particleListOutput/pp_bjet50_evtgen/"

echo "partilceList and tree plot for "$savepath 

mkdir -p $savepath
for ievent in {1..50}
do

cat <<EOF > test_print.py

import FWCore.ParameterSet.Config as cms

process = cms.Process("test")

process.source = cms.Source("PoolSource",
                            duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
                            fileNames = cms.untracked.vstring(
  #                              "root://cmsxrootd.fnal.gov//store/user/dgulhan/PYTHIA_QCD_TuneCUETP8M1_cfi_GEN_SIM_5020GeV/PYTHIA_QCD120_TuneCUETP8M1_cfi_RECODEBUGpp_757p1_timeslew_HcalRespCorrs_v4_00_mc/151215_074107/0000/step3_1.root",
	#                            "file:/home/peng43/scratch/Jet_FlavorID/CMSSW_7_5_8/src/HeavyIonsAnalysis/JetAnalysis/test/sample_input/PYTHIA_QCD120_TuneCUETP8M1_cfi_RECODEBUGpp_757p1_timeslew_HcalRespCorrs_v4_00_mc_pp/step3_1.root",
#	"file:/home/peng43/scratch/CMSSW_7_5_8/src/step3_DIGI_L1_DIGI2RAW_HLT_RAW2DIGI_L1Reco_RECO.root",
	#"file:/home/peng43/scratch/Jet_FlavorID/CMSSW_7_5_8/src/HeavyIonsAnalysis/JetAnalysis/test/sample_input/pp_bjet_backup/step3_1.root",
#"root://cms-xrd-global.cern.ch//store/user/mnguyen//forestMC75X/bJetPythia6_pthat50_EvtGen_TuneZ2_5020GeV/forest_bjet50_Pythia6_evtgen_758p3_hfProdCode/160303_083103/0000/merged_HiForestAOD.root",
"file:/mnt/hadoop/store/user/chengchi/bJetPythia6_pthat50_EvtGen_TuneZ2_5020GeV/MC_bJet50_HLT_RECO_Pythia6/160224_214810/0000/ppReco_RAW2DIGI_L1Reco_RECO_Pythia6_1.root",
                            ),
                             skipEvents = cms.untracked.uint32($ievent)


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

EOF

cat <<EOF >$savepath$outputname$ievent.txt
1a1
pp_bJet200_hardbbbar ,event $ievent

EOF

cmsRun test_print.py >> $savepath$outputname$ievent.txt
done

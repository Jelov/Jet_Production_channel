#! /bin/bash

for ievent in {1..30}
do

cat <<EOF >> fileList_pythia6_pt50_evtgen.txt
root://xrootd.unl.edu//store/user/chengchi/bJetPythia6_pthat50_EvtGen_TuneZ2_5020GeV/MC_bJet50_HLT_RECO_Pythia6/160224_214810/0000/ppReco_RAW2DIGI_L1Reco_RECO_Pythia6_$ievent.root

EOF
done


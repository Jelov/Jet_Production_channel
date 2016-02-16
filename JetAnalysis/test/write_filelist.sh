#! /bin/bash

for ievent in {41..100}
do

cat <<EOF >> test_fileList_41.txt
root://xrootd.unl.edu//store/user/mnguyen/bJet/Pythia8_bjet30_5020GeV_GEN-SIM/Pythia8_bjet30_5020GeV_RECO_75X_mcRun2_asymptotic_ppAt5TeV_v3/151215_143130/0000/step3_$ievent.root

EOF
done


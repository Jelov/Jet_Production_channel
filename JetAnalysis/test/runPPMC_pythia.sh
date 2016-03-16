#! /bin/bash
mv MC_particle_status.txt MC_particle_status_old.txt
mv returnHFJetProduType.txt returnHFJetProduType_old.txt
#cmsRun runForestAOD_pp_MC_75X.py
nohup time cmsRun runForestAOD_pp_MC_75X_bjet50.py &>myout.txt &
tail -f myout.txt 
#vi MC_particle_status.txt


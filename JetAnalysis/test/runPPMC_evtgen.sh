#! /bin/bash
rm myout_evtgen.txt
mv MC_particle_status.txt MC_particle_status_old.txt
mv returnHFJetProduType.txt returnHFJetProduType_old.txt
mv checkDauChare.txt checkDauChare_old.txt
#cmsRun runForestAOD_pp_MC_75X.py
nohup time cmsRun runForestAOD_pp_MC_75X_bjet50_evtgen.py &>myout_evtgen.txt &
tail -f myout_evtgen.txt 
#vi MC_particle_status.txt


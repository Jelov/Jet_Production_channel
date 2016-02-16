#! /bin/bash
rm MC_particle_status.txt
#cmsRun runForestAOD_pp_MC_75X.py
nohup time cmsRun runForestAOD_pp_MC_75X.py &>myout.txt &
tail -f myout.txt 
#vi MC_particle_status.txt


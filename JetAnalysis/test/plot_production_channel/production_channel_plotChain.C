// Cheng-chieh Peng
//
// read output of runForest_pp_MC,
// plot variable with production_channel related.
// created 2016 Feb. 10
// Last modified - 2016 Feb. 21

/* ## current channel value definition - should check with HiInclusive.cc 
          // determine the jetproduction_channel
          int jetProduction_channel = 0; // not matched for flavor :-1  ; not found channel :0 
          if (Quark21_tag==0 && Quark23_tag==1 && DMotherGluon21_tag ==2){jetProduction_channel=1;} // gluon-gluon fusion
          if (Quark21_tag==0 && Quark23_tag==1 && DMotherQuark21_tag ==2){jetProduction_channel=2;} // quark-quark annihilation 
          if (Quark31_tag==0 && Quark33_tag==1 && DMotherGluon31_tag ==2){jetProduction_channel=3;} // gluon-gluon fusion
          if (Quark31_tag==0 && Quark33_tag==1 && DMotherQuark31_tag ==2){jetProduction_channel=4;} // quark-quark annihilation 

          if(Quark21_tag==1){jetProduction_channel=5;} // flavor excitation
          if(Quark31_tag==1){jetProduction_channel=6;} // flavor excitation

          // initial state radiation
          if (Quark21_tag==0 && Quark23_tag==0 && Quark41_tag >=1){jetProduction_channel=7;} // this might look like flavor excitation
          if (Quark21_tag==0 && Quark23_tag==0 && Quark41_tag ==0 && (Quark43_tag >=1 || DMotherGluon43_tag ==1)){jetProduction_channel=8;}
          if (Quark21_tag==0 && Quark23_tag==0 && Quark41_tag ==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon41n43_tag==1){jetProduction_channel=9;} // initial state shower type 2 (should very similar to 8)
          if (Quark21_tag==0 && Quark23_tag==0 && Quark41_tag ==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon41n43_tag==0 && MotherAllOthers41n43_tag==1){jetProduction_channel=10;} // initial state shower type 2 (should very similar to 8
          if (Quark21_tag==0 && Quark23_tag==0 && Quark41_tag ==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon41n43_tag==0 && MotherAllOthers41n43_tag==0 && MotherBeamParticleProton_tag>=1) {jetProduction_channel=11;} // directly from p

          // gluon splitting ( after hard process)          
          if (Quark21_tag==0 && Quark23_tag==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon23_tag==1){jetProduction_channel=12;} // gluon splitting type 1
          if (Quark21_tag==0 && Quark23_tag==0 && Quark31_tag==0 && Quark33_tag==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon33_tag==1){jetProduction_channel=13;} // gluon splitting type 2
          if (Quark21_tag==0 && Quark23_tag==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon23_tag==0 && MotherAllOthers23_tag ==1 ){jetProduction_channel=14;} // gluon splitting type 3, gluon not produced in hardprocess
          if (Quark21_tag==0 && Quark23_tag==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon23_tag==0 && MotherAllOthers33_tag ==1 ){jetProduction_channel=15;} // gluon splitting type 4, gluon not pruduced in hardprocess


*/

/*
	plot	1.dR (and dR for different JetConeReconstrcution)
				2.pt, dphi for each channel, seperatly and combined in one plot-normalized.
				3.dtheta
				
*/

#include <iostream>
#include "TTree.h"
#include "TH1.h"
#include "TFile.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TChain.h"

void production_channel_plotChain(){

	TChain *t1= new TChain("ak4PFJetAnalyzer/t");
//	t1->Add("/mnt/hadoop/store/user/chengchi/Pythia8_bjet30_5020GeV_GEN-SIM/MC_bJet30_HiForest/160215_053738/0000/HiForestAOD_pp_MC_bjets_after41_5*.root"); // old one
//	t1->Add("../HiForestAOD_pp_MC_bjets.root"); // ~16files
		t1->Add("/mnt/hadoop/store/user/chengchi/Pythia8_bjet30_5020GeV_GEN-SIM/MC_bJet30_HiForest/160221_200407/0000/HiForestAOD_pp_MC_bjets30_*.root");

//	TFile *f1 = new TFile ("../HiForestAOD_pp_MC_bjets.root");
//	TTree *t1 = (TTree*)f1->Get("ak4PFJetAnalyzer/t");
//	TH1F *h_qJet_dR_all = (TH1F*)f1->Get("ak4PFJetAnalyzer/h_qJet_dR_all");
//	TH1F *h_qJet_dR_flavormatched = (TH1F*)f1->Get("ak4PFJetAnalyzer/h_qJet_dR_flavormatched");


  TH1I *production_channel =  new TH1I("production_channel","jets production channel",16,0,16);
  TCanvas *Canvas_channel = new TCanvas("production_channel","production_channel"); 
  Canvas_channel->cd(1);
  t1->Draw("refjet_prodChannel>>production_channel","refjet_prodChannel>=0");
  Canvas_channel->SaveAs("plots/bjet30/production_channel.pdf");




	int ptBins=50;
  double ptLo=0;
	double ptHi=100;

	int dphiBins=ptBins;
  double dphiLo=0;
	double dphiHi=3.2;

	int etaBins=ptBins;
	double etaLo=-6;
	double etaHi=6;

  int n_channels=16;

	TH1D* bjet30_channel_pt[n_channels];  	
  TH1D* bjet30_channel_pt_eta2less[n_channels];
	TH1D* bjet30_channel_eta[n_channels];
	TH1D* bjet30_channel_dphi[n_channels];

	TCanvas *C_bjet30_channel_pt[n_channels];
  TCanvas *C_bjet30_channel_pt_eta2less[n_channels];	
	TCanvas *C_bjet30_channel_eta[n_channels];
	TCanvas *C_bjet30_channel_dphi[n_channels];	

	
	for (int i_channels=1; i_channels<n_channels;++i_channels){

		///////// bjet30_channel_pt
		bjet30_channel_pt[i_channels] = new TH1D( Form("bjet30_channel_pt_%i",i_channels) , Form("bjet30_channel_%i_pt",i_channels) ,ptBins,ptLo,ptHi);
		C_bjet30_channel_pt[i_channels] = new TCanvas( Form("C_bjet30_channel%i_pt",i_channels) , Form("C_bjet30_channel%i_pt",i_channels) );
  	t1->Draw( Form("jtpt>>bjet30_channel_pt_%i",i_channels) , Form("refjet_prodChannel==%i",i_channels) );
		bjet30_channel_pt[i_channels]->GetXaxis()->SetTitle("jet_pt");
		bjet30_channel_pt[i_channels]->Draw();
  	cout<<Form("jtpt>>bjet30_channel_pt[%i]",i_channels)<<endl;
		C_bjet30_channel_pt[i_channels]->SaveAs( Form("plots/bjet30/bjet30_channel%i_pt.pdf", i_channels));
		C_bjet30_channel_pt[i_channels]->Close();

    ///////// bjet30_channel_pt_eta2less	
    bjet30_channel_pt_eta2less[i_channels] = new TH1D( Form("bjet30_channel_pt_eta2less_%i",i_channels) , Form("bjet30_channel_%i_pt_eta2less",i_channels) ,ptBins,ptLo,ptHi);
    C_bjet30_channel_pt_eta2less[i_channels] = new TCanvas( Form("C_bjet30_channel%i_pt_eta2less",i_channels) , Form("C_bjet30_channel%i_pt_eta2less",i_channels) );
    t1->Draw( Form("jtpt>>bjet30_channel_pt_eta2less_%i",i_channels) , Form("refjet_prodChannel==%i && abs(jteta)<2",i_channels) );
    bjet30_channel_pt_eta2less[i_channels]->GetXaxis()->SetTitle("jet_pt");
    bjet30_channel_pt_eta2less[i_channels]->Draw();
    cout<<Form("jtpt>>bjet30_channel_pt_eta2less[%i]",i_channels)<<endl;
    C_bjet30_channel_pt_eta2less[i_channels]->SaveAs( Form("plots/bjet30/bjet30_channel%i_pt_eta2less.pdf", i_channels));
		C_bjet30_channel_pt_eta2less[i_channels]->Close();

    ///////// bjet30_channel_eta
    bjet30_channel_eta[i_channels] = new TH1D( Form("bjet30_channel_eta_%i",i_channels) , Form("bjet30_channel_%i_eta",i_channels) ,etaBins,etaLo,etaHi);
    C_bjet30_channel_eta[i_channels] = new TCanvas( Form("C_bjet30_channel%i_eta",i_channels) , Form("C_bjet30_channel%i_eta",i_channels) );
    t1->Draw( Form("jteta>>bjet30_channel_eta_%i",i_channels) , Form("refjet_prodChannel==%i",i_channels) );
    bjet30_channel_eta[i_channels]->GetXaxis()->SetTitle("jet_eta");
    bjet30_channel_eta[i_channels]->Draw();
    cout<<Form("jteta>>bjet30_channel_eta[%i]",i_channels)<<endl;
    C_bjet30_channel_eta[i_channels]->SaveAs( Form("plots/bjet30/bjet30_channel%i_eta.pdf", i_channels));
		C_bjet30_channel_eta[i_channels]->Close();

   ///////// bjet30_channel_dphi
    bjet30_channel_dphi[i_channels] = new TH1D( Form("bjet30_channel_dphi_%i",i_channels) , Form("bjet30_channel_%i_dphi",i_channels) ,dphiBins,dphiLo,dphiHi);
    C_bjet30_channel_dphi[i_channels] = new TCanvas( Form("C_bjet30_channel%i_dphi",i_channels) , Form("C_bjet30_channel%i_dphi",i_channels) );
    t1->Draw( Form("ref_qTwin_dphi>>bjet30_channel_dphi_%i",i_channels) , Form("refjet_prodChannel==%i",i_channels) );
    bjet30_channel_dphi[i_channels]->GetXaxis()->SetTitle("#Delta#phi");
    bjet30_channel_dphi[i_channels]->Draw();
    cout<<Form("ref_qTwin_dphi>>bjet30_channel_dphi[%i]",i_channels)<<endl;
    C_bjet30_channel_dphi[i_channels]->SaveAs( Form("plots/bjet30/bjet30_channel%i_dphi.pdf", i_channels));
		C_bjet30_channel_dphi[i_channels]->Close();

	}


	TCanvas * C_bjet30_channelAll_pt = new TCanvas("C_bjet30_channelAll_pt","C_bjet30_channelAll_pt");
	C_bjet30_channelAll_pt->Divide(4,4);
	for (int i_channels=1; i_channels<n_channels;++i_channels){
//    bjet30_channel_pt[i_channels] = new TH1D( Form("bjet30_channel_pt_%i",i_channels) , Form("bjet30_channel_%i_pt",i_channels) ,ptBins,ptLo,ptHi);
		C_bjet30_channelAll_pt->cd(i_channels);
//	t1->Draw( Form("jtpt>>bjet30_channel_pt_%i",i_channels) , Form("refjet_prodChannel==%i",i_channels) );
		bjet30_channel_pt[i_channels]->Draw();
	}
	C_bjet30_channelAll_pt->SaveAs("plots/bjet30/bjet30_channelAll_pt.pdf");


  TCanvas * C_bjet30_channelAll_pt_eta2less = new TCanvas("C_bjet30_channelAll_pt_eta2less","C_bjet30_channelAll_pt_eta2less");
  C_bjet30_channelAll_pt_eta2less->Divide(4,4);
  for (int i_channels=1; i_channels<n_channels;++i_channels){
    C_bjet30_channelAll_pt_eta2less->cd(i_channels);
    bjet30_channel_pt_eta2less[i_channels]->Draw();
  }
  C_bjet30_channelAll_pt_eta2less->SaveAs("plots/bjet30/bjet30_channelAll_pt_eta2less.pdf");


  TCanvas * C_bjet30_channelAll_eta = new TCanvas("C_bjet30_channelAll_eta","C_bjet30_channelAll_eta");
  C_bjet30_channelAll_eta->Divide(4,4);
  for (int i_channels=1; i_channels<n_channels;++i_channels){
    C_bjet30_channelAll_eta->cd(i_channels);
    bjet30_channel_eta[i_channels]->Draw();
  }
  C_bjet30_channelAll_eta->SaveAs("plots/bjet30/bjet30_channelAll_eta.pdf");


  TCanvas * C_bjet30_channelAll_dphi = new TCanvas("C_bjet30_channelAll_dphi","C_bjet30_channelAll_dphi");
  C_bjet30_channelAll_dphi->Divide(4,4);
  for (int i_channels=1; i_channels<n_channels;++i_channels){
    C_bjet30_channelAll_dphi->cd(i_channels);
    bjet30_channel_dphi[i_channels]->Draw();
  }
  C_bjet30_channelAll_dphi->SaveAs("plots/bjet30/bjet30_channelAll_dphi.pdf");



// why canvas->cd(i), not work in for loop ???

/*
	Canvas_pt->Divide(3,2);
  Canvas_pt->cd(1);
  t1->Draw("jtpt>>flavor_excitation_pt",flavor_excitation_cut);
  flavor_excitation_pt->GetXaxis()->SetTitle("jet_pt");
  flavor_excitation_pt->Draw();
*/

//  Canvas_dR->Divide(2,1);
//  Canvas_dR->cd(1);
/*

	TH1I *production_channel =  new TH1I("production_channel","jets production channel",12,0,16);

  TH1D *channel_0_pt   = new TH1D("channel_0_pt"  ,"channel_0_pt",ptBins,ptLo,ptHi);           // channel_0_pt->Sumw2();
  TH1D *channel_0_dphi = new TH1D("channel_0_dphi","channel_0_dphi",dphiBins,dphiLo,dphiHi);	 // channel_0_dphi->Sumw2();

	TH1D *channel_1_pt   = new TH1D("channel_1_pt"  ,"channel_1_pt",ptBins,ptLo,ptHi);			     // channel_1_pt->Sumw2();  
  TH1D *channel_1_dphi = new TH1D("channel_1_dphi","channel_1_dphi",dphiBins,dphiLo,dphiHi);   // channel_1_dphi->Sumw2();

  TH1D *channel_2_pt   = new TH1D("channel_2_pt"  ,"channel_2_pt",ptBins,ptLo,ptHi);           // channel_2_pt->Sumw2();
  TH1D *channel_2_dphi = new TH1D("channel_2_dphi","channel_2_dphi",dphiBins,dphiLo,dphiHi);   // channel_2_dphi->Sumw2();

  TH1D *channel_3_pt   = new TH1D("channel_3_pt"  ,"channel_3_pt",ptBins,ptLo,ptHi); 					 // channel_3_pt->Sumw2();
  TH1D *channel_3_dphi = new TH1D("channel_3_dphi","channel_3_dphi",dphiBins,dphiLo,dphiHi);	 // channel_3_dphi->Sumw2();

  TH1D *channel_4_pt   = new TH1D("channel_4_pt"  ,"channel_4_pt",ptBins,ptLo,ptHi);           // channel_4_pt->Sumw2();
  TH1D *channel_4_dphi = new TH1D("channel_4_dphi","channel_4_dphi",dphiBins,dphiLo,dphiHi);   // channel_4_dphi->Sumw2();

  TH1D *channel_5_pt   = new TH1D("channel_5_pt"  ,"channel_5_pt",ptBins,ptLo,ptHi);           // channel_5_pt->Sumw2();
  TH1D *channel_5_dphi = new TH1D("channel_5_dphi","channel_5_dphi",dphiBins,dphiLo,dphiHi);   // channel_5_dphi->Sumw2();

  TH1D *channel_6_pt   = new TH1D("channel_6_pt"  ,"channel_6_pt",ptBins,ptLo,ptHi);           // channel_6_pt->Sumw2();
  TH1D *channel_6_dphi = new TH1D("channel_6_dphi","channel_6_dphi",dphiBins,dphiLo,dphiHi);   // channel_6_dphi->Sumw2();

  TH1D *channel_7_pt   = new TH1D("channel_7_pt"  ,"channel_7_pt",ptBins,ptLo,ptHi);					 // channel_7_pt->Sumw2();
  TH1D *channel_7_dphi = new TH1D("channel_7_dphi","channel_7_dphi",dphiBins,dphiLo,dphiHi);	 // channel_7_dphi->Sumw2();

  TH1D *channel_8_pt   = new TH1D("channel_8_pt"  ,"channel_8_pt",ptBins,ptLo,ptHi);           // channel_8_pt->Sumw2();
  TH1D *channel_8_dphi = new TH1D("channel_8_dphi","channel_8_dphi",dphiBins,dphiLo,dphiHi);   // channel_8_dphi->Sumw2();

  TH1D *channel_9_pt   = new TH1D("channel_9_pt"  ,"channel_9_pt",ptBins,ptLo,ptHi);					 // channel_9_pt->Sumw2();
  TH1D *channel_9_dphi = new TH1D("channel_9_dphi","channel_9_dphi",dphiBins,dphiLo,dphiHi);	 // channel_9_dphi->Sumw2();

  TH1D *channel_10_pt   = new TH1D("channel_10_pt"  ,"channel_10_pt",ptBins,ptLo,ptHi);			   // channel_10_pt->Sumw2();
  TH1D *channel_10_dphi = new TH1D("channel_10_dphi","channel_10_dphi",dphiBins,dphiLo,dphiHi);//	channel_10_dphi->Sumw2();

  TH1D *channel_11_pt   = new TH1D("channel_11_pt"  ,"channel_11_pt",ptBins,ptLo,ptHi);           // channel_11_pt->Sumw2();
  TH1D *channel_11_dphi = new TH1D("channel_11_dphi","channel_11_dphi",dphiBins,dphiLo,dphiHi);   // channel_11_dphi->Sumw2();

  TH1D *channel_12_pt   = new TH1D("channel_12_pt"  ,"channel_12_pt",ptBins,ptLo,ptHi);           // channel_12_pt->Sumw2();  
  TH1D *channel_12_dphi = new TH1D("channel_12_dphi","channel_12_dphi",dphiBins,dphiLo,dphiHi);   // channel_12_dphi->Sumw2();

  TH1D *channel_13_pt   = new TH1D("channel_13_pt"  ,"channel_13_pt",ptBins,ptLo,ptHi);           // channel_13_pt->Sumw2();  
  TH1D *channel_13_dphi = new TH1D("channel_13_dphi","channel_13_dphi",dphiBins,dphiLo,dphiHi);   // channel_13_dphi->Sumw2();

// gather channels into meaningful 4 types.
	
	TH1D *flavor_excitation_pt = new TH1D("flavor_excitation_pt","flavor_excitation_pt",ptBins,ptLo,ptHi);
  TH1D *flavor_excitation_pt_eta2less = new TH1D("flavor_excitation_pt_eta2less","flavor_excitation_pt_eta2less",ptBins,ptLo,ptHi);
  TH1D *flavor_excitation_pt_eta2more = new TH1D("flavor_excitation_pt_eta2more","flavor_excitation_pt_eta2more",ptBins,ptLo,ptHi);
	TH1D *flavor_excitation_dphi = new TH1D("flavor_excitation_dphi","flavor_excitation_dphi",dphiBins,dphiLo,dphiHi);

  TH1D *pair_creation_pt = new TH1D("pair_creation_pt","pair_creation_pt",ptBins,ptLo,ptHi);
  TH1D *pair_creation_pt_eta2less = new TH1D("pair_creation_pt_eta2less","pair_creation_pt_eta2less",ptBins,ptLo,ptHi);
  TH1D *pair_creation_pt_eta2more = new TH1D("pair_creation_pt_eta2more","pair_creation_pt_eta2more",ptBins,ptLo,ptHi);
  TH1D *pair_creation_dphi = new TH1D("pair_creation_dphi","pair_creation_dphi",dphiBins,dphiLo,dphiHi);

  TH1D *gluon_splitting_pt = new TH1D("gluon_splitting_pt","gluon_splitting_pt",ptBins,ptLo,ptHi);
  TH1D *gluon_splitting_pt_eta2less = new TH1D("gluon_splitting_pt_eta2less","gluon_splitting_pt_eta2less",ptBins,ptLo,ptHi);
  TH1D *gluon_splitting_pt_eta2more = new TH1D("gluon_splitting_pt_eta2more","gluon_splitting_pt_eta2more",ptBins,ptLo,ptHi);
  TH1D *gluon_splitting_dphi = new TH1D("gluon_splitting_dphi","gluon_splitting_dphi",dphiBins,dphiLo,dphiHi); 

  TH1D *isr_pt = new TH1D("isr_pt","isr_pt",ptBins,ptLo,ptHi);
  TH1D *isr_pt_eta2less = new TH1D("isr_pt_eta2less","isr_pt_eta2less",ptBins,ptLo,ptHi);
  TH1D *isr_pt_eta2more = new TH1D("isr_pt_eta2more","isr_pt_eta2more",ptBins,ptLo,ptHi);
  TH1D *isr_dphi = new TH1D("isr_dphi","isr_dphi",dphiBins,dphiLo,dphiHi); 

//	flavor_excitation_dphi->Sumw2();
//	pair_creation_dphi->Sumw2();
//	gluon_splitting_dphi->Sumw2();
//	isr_dphi->Sumw2();

	TCut flavor_excitation_cut="(refjet_prodChannel==1 || refjet_prodChannel==2)";
	TCut pair_creation_cut="(refjet_prodChannel==3 || refjet_prodChannel==4 || refjet_prodChannel==5 || refjet_prodChannel==6)";
	TCut gluon_splitting_cut="(refjet_prodChannel==10 || refjet_prodChannel==11)";
	TCut isr_cut="(refjet_prodChannel==7 || refjet_prodChannel==8 || refjet_prodChannel==9)";
	TCut eta_2less_cut="(jteta<2 && jteta>-2)";
  TCut eta_2more_cut="(jteta>2 || jteta<-2)";
*/
// plot
/*
	TCanvas *Canvas_channel = new TCanvas("production_channel","production_channel"); 
	Canvas_channel->cd(1);
	t1->Draw("refjet_prodChannel>>production_channel","refjet_prodChannel>=0");
	Canvas_channel->SaveAs("plots/production_channel.pdf");
*/
/*
	TCanvas *Canvas_dR = new TCanvas("qJet_dR","qJet_dR");
	Canvas_dR->Divide(2,1);
	Canvas_dR->cd(1);
	h_qJet_dR_all->Draw("");
	Canvas_dR->cd(2);
	h_qJet_dR_flavormatched->Draw("");
	Canvas_dR->SaveAs("plots/qJet_dR.pdf");
*/
/*
	TCanvas *Canvas_qJetdR = new TCanvas("qJetdR","qJetdR");
	Canvas_qJetdR->Divide(3,2);
	Canvas_qJetdR->cd(1);
	t1->Draw("ref_qfJet_dR",flavor_excitation_cut);
	Canvas_qJetdR->cd(2);
  t1->Draw("ref_qfJet_dR",pair_creation_cut);
  Canvas_qJetdR->cd(3);
  t1->Draw("ref_qfJet_dR",gluon_splitting_cut);
  Canvas_qJetdR->cd(4);
  t1->Draw("ref_qfJet_dR",isr_cut);
  Canvas_qJetdR->cd(5);
  t1->Draw("ref_qfJet_dR","refjet_prodChannel ==0 ");
	Canvas_qJetdR->SaveAs("plots/qJetdR.pdf");

  TCanvas *Canvas_qJetdpt = new TCanvas("qJetdpt","qJetdpt");
  Canvas_qJetdpt->Divide(3,2);
  Canvas_qJetdpt->cd(1);
  t1->Draw("ref_qfJet_dpt",flavor_excitation_cut);
  Canvas_qJetdpt->cd(2);
  t1->Draw("ref_qfJet_dpt",pair_creation_cut);
  Canvas_qJetdpt->cd(3);
  t1->Draw("ref_qfJet_dpt",gluon_splitting_cut);
  Canvas_qJetdpt->cd(4);
  t1->Draw("ref_qfJet_dpt",isr_cut);
  Canvas_qJetdpt->cd(5);
  t1->Draw("ref_qfJet_dpt","refjet_prodChannel ==0 ");
  Canvas_qJetdpt->SaveAs("plots/qJetdpt.pdf");


  TCanvas *Canvas_Jeteta = new TCanvas("Jeteta","Jeteta");
  Canvas_Jeteta->Divide(3,2);
  Canvas_Jeteta->cd(1);
  t1->Draw("jteta",flavor_excitation_cut);
  Canvas_Jeteta->cd(2);
  t1->Draw("jteta",pair_creation_cut);
  Canvas_Jeteta->cd(3);
  t1->Draw("jteta",gluon_splitting_cut);
  Canvas_Jeteta->cd(4);
  t1->Draw("jteta",isr_cut);
  Canvas_Jeteta->cd(5);
  t1->Draw("jteta","refjet_prodChannel ==0 ");
  Canvas_Jeteta->SaveAs("plots/Jeteta.pdf");
*/

/* ### working here
	TCanvas *Canvas_pt_channels = new TCanvas("jet_pt_channels","jet_pt_channels");
	Canvas_pt_channels->Divide(3,2);
	Canvas_pt_channels->cd(1);
  t1->Draw("jtpt>>channel_0_pt","refjet_prodChannel==0");
  Canvas_pt_channels->cd(2);
  t1->Draw("jtpt>>channel_1_pt","refjet_prodChannel==1");
  Canvas_pt_channels->cd(3);
  t1->Draw("jtpt>>channel_3_pt","refjet_prodChannel==3");
  Canvas_pt_channels->cd(4);
  t1->Draw("jtpt>>channel_7_pt","refjet_prodChannel==7");
	Canvas_pt_channels->cd(5);
  t1->Draw("jtpt>>channel_9_pt","refjet_prodChannel==9");
  Canvas_pt_channels->cd(6);
  t1->Draw("jtpt>>channel_10_pt","refjet_prodChannel==10");

	Canvas_pt1->SaveAs("plots/Jetpt1.pdf");
*/
/*
	TCanvas *Canvas_pt = new TCanvas("jet_pt","jet_pt");
  Canvas_pt->Divide(3,2);
  Canvas_pt->cd(1);
  t1->Draw("jtpt>>flavor_excitation_pt",flavor_excitation_cut);
  flavor_excitation_pt->GetXaxis()->SetTitle("jet_pt");
  flavor_excitation_pt->Draw();
  Canvas_pt->cd(2);
  t1->Draw("jtpt>>pair_creation_pt",pair_creation_cut);
  pair_creation_pt->GetXaxis()->SetTitle("jet_pt");
  pair_creation_pt->Draw();
  Canvas_pt->cd(3);
  t1->Draw("jtpt>>gluon_splitting_pt",gluon_splitting_cut);
  gluon_splitting_pt->GetXaxis()->SetTitle("jet_pt");
  gluon_splitting_pt->Draw();
  Canvas_pt->cd(4);
  t1->Draw("jtpt>>isr_pt",isr_cut);
  isr_pt->GetXaxis()->SetTitle("jet_pt");
  isr_pt->Draw();
  Canvas_pt->cd(5);
  t1->Draw("jtpt>>channel_0_pt","refjet_prodChannel==0");
  channel_0_pt->GetXaxis()->SetTitle("jet_pt");
  channel_0_pt->Draw();

  Canvas_pt->SaveAs("plots/Jetpt.pdf");

  TCanvas *Canvas_pt1 = new TCanvas("jet_pt_eta2less","jet_pt_eta2less");
  Canvas_pt1->Divide(3,2);
  Canvas_pt1->cd(1);
  t1->Draw("jtpt>>flavor_excitation_pt_eta2less",flavor_excitation_cut&&eta_2less_cut);
  flavor_excitation_pt_eta2less->GetXaxis()->SetTitle("jet_pt");
  flavor_excitation_pt_eta2less->Draw();
  Canvas_pt1->cd(2);
  t1->Draw("jtpt>>pair_creation_pt_eta2less",pair_creation_cut&&eta_2less_cut);
  pair_creation_pt_eta2less->GetXaxis()->SetTitle("jet_pt");
  pair_creation_pt_eta2less->Draw();
  Canvas_pt1->cd(3);
  t1->Draw("jtpt>>gluon_splitting_pt_eta2less",gluon_splitting_cut&&eta_2less_cut);
  gluon_splitting_pt_eta2less->GetXaxis()->SetTitle("jet_pt");
  gluon_splitting_pt_eta2less->Draw();
  Canvas_pt1->cd(4);
  t1->Draw("jtpt>>isr_pt_eta2less",isr_cut&&eta_2less_cut);
  isr_pt_eta2less->GetXaxis()->SetTitle("jet_pt");
  isr_pt_eta2less->Draw();
  Canvas_pt1->cd(5);
  t1->Draw("jtpt>>channel_0_pt","refjet_prodChannel==0" && eta_2less_cut);
  channel_0_pt->GetXaxis()->SetTitle("jet_pt");
  channel_0_pt->Draw();

  Canvas_pt1->SaveAs("plots/Jetpt_eta_2less.pdf");

  TCanvas *Canvas_pt2 = new TCanvas("jet_pt_eta2more","jet_pt_eta2more");
  Canvas_pt2->Divide(3,2);
  Canvas_pt2->cd(1);
  t1->Draw("jtpt>>flavor_excitation_pt_eta2more",flavor_excitation_cut&&eta_2more_cut);
  flavor_excitation_pt_eta2more->GetXaxis()->SetTitle("jet_pt");
  flavor_excitation_pt_eta2more->Draw();
  Canvas_pt2->cd(2);
  t1->Draw("jtpt>>pair_creation_pt_eta2more",pair_creation_cut&&eta_2more_cut);
  pair_creation_pt_eta2more->GetXaxis()->SetTitle("jet_pt");
  pair_creation_pt_eta2more->Draw();
  Canvas_pt2->cd(3);
  t1->Draw("jtpt>>gluon_splitting_pt_eta2more",gluon_splitting_cut&&eta_2more_cut);
  gluon_splitting_pt_eta2more->GetXaxis()->SetTitle("jet_pt");
  gluon_splitting_pt_eta2more->Draw();
  Canvas_pt2->cd(4);
  t1->Draw("jtpt>>isr_pt_eta2more",isr_cut && eta_2more_cut);
  isr_pt_eta2more->GetXaxis()->SetTitle("jet_pt");
  isr_pt_eta2more->Draw();
  Canvas_pt2->cd(5);
  t1->Draw("jtpt>>channel_0_pt","refjet_prodChannel==0" && eta_2more_cut);
  channel_0_pt->GetXaxis()->SetTitle("jet_pt");
  channel_0_pt->Draw();

  Canvas_pt2->SaveAs("plots/Jetpt_eta2more.pdf");



	TCanvas *Canvas_dphi = new TCanvas("qqbar_dphi","qqbar_dphi");
  Canvas_dphi->Divide(3,2);
  Canvas_dphi->cd(1);
  t1->Draw("ref_qTwin_dphi>>flavor_excitation_dphi",flavor_excitation_cut);
	flavor_excitation_dphi->GetXaxis()->SetTitle("#Delta#phi");
	flavor_excitation_dphi->Draw();
  Canvas_dphi->cd(2);
  t1->Draw("ref_qTwin_dphi>>pair_creation_dphi",pair_creation_cut);
  pair_creation_dphi->GetXaxis()->SetTitle("#Delta#phi");
  pair_creation_dphi->Draw();
  Canvas_dphi->cd(3);
  t1->Draw("ref_qTwin_dphi>>gluon_splitting_dphi",gluon_splitting_cut);
  gluon_splitting_dphi->GetXaxis()->SetTitle("#Delta#phi");
  gluon_splitting_dphi->Draw();
  Canvas_dphi->cd(4);
  t1->Draw("ref_qTwin_dphi>>isr_dphi",isr_cut);
  isr_dphi->GetXaxis()->SetTitle("#Delta#phi");
  isr_dphi->Draw();
  Canvas_dphi->cd(5);
  t1->Draw("ref_qTwin_dphi>>channel_0_dphi","refjet_prodChannel==0");
	channel_0_dphi->GetXaxis()->SetTitle("#Delta#phi");
	channel_0_dphi->Draw();

  Canvas_dphi->SaveAs("plots/qqbar_dphi.pdf");
*/


		
/*
	TCanvas *Canvas_dphi1 = new TCanvas("qqbar_dphi1","qqbar_dphi1");
  Canvas_dphi1->Divide(3,2);
  Canvas_dphi1->cd(1);
  t1->Draw("ref_qTwin_dphi>>channel_0_dphi","refjet_prodChannel==0");
  Canvas_dphi1->cd(2);
  t1->Draw("ref_qTwin_dphi>>channel_1_dphi","refjet_prodChannel==1");
  Canvas_dphi1->cd(3);
  t1->Draw("ref_qTwin_dphi>>channel_3_dphi","refjet_prodChannel==3");
  Canvas_dphi1->cd(4);
  t1->Draw("ref_qTwin_dphi>>channel_7_dphi","refjet_prodChannel==7");
  Canvas_dphi1->cd(5);
  t1->Draw("ref_qTwin_dphi>>channel_9_dphi","refjet_prodChannel==9");
  Canvas_dphi1->cd(6);
  t1->Draw("ref_qTwin_dphi>>channel_10_dphi","refjet_prodChannel==10");

  Canvas_dphi1->SaveAs("plots/qqbar_dphi1.pdf");
*/




/*
	Canvas1->Divide(2,2);
	Canvas1->cd(1);
	t1->Draw("jtpt>>flavor_excitation_pt","refjet_prodChannel==1 || refjet_prodChannel==2");
	Canvas1->cd(2);
	t1->Draw("ref_qTwin_dphi>>flavor_excitation_dphi","refjet_prodChannel==1 || refjet_prodChannel==2");
	Canvas1->cd(3);
	t1->Draw("refjet_prodChannel>>production_channel");
*/

}

// Cheng-chieh Peng
//
// read output of runForest_pp_MC,
// plot variable with production_channel related.
// created 2015 Feb. 10
// modified -

/* ## current channel value definition - should check with HiInclusive.cc   ####   Waning changed, not updated here
          int jetProduction_channel = 0; // not matched for flavor :-1  ; not found channel :0 
          if(Quark21_tag==1){jetProduction_channel=1;} // flavor excitation
          if(Quark31_tag==1){jetProduction_channel=2;} // flavor excitation

          if (Quark21_tag==0 && Quark23_tag==1 && DMotherGluon21_tag ==2){jetProduction_channel=3;} // gluon-gluon fusion
          if (Quark21_tag==0 && Quark23_tag==1 && DMotherQuark21_tag ==2){jetProduction_channel=4;} // quark-quark annihilation 
          if (Quark31_tag==0 && Quark33_tag==1 && DMotherGluon31_tag ==2){jetProduction_channel=5;} // gluon-gluon fusion
          if (Quark31_tag==0 && Quark33_tag==1 && DMotherQuark31_tag ==2){jetProduction_channel=6;} // quark-quark annihilation 

          if (Quark21_tag==0 && Quark23_tag==0 && DMotherGluon41_tag ==1){jetProduction_channel=7;} // initial state shower or flavor excitation
          if (Quark21_tag==0 && Quark23_tag==0 && DMotherGluon43_tag ==1){jetProduction_channel=8;} // pure initial state shower
          if (Quark21_tag==0 && Quark23_tag==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon41n43_tag==1){jetProduction_channel=9;} // initial state shower type 2 (should very similar to 8)

          if (Quark21_tag==0 && Quark23_tag==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon23_tag==1){jetProduction_channel=10;} // gluon splitting
          if (Quark21_tag==0 && Quark23_tag==0 && Quark31_tag==0 && Quark33_tag==0 && DMotherGluon41_tag ==0 && DMotherGluon43_tag==0 && MotherGluon33_tag==1){jetProduction_channel=11;} // gluon splitting type 2

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

void production_channel_plot(){

	TFile *f1 = new TFile ("../HiForestAOD_pp_MC_bjets.root");
	TTree *t1 = (TTree*)f1->Get("ak4PFJetAnalyzer/t");
	TH1F *h_qJet_dR_all = (TH1F*)f1->Get("ak4PFJetAnalyzer/h_qJet_dR_all");
	TH1F *h_qJet_dR_flavormatched = (TH1F*)f1->Get("ak4PFJetAnalyzer/h_qJet_dR_flavormatched");

	int ptBins=20;
  double ptLo=0;
	double ptHi=100;

	int dphiBins=ptBins;
  double dphiLo=0;
	double dphiHi=3.2;

	TH1I *production_channel =  new TH1I("production_channel","jets production channel",12,0,12);

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

  TH1D *channel_7_pt   = new TH1D("channel_7_pt"  ,"channel_7_pt",ptBins,ptLo,ptHi);					 // channel_7_pt->Sumw2();
  TH1D *channel_7_dphi = new TH1D("channel_7_dphi","channel_7_dphi",dphiBins,dphiLo,dphiHi);	 // channel_7_dphi->Sumw2();

  TH1D *channel_9_pt   = new TH1D("channel_9_pt"  ,"channel_9_pt",ptBins,ptLo,ptHi);					 // channel_9_pt->Sumw2();
  TH1D *channel_9_dphi = new TH1D("channel_9_dphi","channel_9_dphi",dphiBins,dphiLo,dphiHi);	 // channel_9_dphi->Sumw2();

  TH1D *channel_10_pt   = new TH1D("channel_10_pt"  ,"channel_10_pt",ptBins,ptLo,ptHi);			   // channel_10_pt->Sumw2();
  TH1D *channel_10_dphi = new TH1D("channel_10_dphi","channel_10_dphi",dphiBins,dphiLo,dphiHi);//	channel_10_dphi->Sumw2();


// gather channels into meaningful 4 types.
	
	TH1D *flavor_excitation_pt = new TH1D("flavor_excitation_pt","flavor_excitation_pt",ptBins,ptLo,ptHi);
	TH1D *flavor_excitation_dphi = new TH1D("flavor_excitation_dphi","flavor_excitation_dphi",dphiBins,dphiLo,dphiHi);

  TH1D *pair_creation_pt = new TH1D("pair_creation_pt","pair_creation_pt",ptBins,ptLo,ptHi);
  TH1D *pair_creation_dphi = new TH1D("pair_creation_dphi","pair_creation_dphi",dphiBins,dphiLo,dphiHi);

  TH1D *gluon_splitting_pt = new TH1D("gluon_splitting_pt","gluon_splitting_pt",ptBins,ptLo,ptHi);
  TH1D *gluon_splitting_dphi = new TH1D("gluon_splitting_dphi","gluon_splitting_dphi",dphiBins,dphiLo,dphiHi); 

  TH1D *isr_pt = new TH1D("isr_pt","isr_pt",ptBins,ptLo,ptHi);
  TH1D *isr_dphi = new TH1D("isr_dphi","isr_dphi",dphiBins,dphiLo,dphiHi); 

//	flavor_excitation_dphi->Sumw2();
//	pair_creation_dphi->Sumw2();
//	gluon_splitting_dphi->Sumw2();
//	isr_dphi->Sumw2();

	TCut flavor_excitation_cut="refjet_prodChannel==1 || refjet_prodChannel==2";
	TCut pair_creation_cut="refjet_prodChannel==3 || refjet_prodChannel==4 || refjet_prodChannel==5 || refjet_prodChannel==6";
	TCut gluon_splitting_cut="refjet_prodChannel==10 || refjet_prodChannel==11";
	TCut isr_cut="refjet_prodChannel==7 || refjet_prodChannel==8 || refjet_prodChannel==9";


// plot

	TCanvas *Canvas_channel = new TCanvas("production_channel","production_channel"); 
	Canvas_channel->cd(1);
	t1->Draw("refjet_prodChannel>>production_channel","refjet_prodChannel>=0");
	Canvas_channel->SaveAs("production_channel.pdf");

	TCanvas *Canvas_dR = new TCanvas("qJet_dR","qJet_dR");
	Canvas_dR->Divide(2,1);
	Canvas_dR->cd(1);
	h_qJet_dR_all->Draw("");
	Canvas_dR->cd(2);
	h_qJet_dR_flavormatched->Draw("");
	Canvas_dR->SaveAs("qJet_dR.pdf");
/*
	TCanvas *Canvas_pt1 = new TCanvas("jet_pt1","jet_pt1");
	Canvas_pt1->Divide(3,2);
	Canvas_pt1->cd(1);
  t1->Draw("jtpt>>channel_0_pt","refjet_prodChannel==0");
  Canvas_pt1->cd(2);
  t1->Draw("jtpt>>channel_1_pt","refjet_prodChannel==1");
  Canvas_pt1->cd(3);
  t1->Draw("jtpt>>channel_3_pt","refjet_prodChannel==3");
  Canvas_pt1->cd(4);
  t1->Draw("jtpt>>channel_7_pt","refjet_prodChannel==7");
	Canvas_pt1->cd(5);
  t1->Draw("jtpt>>channel_9_pt","refjet_prodChannel==9");
  Canvas_pt1->cd(6);
  t1->Draw("jtpt>>channel_10_pt","refjet_prodChannel==10");

	Canvas_pt1->SaveAs("Jetpt1.pdf");
*/
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


  Canvas_pt->SaveAs("Jetpt.pdf");


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

  Canvas_dphi->SaveAs("qqbar_dphi.pdf");



		
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

  Canvas_dphi1->SaveAs("qqbar_dphi1.pdf");
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

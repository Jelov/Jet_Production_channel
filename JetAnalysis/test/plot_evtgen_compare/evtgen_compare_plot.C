// Cheng-Chieh Peng
// read output of runForest_pp_MC,
// plot evtgen_Bmeson decay products.
// Created 2016 Mar. 10
// Last Modified

#include <iostream>
#include "TTree.h"
#include "TH1.h"
#include "TFile.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TStyle.h"
#include <TAttMarker.h>
#include <TLegend.h>

/*
B0 decays:
            else if (d_DM_tag==1 && d_TauP_tag==1 && d_nuTau_tag==1){       B0_decay1=1;}
            else if (d_DeM_tag==1 && d_TauP_tag==1 && d_nuTau_tag==1){      B0_decay1=2;}
            else if (d_DeM_tag==1 && d_PiP_tag==1 && d_Pi0_tag==1){         B0_decay1=3;}
            else if (d_DeM_tag==1 && d_De0_tag==1 && d_KP_tag==1){          B0_decay1=4;}
            else if (d_DeM_tag==1 && d_a1P_tag==1 ){                        B0_decay1=5;}
            else if (d_DeSP_tag==1 && d_DeM_tag==1){                        B0_decay1=6;}
            else if (d_DeM_tag==1 && d_PiP_tag==2 && d_PiM_tag==1 && d_Pi0_tag==1 ){B0_decay1=7;}
	
*/

void evtgen_compare_plot(){

//	TChain *t_evtgen = new TChain("ak4PFJetAnalyzer/t");
	
//	TChain *t_pythia = new TChain("ak4PFJetAnalyzer/t");	
	TFile *f_pythia = new TFile("./root/origin1/HiForestAOD_pp_MC_bjets50.root"); 
	TH1I *h_B0_decay_pythia = (TH1I*)f_pythia->Get("ak4PFJetAnalyzer/h_B0_decay");
  TH1I *h_B0bar_decay_pythia = (TH1I*)f_pythia->Get("ak4PFJetAnalyzer/h_B0bar_decay");
  TH1I *h_Bp_decay_pythia = (TH1I*)f_pythia->Get("ak4PFJetAnalyzer/h_Bp_decay");
  TH1I *h_Bs_decay_pythia = (TH1I*)f_pythia->Get("ak4PFJetAnalyzer/h_Bs_decay");

	TTree *t_pythia_gen= (TTree*)f_pythia->Get("HiGenParticleAna/hi");

  TFile *f_evtgen = new TFile("./root/evtgen/HiForestAOD_pp_MC_bjets50_evtgen.root");
//	TFile *f_evtgen = new TFile("../HiForestAOD_pp_MC_bjets50_evtgen_01.root");
  TH1I *h_B0_decay_evtgen = (TH1I*)f_evtgen->Get("ak4PFJetAnalyzer/h_B0_decay");
  TH1I *h_B0bar_decay_evtgen = (TH1I*)f_evtgen->Get("ak4PFJetAnalyzer/h_B0bar_decay");
  TH1I *h_Bp_decay_evtgen = (TH1I*)f_evtgen->Get("ak4PFJetAnalyzer/h_Bp_decay");
  TH1I *h_Bs_decay_evtgen = (TH1I*)f_evtgen->Get("ak4PFJetAnalyzer/h_Bs_decay");

  TTree *t_evtgen_gen= (TTree*)f_evtgen->Get("HiGenParticleAna/hi");


	TH1I *h_pythia_bmix= new TH1I("h_pythia_bmix","h_pythia_bmix",30,510,540);	
  TH1I *h_evtgen_bmix= new TH1I("h_evtgen_bmix","h_evtgen_bmix",30,510,540);
  TH1I *h_pdg_bmix= new TH1I("h_pdg_bmix","h_pdg_bmix",30,510,540);

  double scale=100;
	h_pdg_bmix->SetBinContent(2,0.405*scale);
  h_pdg_bmix->SetBinContent(12,0.405*scale);
  h_pdg_bmix->SetBinContent(22,0.105*scale);
	h_pdg_bmix->SetMarkerStyle(2);
	scale= 91.5;
	
//  TCanvas *C_Bmix = new TCanvas("C_Bmix","C_Bmix");
	t_pythia_gen->Draw("abs(pdg)>>h_pythia_bmix","abs(pdg)== 511 || abs(pdg) == 521 || abs(pdg) == 531");
	double b_pythia_entries = (double)h_pythia_bmix->GetEntries();
	h_pythia_bmix->Scale(scale/b_pythia_entries);
	h_pythia_bmix->SetLineColor(4);
	h_pythia_bmix->SetMarkerStyle(2);
  h_pythia_bmix->SetMarkerColor(4);
  t_evtgen_gen->Draw("abs(pdg)>>h_evtgen_bmix","abs(pdg)== 511 || abs(pdg) == 521 || abs(pdg) == 531");
	double b_evtgen_entries = (double)h_evtgen_bmix->GetEntries();
  h_evtgen_bmix->Scale(scale/b_evtgen_entries);
	h_evtgen_bmix->SetLineColor(2);
	h_evtgen_bmix->SetMarkerStyle(5);
  h_evtgen_bmix->SetMarkerColor(2);
// normalize
	h_pdg_bmix->SetLineColor(1);

  TCanvas *C_Bmix = new TCanvas("C_Bmix","C_Bmix");
	gStyle->SetOptStat(0);
	h_pdg_bmix->SetTitle("B^{0}/B^{+}/B_{S} Fraction");
	h_pdg_bmix->GetXaxis()->SetTitle("pdgId");
  h_pdg_bmix->Draw("PSAME");
  h_pythia_bmix->Draw("SAME");
  h_evtgen_bmix->Draw("PSAME");

	TLegend *leg = new TLegend(0.7,0.8,0.9,0.9);
//	leg->SetHeader("header");
	leg->AddEntry(h_pdg_bmix,"pdg","p");
	leg->AddEntry(h_pythia_bmix,"pythia6","l");
  leg->AddEntry(h_evtgen_bmix,"evtgen","p");
	leg->Draw();		

	C_Bmix->SaveAs("Bmix.pdf");

	int nbins=10;
	int binLo=-1;
	int binHi=9;

	TH1I *h_B0_decay_pdg = new TH1I("h_B0_decay_pdg","h_B0_decay_pdg",nbins,binLo,binHi); //h_B0_decay_pdg->Sumw2();
  TH1I *h_B0bar_decay_pdg = new TH1I("h_B0bar_decay_pdg","h_B0bar_decay_pdg",nbins,binLo,binHi);
  TH1I *h_Bp_decay_pdg = new TH1I("h_Bp_decay_pdg","h_Bp_decay_pdg",nbins,binLo,binHi);
  TH1I *h_Bs_decay_pdg = new TH1I("h_Bs_decay_pdg","h_Bs_decay_pdg",nbins,binLo,binHi);

	h_B0_decay_pdg->SetMarkerStyle(5);
	h_B0_decay_pdg->SetMarkerColor(1);
	h_B0_decay_pdg->SetMarkerSize(2);

  h_Bp_decay_pdg->SetMarkerStyle(5);
  h_Bp_decay_pdg->SetMarkerColor(1);
  h_Bp_decay_pdg->SetMarkerSize(2);

  h_Bs_decay_pdg->SetMarkerStyle(5);
  h_Bs_decay_pdg->SetMarkerColor(1);
  h_Bs_decay_pdg->SetMarkerSize(2);


	TH1I *h_B0_decay_table = new TH1I("h_B0_decay_table","h_B0_decay_table",nbins,binLo,binHi); //h_B0_decay_pdg->Sumw2();
  h_B0_decay_table->SetMarkerStyle(4);
  h_B0_decay_table->SetMarkerColor(2);
  h_B0_decay_table->SetMarkerSize(1);


	
	double scale_factor=10000;
	double B0_decaysBR[8];
	B0_decaysBR[0]=1.0;
	B0_decaysBR[1]=0.0103;
	B0_decaysBR[2]=0.0184;
	B0_decaysBR[3]=0.0082;
	B0_decaysBR[4]=0.0106;
	B0_decaysBR[5]=0.013;
	B0_decaysBR[6]=0.0177;
	B0_decaysBR[7]=0.0176;
	for(size_t i=1 ; i<sizeof(B0_decaysBR)/sizeof(B0_decaysBR[0]); i++){
	B0_decaysBR[0]=B0_decaysBR[0]-B0_decaysBR[i];
	h_B0_decay_pdg->SetBinContent(i+2,B0_decaysBR[i]*scale_factor);
	}

  double B0_decaysBR_table[8];
  B0_decaysBR_table[0]=1.0;
  B0_decaysBR_table[1]=0.007;
  B0_decaysBR_table[2]=0.016;
  B0_decaysBR_table[3]=0.0008;
  B0_decaysBR_table[4]=0.0100;
  B0_decaysBR_table[5]=0.0120;
  B0_decaysBR_table[6]=0.0240;
  B0_decaysBR_table[7]=0.0;
  for(size_t i=1 ; i<sizeof(B0_decaysBR_table)/sizeof(B0_decaysBR_table[0]); i++){
  B0_decaysBR_table[0]=B0_decaysBR_table[0]-B0_decaysBR_table[i];
  h_B0_decay_table->SetBinContent(i+2,B0_decaysBR_table[i]*scale_factor);
  }

  double Bp_decaysBR[8];
  Bp_decaysBR[0]=1.0;
  Bp_decaysBR[1]=0.0134;
  Bp_decaysBR[2]=0.019;
  Bp_decaysBR[3]=0.0171;
  Bp_decaysBR[4]=0.0112;
  Bp_decaysBR[5]=0.0103;
  Bp_decaysBR[6]=0.015;
  Bp_decaysBR[7]=0.018;
  for(size_t i=1 ; i<sizeof(Bp_decaysBR)/sizeof(Bp_decaysBR[0]); i++){
  Bp_decaysBR[0]=Bp_decaysBR[0]-Bp_decaysBR[i];
  h_Bp_decay_pdg->SetBinContent(i+2,Bp_decaysBR[i]*scale_factor);
  }

  double Bs_decaysBR[7];
  Bs_decaysBR[0]=1.0;
  Bs_decaysBR[1]=0.0186;
  Bs_decaysBR[2]=0.0044;
  Bs_decaysBR[3]=0.007;
  Bs_decaysBR[4]=0.0097;
  Bs_decaysBR[5]=0.00304;
  Bs_decaysBR[6]=0.0063;
  for(size_t i=1 ; i<sizeof(Bs_decaysBR)/sizeof(Bs_decaysBR[0]); i++){
  Bs_decaysBR[0]=Bs_decaysBR[0]-Bs_decaysBR[i];
  h_Bs_decay_pdg->SetBinContent(i+2,Bs_decaysBR[i]*scale_factor);
  }



//	h_B0_decay_pdg->SetBinContent(2,B0_decaysBR[i]*scale_factor)

//	h_B0_decay_pdg->Draw();



	TCanvas *C_B0_decay = new TCanvas("C_B0_decay","C_B0_decay");
//	C_B0_decay->Divide(2,1);
	C_B0_decay->cd(1);
  double B0_pythia_entries= (double)h_B0_decay_pythia->GetEntries();
	double B0_evtgen_entries= (double)h_B0_decay_evtgen->GetEntries();

	double B0_pythia_n1= (double)h_B0_decay_pythia->GetBinContent(3);
  double B0_pythia_n2= (double)h_B0_decay_pythia->GetBinContent(4);
  double B0_pythia_n3= (double)h_B0_decay_pythia->GetBinContent(5);
  double B0_pythia_n4= (double)h_B0_decay_pythia->GetBinContent(6);
  double B0_pythia_n5= (double)h_B0_decay_pythia->GetBinContent(7);
  double B0_pythia_n6= (double)h_B0_decay_pythia->GetBinContent(8);
  double B0_pythia_n7= (double)h_B0_decay_pythia->GetBinContent(9);

	cout<<"B0_pythia_entries = "<<B0_pythia_entries<<" , B0_pythia_n1 = "<<B0_pythia_n1<<" , ratio= "<<B0_pythia_n1/B0_pythia_entries<<endl;
	cout<<" ratio1 = "<<B0_pythia_n1/B0_pythia_entries<<endl;
  cout<<" ratio2 = "<<B0_pythia_n2/B0_pythia_entries<<endl;
  cout<<" ratio3 = "<<B0_pythia_n3/B0_pythia_entries<<endl;
  cout<<" ratio4 = "<<B0_pythia_n4/B0_pythia_entries<<endl;
  cout<<" ratio5 = "<<B0_pythia_n5/B0_pythia_entries<<endl;
  cout<<" ratio6 = "<<B0_pythia_n6/B0_pythia_entries<<endl;
  cout<<" ratio7 = "<<B0_pythia_n7/B0_pythia_entries<<endl;

  double B0_evtgen_n1= (double)h_B0_decay_evtgen->GetBinContent(3);
  double B0_evtgen_n2= (double)h_B0_decay_evtgen->GetBinContent(4);
  double B0_evtgen_n3= (double)h_B0_decay_evtgen->GetBinContent(5);
  double B0_evtgen_n4= (double)h_B0_decay_evtgen->GetBinContent(6);
  double B0_evtgen_n5= (double)h_B0_decay_evtgen->GetBinContent(7);
  double B0_evtgen_n6= (double)h_B0_decay_evtgen->GetBinContent(8);
  double B0_evtgen_n7= (double)h_B0_decay_evtgen->GetBinContent(9);

  cout<<"B0_evtgen_entries = "<<B0_evtgen_entries<<" , B0_evtgen_n1 = "<<B0_evtgen_n1<<" , ratio= "<<B0_evtgen_n1/B0_evtgen_entries<<endl;
  cout<<" ratio1 = "<<B0_evtgen_n1/B0_evtgen_entries<<endl;
  cout<<" ratio2 = "<<B0_evtgen_n2/B0_evtgen_entries<<endl;
  cout<<" ratio3 = "<<B0_evtgen_n3/B0_evtgen_entries<<endl;
  cout<<" ratio4 = "<<B0_evtgen_n4/B0_evtgen_entries<<endl;
  cout<<" ratio5 = "<<B0_evtgen_n5/B0_evtgen_entries<<endl;
  cout<<" ratio6 = "<<B0_evtgen_n6/B0_evtgen_entries<<endl;
  cout<<" ratio7 = "<<B0_evtgen_n7/B0_evtgen_entries<<endl;



	h_B0_decay_pythia->Scale(scale_factor/B0_pythia_entries);
	h_B0_decay_evtgen->Scale(scale_factor/B0_evtgen_entries);

	h_B0_decay_evtgen->GetXaxis()->SetRange(3,10);
//	h_B0_decay_evtgen->SetMarkerColor(1);
	h_B0_decay_evtgen->SetLineColor(2);
	h_B0_decay_evtgen->SetMarkerStyle(3);
	h_B0_decay_evtgen->SetMarkerColor(2);
	h_B0_decay_pythia->SetMarkerStyle(3);
	h_B0_decay_pythia->SetMarkerColor(4);
	h_B0_decay_pythia->SetLineColor(4);
	h_B0_decay_pythia->GetXaxis()->SetRange(3,10);
	h_B0_decay_pdg->GetXaxis()->SetRange(3,10);

  h_B0_decay_pythia->SetTitle("B^{0} decays");

	h_B0_decay_pythia->Draw();
	h_B0_decay_evtgen->Draw("SAME");
  h_B0_decay_pdg->Draw("PSAME");
	h_B0_decay_table->Draw("PSAME");

 TLegend *leg_B0 = new TLegend(0.75,0.75,0.9,0.9);
//  leg->SetHeader("header");
  leg_B0->AddEntry(h_B0_decay_pythia,"pythia6","p");
  leg_B0->AddEntry(h_B0_decay_evtgen,"evtgen","p");
  leg_B0->AddEntry(h_B0_decay_table,"evtgen_table","p");
  leg_B0->AddEntry(h_B0_decay_pdg,"pdg","p");
  leg_B0->Draw();

	C_B0_decay->SaveAs("B0_decay.pdf");



	TCanvas *C_B0bar_decay = new TCanvas("C_B0bar_decay","C_B0bar_decay");
  C_B0bar_decay->cd(1);
  double B0bar_pythia_entries= (double)h_B0bar_decay_pythia->GetEntries();
  double B0bar_evtgen_entries= (double)h_B0bar_decay_evtgen->GetEntries();
  h_B0bar_decay_pythia->Scale(scale_factor/B0bar_pythia_entries);
  h_B0bar_decay_evtgen->Scale(scale_factor/B0bar_evtgen_entries);

  h_B0bar_decay_evtgen->GetXaxis()->SetRange(3,10);
  h_B0bar_decay_evtgen->SetMarkerColor(2);
	h_B0bar_decay_evtgen->SetMarkerStyle(4);
	h_B0bar_decay_evtgen->SetLineColor(2);
  h_B0bar_decay_pythia->GetXaxis()->SetRange(3,10);
	h_B0bar_decay_pythia->SetMarkerStyle(4);
	h_B0bar_decay_pythia->SetMarkerColor(4);
	h_B0bar_decay_pythia->SetLineColor(4);
//  h_B0bar_decay_pdg->GetXaxis()->SetRange(3,10);
  h_B0bar_decay_pythia->SetTitle("B^{0}/B^{0}bar decays");

  h_B0bar_decay_pythia->Draw();
	h_B0_decay_pythia->Draw("PSAME");
  h_B0bar_decay_evtgen->Draw("PSAME");
	h_B0_decay_evtgen->Draw("PSAME");
 TLegend *leg_B0bar = new TLegend(0.75,0.75,0.9,0.9);
//  leg->SetHeader("header");
  leg_B0bar->AddEntry(h_B0_decay_pythia,"pythia6_B0","p");
  leg_B0bar->AddEntry(h_B0bar_decay_pythia,"pythia6_B0bar","p");
  leg_B0bar->AddEntry(h_B0_decay_evtgen,"evtgen_B0","p");
  leg_B0bar->AddEntry(h_B0bar_decay_evtgen,"evtgen_B0bar","p");
  leg_B0bar->Draw();

  C_B0bar_decay->SaveAs("B0bar_decay.pdf");


//  h_B0bar_decay_pdg->Draw("PSAME");

  TCanvas *C_Bp_decay = new TCanvas("C_Bp_decay","C_Bp_decay");
  double Bp_pythia_entries= (double)h_Bp_decay_pythia->GetEntries();
  double Bp_evtgen_entries= (double)h_Bp_decay_evtgen->GetEntries();
  h_Bp_decay_pythia->Scale(scale_factor/Bp_pythia_entries);
  h_Bp_decay_evtgen->Scale(scale_factor/Bp_evtgen_entries);

  h_Bp_decay_evtgen->GetXaxis()->SetRange(3,10);
  h_Bp_decay_evtgen->SetMarkerColor(2);
  h_Bp_decay_evtgen->SetMarkerStyle(2);
  h_Bp_decay_evtgen->SetLineColor(2);
  h_Bp_decay_pythia->GetXaxis()->SetRange(3,10);
  h_Bp_decay_pythia->SetMarkerStyle(2);
  h_Bp_decay_pythia->SetMarkerColor(4);
  h_Bp_decay_pythia->SetLineColor(4);
//  h_Bp_decay_pdg->GetXaxis()->SetRange(3,10);
  h_Bp_decay_pythia->SetTitle("B^{+} decays");

  h_Bp_decay_pythia->Draw();
  h_Bp_decay_evtgen->Draw("PSAME");
	h_Bp_decay_pdg->Draw("PSAME");
 TLegend *leg_Bp = new TLegend(0.75,0.75,0.9,0.9);
//  leg->SetHeader("header");
  leg_Bp->AddEntry(h_Bp_decay_pythia,"pythia6_Bp","p");
  leg_Bp->AddEntry(h_Bp_decay_evtgen,"evtgen_Bp","p");
  leg_Bp->AddEntry(h_Bp_decay_pdg,"pdg_Bp","p");
  leg_Bp->Draw();

  C_Bp_decay->SaveAs("Bp_decay.pdf");



  TCanvas *C_Bs_decay = new TCanvas("C_Bs_decay","C_Bs_decay");
  double Bs_pythia_entries= (double)h_Bs_decay_pythia->GetEntries();
  double Bs_evtgen_entries= (double)h_Bs_decay_evtgen->GetEntries();
  h_Bs_decay_pythia->Scale(scale_factor/Bs_pythia_entries);
  h_Bs_decay_evtgen->Scale(scale_factor/Bs_evtgen_entries);

  h_Bs_decay_evtgen->GetXaxis()->SetRange(3,10);
  h_Bs_decay_evtgen->SetMarkerColor(2);
  h_Bs_decay_evtgen->SetMarkerStyle(2);
  h_Bs_decay_evtgen->SetLineColor(2);
  h_Bs_decay_pythia->GetXaxis()->SetRange(3,10);
  h_Bs_decay_pythia->SetMarkerStyle(2);
  h_Bs_decay_pythia->SetMarkerColor(4);
  h_Bs_decay_pythia->SetLineColor(4);
//  h_Bs_decay_pdg->GetXaxis()->SetRange(3,10);
  h_Bs_decay_pythia->SetTitle("B_{S} decays");

  h_Bs_decay_pythia->Draw();
  h_Bs_decay_evtgen->Draw("PSAME");
  h_Bs_decay_pdg->Draw("PSAME");

 TLegend *leg_Bs = new TLegend(0.75,0.75,0.9,0.9);
//  leg->SetHeader("header");
  leg_Bs->AddEntry(h_Bs_decay_pythia,"pythia6_Bs","p");
  leg_Bs->AddEntry(h_Bs_decay_evtgen,"evtgen_Bs","p");
  leg_Bs->AddEntry(h_Bs_decay_pdg,"pdg_Bs","p");
  leg_Bs->Draw();

  C_Bs_decay->SaveAs("Bs_decay.pdf");

// work on TLegend.

}

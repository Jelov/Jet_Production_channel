/*
	 Based on the jet response analyzer
	 Modified by Matt Nguyen, November 2010

*/

// Modified by Cheng-Chieh, 2016.Feb.1
// to implement tagging for different b-jets production mechanism
// working on about line 1110
// loop start from about line 585 (useful info)

#include <fstream>

#include "HeavyIonsAnalysis/JetAnalysis/interface/HiInclusiveJetAnalyzer.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include <Math/DistFunc.h>
#include "TMath.h"

#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Framework/interface/EventSetup.h"

#include "Geometry/Records/interface/CaloGeometryRecord.h"

#include "DataFormats/CaloTowers/interface/CaloTowerCollection.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"
#include "SimDataFormats/HiGenData/interface/GenHIEvent.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutSetup.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMapRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMap.h"
#include "L1Trigger/GlobalTrigger/plugins/L1GlobalTrigger.h"

#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/HcalRecHit/interface/HcalRecHitCollections.h"

using namespace std;
using namespace edm;
using namespace reco;

HiInclusiveJetAnalyzer::HiInclusiveJetAnalyzer(const edm::ParameterSet& iConfig) :
	geo(0)
{

	doMatch_ = iConfig.getUntrackedParameter<bool>("matchJets",false);
	jetTagLabel_ = iConfig.getParameter<InputTag>("jetTag");
	jetTag_ = consumes<reco::JetView> (jetTagLabel_);
	jetTagPat_ = consumes<pat::JetCollection> (jetTagLabel_);
	matchTag_ = consumes<reco::JetView> (iConfig.getUntrackedParameter<InputTag>("matchTag"));
	matchTagPat_ = consumes<pat::JetCollection> (iConfig.getUntrackedParameter<InputTag>("matchTag"));

	// vtxTag_ = iConfig.getUntrackedParameter<edm::InputTag>("vtxTag",edm::InputTag("hiSelectedVertex"));
	vtxTag_ = consumes<vector<reco::Vertex> >        (iConfig.getUntrackedParameter<edm::InputTag>("vtxTag",edm::InputTag("hiSelectedVertex")));  
	// iConfig.getUntrackedParameter<edm::InputTag>("vtxTag",edm::InputTag("hiSelectedVertex"));
	trackTag_ = consumes<reco::TrackCollection> (iConfig.getParameter<InputTag>("trackTag"));
	useQuality_ = iConfig.getUntrackedParameter<bool>("useQuality",1);
	trackQuality_ = iConfig.getUntrackedParameter<string>("trackQuality","highPurity");

	isMC_ = iConfig.getUntrackedParameter<bool>("isMC",false);
	useHepMC_ = iConfig.getUntrackedParameter<bool> ("useHepMC",false);
	fillGenJets_ = iConfig.getUntrackedParameter<bool>("fillGenJets",false);

	doTrigger_ = iConfig.getUntrackedParameter<bool>("doTrigger",false);
	doHiJetID_ = iConfig.getUntrackedParameter<bool>("doHiJetID",false);
	doStandardJetID_ = iConfig.getUntrackedParameter<bool>("doStandardJetID",false);

	rParam = iConfig.getParameter<double>("rParam");
	hardPtMin_ = iConfig.getUntrackedParameter<double>("hardPtMin",4);
	jetPtMin_ = iConfig.getUntrackedParameter<double>("jetPtMin",10);

	if(isMC_){
		genjetTag_ = consumes<vector<reco::GenJet> > (iConfig.getParameter<InputTag>("genjetTag"));
		if(useHepMC_) eventInfoTag_ = consumes<HepMCProduct> (iConfig.getParameter<InputTag>("eventInfoTag"));
		eventGenInfoTag_ = consumes<GenEventInfoProduct> (iConfig.getParameter<InputTag>("eventInfoTag"));
	}
	verbose_ = iConfig.getUntrackedParameter<bool>("verbose",false);

	useVtx_ = iConfig.getUntrackedParameter<bool>("useVtx",false);
	useJEC_ = iConfig.getUntrackedParameter<bool>("useJEC",true);
	usePat_ = iConfig.getUntrackedParameter<bool>("usePAT",true);

	doLifeTimeTagging_ = iConfig.getUntrackedParameter<bool>("doLifeTimeTagging",false);
	doLifeTimeTaggingExtras_ = iConfig.getUntrackedParameter<bool>("doLifeTimeTaggingExtras",true);
	saveBfragments_  = iConfig.getUntrackedParameter<bool>("saveBfragments",false);
	skipCorrections_  = iConfig.getUntrackedParameter<bool>("skipCorrections",false);

	pfCandidateLabel_ = consumes<reco::PFCandidateCollection> (iConfig.getUntrackedParameter<edm::InputTag>("pfCandidateLabel",edm::InputTag("particleFlowTmp")));

	doTower = iConfig.getUntrackedParameter<bool>("doTower",false);
	if(doTower){
		TowerSrc_ = consumes<CaloTowerCollection>( iConfig.getUntrackedParameter<edm::InputTag>("towersSrc",edm::InputTag("towerMaker")));
	}

	// EBSrc_ = iConfig.getUntrackedParameter<edm::InputTag>("EBRecHitSrc",edm::InputTag("ecalRecHit","EcalRecHitsEB"));
	// EESrc_ = iConfig.getUntrackedParameter<edm::InputTag>("EERecHitSrc",edm::InputTag("ecalRecHit","EcalRecHitsEE"));
	// HcalRecHitHFSrc_ = iConfig.getUntrackedParameter<edm::InputTag>("hcalHFRecHitSrc",edm::InputTag("hfreco"));
	// HcalRecHitHBHESrc_ = iConfig.getUntrackedParameter<edm::InputTag>("hcalHBHERecHitSrc",edm::InputTag("hbhereco"));

	genParticleSrc_ = consumes<reco::GenParticleCollection> (iConfig.getUntrackedParameter<edm::InputTag>("genParticles",edm::InputTag("hiGenParticles")));

	if(doTrigger_){
		L1gtReadout_ = consumes< L1GlobalTriggerReadoutRecord > (iConfig.getParameter<edm::InputTag>("L1gtReadout"));
		hltResName_ = consumes< TriggerResults >(iConfig.getUntrackedParameter<string>("hltTrgResults","TriggerResults::HLT"));


		if (iConfig.exists("hltTrgNames"))
			hltTrgNames_ = iConfig.getUntrackedParameter<vector<string> >("hltTrgNames");

		if (iConfig.exists("hltProcNames"))
			hltProcNames_ = iConfig.getUntrackedParameter<vector<string> >("hltProcNames");
		else {
			hltProcNames_.push_back("FU");
			hltProcNames_.push_back("HLT");
		}
	}
	if(doLifeTimeTagging_){
		bTagJetName_ = iConfig.getUntrackedParameter<string>("bTagJetName");
		ImpactParameterTagInfos_ = consumes<vector<TrackIPTagInfo> > (iConfig.getUntrackedParameter<string>("ImpactParameterTagInfos",(bTagJetName_+"ImpactParameterTagInfos")));
		TrackCountingHighEffBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("TrackCountingHighEffBJetTags",(bTagJetName_+"TrackCountingHighEffBJetTags")));
		NegativeTrackCountingHighEffBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("NegativeTrackCountingHighEffBJetTags",bTagJetName_+("NegativeTrackCountingHighEffBJetTags")));
		TrackCountingHighPurBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("TrackCountingHighPurBJetTags",(bTagJetName_+"TrackCountingHighPurBJetTags")));
		NegativeTrackCountingHighPurBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("NegativeTrackCountingHighPurBJetTags",(bTagJetName_+"NegativeTrackCountingHighPurBJetTags")));
		JetProbabilityBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("JetProbabilityBJetTags",(bTagJetName_+"JetProbabilityBJetTags")));
		PositiveOnlyJetProbabilityBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("PositiveOnlyJetProbabilityBJetTags",(bTagJetName_+"PositiveOnlyJetProbabilityBJetTags")));
		NegativeOnlyJetProbabilityBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("NegativeOnlyJetProbabilityBJetTags",(bTagJetName_+"NegativeOnlyJetProbabilityBJetTags")));
		JetBProbabilityBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("JetBProbabilityBJetTags",(bTagJetName_+"JetBProbabilityBJetTags")));
		NegativeOnlyJetBProbabilityBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("NegativeOnlyJetBProbabilityBJetTags",(bTagJetName_+"NegativeOnlyJetBProbabilityBJetTags")));
		PositiveOnlyJetBProbabilityBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("PositiveOnlyJetBProbabilityBJetTags",(bTagJetName_+"PositiveOnlyJetBProbabilityBJetTags")));
		SecondaryVertexTagInfos_ = consumes<vector<SecondaryVertexTagInfo> > (iConfig.getUntrackedParameter<string>("SecondaryVertexTagInfos",(bTagJetName_+"SecondaryVertexTagInfos")));
		SecondaryVertexNegativeTagInfos_ = consumes<vector<SecondaryVertexTagInfo> > (iConfig.getUntrackedParameter<string>("SecondaryVertexNegativeTagInfos",(bTagJetName_+"SecondaryVertexNegativeTagInfos")));
		SimpleSecondaryVertexHighEffBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("SimpleSecondaryVertexHighEffBJetTags",(bTagJetName_+"SimpleSecondaryVertexHighEffBJetTags")));
		NegativeSimpleSecondaryVertexHighEffBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("NegativeSimpleSecondaryVertexHighEffBJetTags",(bTagJetName_+"NegativeSimpleSecondaryVertexHighEffBJetTags")));
		SimpleSecondaryVertexHighPurBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("SimpleSecondaryVertexHighPurBJetTags",(bTagJetName_+"SimpleSecondaryVertexHighPurBJetTags")));
		NegativeSimpleSecondaryVertexHighPurBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("NegativeSimpleSecondaryVertexHighPurBJetTags",(bTagJetName_+"NegativeSimpleSecondaryVertexHighPurBJetTags")));
		CombinedSecondaryVertexBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("CombinedSecondaryVertexBJetTags",(bTagJetName_+"CombinedSecondaryVertexBJetTags")));
		NegativeCombinedSecondaryVertexBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("NegativeCombinedSecondaryVertexBJetTags",(bTagJetName_+"NegativeCombinedSecondaryVertexBJetTags")));
		PositiveCombinedSecondaryVertexBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("PositiveCombinedSecondaryVertexBJetTags",(bTagJetName_+"PositiveCombinedSecondaryVertexBJetTags")));
		NegativeSoftPFMuonByPtBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("NegativeSoftPFMuonByPtBJetTags",(bTagJetName_+"NegativeSoftPFMuonByPtBJetTags")));
		PositiveSoftPFMuonByPtBJetTags_ = consumes<JetTagCollection> (iConfig.getUntrackedParameter<string>("PositiveSoftPFMuonByPtBJetTags",(bTagJetName_+"PositiveSoftPFMuonByPtBJetTags")));
	}

	//  cout<<" jet collection : "<<jetTag_<<endl;
	doSubEvent_ = 0;

	if(isMC_){
		//     cout<<" genjet collection : "<<genjetTag_<<endl;
		genPtMin_ = iConfig.getUntrackedParameter<double>("genPtMin",10);
		doSubEvent_ = iConfig.getUntrackedParameter<bool>("doSubEvent",0);
	}


}



HiInclusiveJetAnalyzer::~HiInclusiveJetAnalyzer() { }



void
HiInclusiveJetAnalyzer::beginRun(const edm::Run& run,
		const edm::EventSetup & es) {}

void
HiInclusiveJetAnalyzer::beginJob() {

	//string jetTagName = jetTag_.label()+"_tree";
	string jetTagTitle = jetTagLabel_.label()+" Jet Analysis Tree";
	t = fs1->make<TTree>("t",jetTagTitle.c_str());
	string historgramtitle = jetTagTitle+"qJet dR all";
  h_qJet_dR_all  = fs1->make<TH1F>("h_qJet_dR_all",historgramtitle.c_str(),150,0,3); h_qJet_dR_all->Sumw2();
  string historgramtitle1 = jetTagTitle+"qJet dR flavormatched";
  h_qJet_dR_flavormatched = fs1->make<TH1F>("h_qJet_dR_flavormatched",historgramtitle1.c_str(),150,0,3); h_qJet_dR_flavormatched->Sumw2();

	//  TTree* t= new TTree("t","Jet Response Analyzer");
	//t->Branch("run",&jets_.run,"run/I");
	t->Branch("evt",&jets_.evt,"evt/I");
	//t->Branch("lumi",&jets_.lumi,"lumi/I");
	t->Branch("b",&jets_.b,"b/F");
	if (useVtx_) {
		t->Branch("vx",&jets_.vx,"vx/F");
		t->Branch("vy",&jets_.vy,"vy/F");
		t->Branch("vz",&jets_.vz,"vz/F");
	}

	t->Branch("nref",&jets_.nref,"nref/I");
	t->Branch("rawpt",jets_.rawpt,"rawpt[nref]/F");
	if(!skipCorrections_) t->Branch("jtpt",jets_.jtpt,"jtpt[nref]/F");
	t->Branch("jtenergy",jets_.jtenergy,"jtenergy[nref]/F");
	t->Branch("jteta",jets_.jteta,"jteta[nref]/F");
	t->Branch("jty",jets_.jty,"jty[nref]/F");
	t->Branch("jtphi",jets_.jtphi,"jtphi[nref]/F");
	t->Branch("jtpu",jets_.jtpu,"jtpu[nref]/F");
	t->Branch("jtm",jets_.jtm,"jtm[nref]/F");

	// jet ID information, jet composition
	if(doHiJetID_){
		t->Branch("discr_fr01", jets_.discr_fr01,"discr_fr01[nref]/F");

		t->Branch("trackMax", jets_.trackMax,"trackMax[nref]/F");
		t->Branch("trackSum", jets_.trackSum,"trackSum[nref]/F");
		t->Branch("trackN", jets_.trackN,"trackN[nref]/I");
		t->Branch("trackHardSum", jets_.trackHardSum,"trackHardSum[nref]/F");
		t->Branch("trackHardN", jets_.trackHardN,"trackHardN[nref]/I");

		t->Branch("chargedMax", jets_.chargedMax,"chargedMax[nref]/F");
		t->Branch("chargedSum", jets_.chargedSum,"chargedSum[nref]/F");
		t->Branch("chargedN", jets_.chargedN,"chargedN[nref]/I");
		t->Branch("chargedHardSum", jets_.chargedHardSum,"chargedHardSum[nref]/F");
		t->Branch("chargedHardN", jets_.chargedHardN,"chargedHardN[nref]/I");

		t->Branch("photonMax", jets_.photonMax,"photonMax[nref]/F");
		t->Branch("photonSum", jets_.photonSum,"photonSum[nref]/F");
		t->Branch("photonN", jets_.photonN,"photonN[nref]/I");
		t->Branch("photonHardSum", jets_.photonHardSum,"photonHardSum[nref]/F");
		t->Branch("photonHardN", jets_.photonHardN,"photonHardN[nref]/I");

		t->Branch("neutralMax", jets_.neutralMax,"neutralMax[nref]/F");
		t->Branch("neutralSum", jets_.neutralSum,"neutralSum[nref]/F");
		t->Branch("neutralN", jets_.neutralN,"neutralN[nref]/I");

		t->Branch("hcalSum", jets_.hcalSum,"hcalSum[nref]/F");
		t->Branch("ecalSum", jets_.ecalSum,"ecalSum[nref]/F");

		t->Branch("eMax", jets_.eMax,"eMax[nref]/F");
		t->Branch("eSum", jets_.eSum,"eSum[nref]/F");
		t->Branch("eN", jets_.eN,"eN[nref]/I");

		t->Branch("muMax", jets_.muMax,"muMax[nref]/F");
		t->Branch("muSum", jets_.muSum,"muSum[nref]/F");
		t->Branch("muN", jets_.muN,"muN[nref]/I");
	}

	if(doStandardJetID_){
		t->Branch("fHPD",jets_.fHPD,"fHPD[nref]/F");
		t->Branch("fRBX",jets_.fRBX,"fRBX[nref]/F");
		t->Branch("n90",jets_.n90,"n90[nref]/I");
		t->Branch("fSubDet1",jets_.fSubDet1,"fSubDet1[nref]/F");
		t->Branch("fSubDet2",jets_.fSubDet2,"fSubDet2[nref]/F");
		t->Branch("fSubDet3",jets_.fSubDet3,"fSubDet3[nref]/F");
		t->Branch("fSubDet4",jets_.fSubDet4,"fSubDet4[nref]/F");
		t->Branch("restrictedEMF",jets_.restrictedEMF,"restrictedEMF[nref]/F");
		t->Branch("nHCAL",jets_.nHCAL,"nHCAL[nref]/I");
		t->Branch("nECAL",jets_.nECAL,"nECAL[nref]/I");
		t->Branch("apprHPD",jets_.apprHPD,"apprHPD[nref]/F");
		t->Branch("apprRBX",jets_.apprRBX,"apprRBX[nref]/F");

		//  t->Branch("hitsInN90",jets_.n90,"hitsInN90[nref]");
		t->Branch("n2RPC",jets_.n2RPC,"n2RPC[nref]/I");
		t->Branch("n3RPC",jets_.n3RPC,"n3RPC[nref]/I");
		t->Branch("nRPC",jets_.nRPC,"nRPC[nref]/I");

		t->Branch("fEB",jets_.fEB,"fEB[nref]/F");
		t->Branch("fEE",jets_.fEE,"fEE[nref]/F");
		t->Branch("fHB",jets_.fHB,"fHB[nref]/F");
		t->Branch("fHE",jets_.fHE,"fHE[nref]/F");
		t->Branch("fHO",jets_.fHO,"fHO[nref]/F");
		t->Branch("fLong",jets_.fLong,"fLong[nref]/F");
		t->Branch("fShort",jets_.fShort,"fShort[nref]/F");
		t->Branch("fLS",jets_.fLS,"fLS[nref]/F");
		t->Branch("fHFOOT",jets_.fHFOOT,"fHFOOT[nref]/F");
	}

	// Jet ID
	if(doMatch_){
		if(!skipCorrections_) t->Branch("matchedPt", jets_.matchedPt,"matchedPt[nref]/F");
		t->Branch("matchedRawPt", jets_.matchedRawPt,"matchedRawPt[nref]/F");
		t->Branch("matchedPu", jets_.matchedPu,"matchedPu[nref]/F");
		t->Branch("matchedR", jets_.matchedR,"matchedR[nref]/F");
	}

	// b-jet discriminators
	if (doLifeTimeTagging_) {

		t->Branch("discr_ssvHighEff",jets_.discr_ssvHighEff,"discr_ssvHighEff[nref]/F");
		t->Branch("discr_ssvHighPur",jets_.discr_ssvHighPur,"discr_ssvHighPur[nref]/F");

		t->Branch("discr_csvMva",jets_.discr_csvMva,"discr_csvMva[nref]/F");
		t->Branch("discr_csvSimple",jets_.discr_csvSimple,"discr_csvSimple[nref]/F");
		t->Branch("discr_muByIp3",jets_.discr_muByIp3,"discr_muByIp3[nref]/F");
		t->Branch("discr_muByPt",jets_.discr_muByPt,"discr_muByPt[nref]/F");
		t->Branch("discr_prob",jets_.discr_prob,"discr_prob[nref]/F");
		t->Branch("discr_probb",jets_.discr_probb,"discr_probb[nref]/F");
		t->Branch("discr_tcHighEff",jets_.discr_tcHighEff,"discr_tcHighEff[nref]/F");
		t->Branch("discr_tcHighPur",jets_.discr_tcHighPur,"discr_tcHighPur[nref]/F");

		t->Branch("ndiscr_ssvHighEff",jets_.ndiscr_ssvHighEff,"ndiscr_ssvHighEff[nref]/F");
		t->Branch("ndiscr_ssvHighPur",jets_.ndiscr_ssvHighPur,"ndiscr_ssvHighPur[nref]/F");
		t->Branch("ndiscr_csvSimple",jets_.ndiscr_csvSimple,"ndiscr_csvSimple[nref]/F");
		t->Branch("ndiscr_muByPt",jets_.ndiscr_muByPt,"ndiscr_muByPt[nref]/F");
		t->Branch("ndiscr_prob",jets_.ndiscr_prob,"ndiscr_prob[nref]/F");
		t->Branch("ndiscr_probb",jets_.ndiscr_probb,"ndiscr_probb[nref]/F");
		t->Branch("ndiscr_tcHighEff",jets_.ndiscr_tcHighEff,"ndiscr_tcHighEff[nref]/F");
		t->Branch("ndiscr_tcHighPur",jets_.ndiscr_tcHighPur,"ndiscr_tcHighPur[nref]/F");

		t->Branch("pdiscr_csvSimple",jets_.pdiscr_csvSimple,"pdiscr_csvSimple[nref]/F");
		t->Branch("pdiscr_prob",jets_.pdiscr_prob,"pdiscr_prob[nref]/F");
		t->Branch("pdiscr_probb",jets_.pdiscr_probb,"pdiscr_probb[nref]/F");

		t->Branch("nsvtx",    jets_.nsvtx,    "nsvtx[nref]/I");
		t->Branch("svtxntrk", jets_.svtxntrk, "svtxntrk[nref]/I");
		t->Branch("svtxdl",   jets_.svtxdl,   "svtxdl[nref]/F");
		t->Branch("svtxdls",  jets_.svtxdls,  "svtxdls[nref]/F");
		t->Branch("svtxdl2d", jets_.svtxdl2d, "svtxdl2d[nref]/F");
		t->Branch("svtxdls2d", jets_.svtxdls2d, "svtxdls2d[nref]/F");
		t->Branch("svtxm",    jets_.svtxm,    "svtxm[nref]/F");
		t->Branch("svtxpt",   jets_.svtxpt,   "svtxpt[nref]/F");
		t->Branch("svtxmcorr", jets_.svtxmcorr, "svtxmcorr[nref]/F");

		t->Branch("nIPtrk",jets_.nIPtrk,"nIPtrk[nref]/I");
		t->Branch("nselIPtrk",jets_.nselIPtrk,"nselIPtrk[nref]/I");

		if (doLifeTimeTaggingExtras_) {
			t->Branch("nIP",&jets_.nIP,"nIP/I");
			t->Branch("ipJetIndex",jets_.ipJetIndex,"ipJetIndex[nIP]/I");
			t->Branch("ipPt",jets_.ipPt,"ipPt[nIP]/F");
			t->Branch("ipProb0",jets_.ipProb0,"ipProb0[nIP]/F");
			t->Branch("ipProb1",jets_.ipProb1,"ipProb1[nIP]/F");
			t->Branch("ip2d",jets_.ip2d,"ip2d[nIP]/F");
			t->Branch("ip2dSig",jets_.ip2dSig,"ip2dSig[nIP]/F");
			t->Branch("ip3d",jets_.ip3d,"ip3d[nIP]/F");
			t->Branch("ip3dSig",jets_.ip3dSig,"ip3dSig[nIP]/F");
			t->Branch("ipDist2Jet",jets_.ipDist2Jet,"ipDist2Jet[nIP]/F");
			t->Branch("ipDist2JetSig",jets_.ipDist2JetSig,"ipDist2JetSig[nIP]/F");
			t->Branch("ipClosest2Jet",jets_.ipClosest2Jet,"ipClosest2Jet[nIP]/F");

		}

		t->Branch("mue",     jets_.mue,     "mue[nref]/F");
		t->Branch("mupt",    jets_.mupt,    "mupt[nref]/F");
		t->Branch("mueta",   jets_.mueta,   "mueta[nref]/F");
		t->Branch("muphi",   jets_.muphi,   "muphi[nref]/F");
		t->Branch("mudr",    jets_.mudr,    "mudr[nref]/F");
		t->Branch("muptrel", jets_.muptrel, "muptrel[nref]/F");
		t->Branch("muchg",   jets_.muchg,   "muchg[nref]/I");
	}


	if(isMC_){
		t->Branch("beamId1",&jets_.beamId1,"beamId1/I");
		t->Branch("beamId2",&jets_.beamId2,"beamId2/I");

		t->Branch("pthat",&jets_.pthat,"pthat/F");

		// Only matched gen jets
		t->Branch("refpt",jets_.refpt,"refpt[nref]/F");
		t->Branch("refeta",jets_.refeta,"refeta[nref]/F");
		t->Branch("refy",jets_.refy,"refy[nref]/F");
		t->Branch("refphi",jets_.refphi,"refphi[nref]/F");
		t->Branch("refdphijt",jets_.refdphijt,"refdphijt[nref]/F");
		t->Branch("refdrjt",jets_.refdrjt,"refdrjt[nref]/F");

		// Only mactched gen jets, production channel info

		t->Branch("refjet_prodChannel",jets_.refjet_prodChannel,"refjet_prodChannel[nref]/I");	
		t->Branch("ref_q_TwinFound",jets_.ref_q_Twinfound,"ref_q_TwinFound[nref]/I");
		t->Branch("ref_qfJet_dR",jets_.ref_qfJet_dR,"ref_qfJet_dR[nref]/F");
		t->Branch("ref_qfJet_dpt",jets_.ref_qfJet_dpt,"ref_qfJet_dpt[nref]/F");
		t->Branch("ref_q_pt", jets_.ref_q_pt, "ref_q_pt[nref]/F");
		t->Branch("ref_q_eta",jets_.ref_q_eta,"ref_q_eta[nref]/F");
		t->Branch("ref_q_phi",jets_.ref_q_phi, "ref_q_phi[nref]/F");
		t->Branch("ref_qTwin_pt", jets_.ref_qTwin_pt, "ref_qTwin_pt[nref]/F");
		t->Branch("ref_qTwin_eta",jets_.ref_qTwin_eta,"ref_qTwin_eta[nref]/F");
		t->Branch("ref_qTwin_phi",jets_.ref_qTwin_phi,"ref_qTwin_phi[nref]/F");
		t->Branch("ref_qTwin_dphi",jets_.ref_qTwin_dphi,"ref_qTwin_dphi[nref]/F");
		t->Branch("ref_qTwin_dtheta",jets_.ref_qTwin_dtheta,"ref_qTwin_dtheta[nref]/F");
		t->Branch("ref_qMom_pt", jets_.ref_qMom_pt, "ref_qMom_pt[nref]/F");
		t->Branch("ref_qMom_eta",jets_.ref_qMom_eta,"ref_qMom_eta[nref]/F");
		t->Branch("ref_qMom_phi",jets_.ref_qMom_phi,"ref_qMom_phi[nref]/F");


		// matched parton
		t->Branch("refparton_pt",jets_.refparton_pt,"refparton_pt[nref]/F");
		t->Branch("refparton_flavor",jets_.refparton_flavor,"refparton_flavor[nref]/I");
		t->Branch("refparton_flavorForB",jets_.refparton_flavorForB,"refparton_flavorForB[nref]/I");

		t->Branch("refparton_status",jets_.refparton_status,"refparton_status[nref]/I");
		t->Branch("refparton_isGSP",jets_.refparton_isGSP,"refparton_isGSP[nref]/O");

		t->Branch("genChargedSum", jets_.genChargedSum,"genChargedSum[nref]/F");
		t->Branch("genHardSum", jets_.genHardSum,"genHardSum[nref]/F");
		t->Branch("signalChargedSum", jets_.signalChargedSum,"signalChargedSum[nref]/F");
		t->Branch("signalHardSum", jets_.signalHardSum,"signalHardSum[nref]/F");

		if(doSubEvent_){
			t->Branch("subid",jets_.subid,"subid[nref]/I");
		}

		if(fillGenJets_){
			// For all gen jets, matched or unmatched
			t->Branch("ngen",&jets_.ngen,"ngen/I");
			t->Branch("genmatchindex",jets_.genmatchindex,"genmatchindex[ngen]/I");
			t->Branch("genpt",jets_.genpt,"genpt[ngen]/F");
			t->Branch("geneta",jets_.geneta,"geneta[ngen]/F");
			t->Branch("geny",jets_.geny,"geny[ngen]/F");
			t->Branch("genphi",jets_.genphi,"genphi[ngen]/F");
			t->Branch("gendphijt",jets_.gendphijt,"gendphijt[ngen]/F");
			t->Branch("gendrjt",jets_.gendrjt,"gendrjt[ngen]/F");

			if(doSubEvent_){
				t->Branch("gensubid",jets_.gensubid,"gensubid[ngen]/I");
			}
		}

		if(saveBfragments_  ) {
			t->Branch("bMult",&jets_.bMult,"bMult/I");
			t->Branch("bJetIndex",jets_.bJetIndex,"bJetIndex[bMult]/I");
			t->Branch("bStatus",jets_.bStatus,"bStatus[bMult]/I");
			t->Branch("bVx",jets_.bVx,"bVx[bMult]/F");
			t->Branch("bVy",jets_.bVy,"bVy[bMult]/F");
			t->Branch("bVz",jets_.bVz,"bVz[bMult]/F");
			t->Branch("bPt",jets_.bPt,"bPt[bMult]/F");
			t->Branch("bEta",jets_.bEta,"bEta[bMult]/F");
			t->Branch("bPhi",jets_.bPhi,"bPhi[bMult]/F");
			t->Branch("bPdg",jets_.bPdg,"bPdg[bMult]/I");
			t->Branch("bChg",jets_.bChg,"bChg[bMult]/I");
		}

	}
	/*
		 if(!isMC_){
		 t->Branch("nL1TBit",&jets_.nL1TBit,"nL1TBit/I");
		 t->Branch("l1TBit",jets_.l1TBit,"l1TBit[nL1TBit]/O");

		 t->Branch("nL1ABit",&jets_.nL1ABit,"nL1ABit/I");
		 t->Branch("l1ABit",jets_.l1ABit,"l1ABit[nL1ABit]/O");

		 t->Branch("nHLTBit",&jets_.nHLTBit,"nHLTBit/I");
		 t->Branch("hltBit",jets_.hltBit,"hltBit[nHLTBit]/O");

		 }
		 */
	TH1D::SetDefaultSumw2();


}

int nth_event=-1;
int event_temp=0;

void
HiInclusiveJetAnalyzer::analyze(const Event& iEvent,
		const EventSetup& iSetup) {

	char filename[]="MC_particle_status.txt";
	fstream myfile;
	myfile.open (filename,ios::out|ios::app);
	//			myfile.open (filename,ios::out);
	int event = iEvent.id().event();
	int run = iEvent.id().run();
	int lumi = iEvent.id().luminosityBlock();

	if (event != event_temp){
		event_temp=event;
		nth_event++;
		myfile<<"\n\nnew event :"<<nth_event<<" th event , with iEvent.id().event() = "<<event_temp<<endl;
	}	

	jets_.run = run;
	jets_.evt = event;
	jets_.lumi = lumi;

	LogDebug("HiInclusiveJetAnalyzer")<<"START event: "<<event<<" in run "<<run<<endl;

	int bin = -1;
	double hf = 0.;
	double b = 999.;

	if(doHiJetID_ && !geo){
		edm::ESHandle<CaloGeometry> pGeo;
		iSetup.get<CaloGeometryRecord>().get(pGeo);
		geo = pGeo.product();
	}

	// loop the events

	jets_.bin = bin;
	jets_.hf = hf;

	reco::Vertex::Point vtx(0,0,0);
	if (useVtx_) {
		edm::Handle<vector<reco::Vertex> >vertex;
		iEvent.getByToken(vtxTag_, vertex);

		if(vertex->size()>0) {
			jets_.vx=vertex->begin()->x();
			jets_.vy=vertex->begin()->y();
			jets_.vz=vertex->begin()->z();
			vtx = vertex->begin()->position();
		}
	}

	edm::Handle<pat::JetCollection> patjets;
	if(usePat_)iEvent.getByToken(jetTagPat_, patjets);

	edm::Handle<pat::JetCollection> patmatchedjets;
	iEvent.getByToken(matchTagPat_, patmatchedjets);

	edm::Handle<reco::JetView> matchedjets;
	iEvent.getByToken(matchTag_, matchedjets);

	edm::Handle<reco::JetView> jets;
	iEvent.getByToken(jetTag_, jets);

	edm::Handle<reco::PFCandidateCollection> pfCandidates;
	iEvent.getByToken(pfCandidateLabel_,pfCandidates);

	edm::Handle<reco::TrackCollection> tracks;
	iEvent.getByToken(trackTag_,tracks);

	// edm::Handle<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > ebHits;
	// edm::Handle<edm::SortedCollection<EcalRecHit,edm::StrictWeakOrdering<EcalRecHit> > > eeHits;

	//edm::Handle<HFRecHitCollection> hfHits;
	//edm::Handle<HBHERecHitCollection> hbheHits;


	edm::Handle<reco::GenParticleCollection> genparts;
	iEvent.getByToken(genParticleSrc_,genparts);


	//Get all the b-tagging handles
	// Track Counting Taggers
	//------------------------------------------------------
	Handle<JetTagCollection> jetTags_TCHighEff;
	Handle<JetTagCollection> jetTags_NegTCHighEff;
	Handle<JetTagCollection> jetTags_TCHighPur;
	Handle<JetTagCollection> jetTags_NegTCHighPur;

	//------------------------------------------------------
	// Jet Probability tagger
	//------------------------------------------------------
	Handle<vector<TrackIPTagInfo> > tagInfo;
	Handle<JetTagCollection> jetTags_JP;
	Handle<JetTagCollection> jetTags_PosJP;
	Handle<JetTagCollection> jetTags_NegJP;
	Handle<JetTagCollection> jetTags_JB;
	Handle<JetTagCollection> jetTags_NegJB;
	Handle<JetTagCollection> jetTags_PosJB;

	//------------------------------------------------------
	// Secondary vertex taggers
	//------------------------------------------------------
	Handle<vector<SecondaryVertexTagInfo> > tagInfoSVx;
	Handle<vector<SecondaryVertexTagInfo> > tagInfoNegSVx;
	Handle<JetTagCollection> jetTags_SvtxHighEff;
	Handle<JetTagCollection> jetTags_negSvtxHighEff;
	Handle<JetTagCollection> jetTags_SvtxHighPur;
	Handle<JetTagCollection> jetTags_negSvtxHighPur;
	Handle<JetTagCollection> jetTags_CombinedSvtx;
	Handle<JetTagCollection> jetTags_negCombinedSvtx;
	Handle<JetTagCollection> jetTags_posCombinedSvtx;

	//------------------------------------------------------
	// Soft muon tagger
	//------------------------------------------------------

	//Handle<SoftLeptonTagInfoCollection> tagInos_softmuon;
	//Handle<JetTagCollection> jetTags_softMu;
	//Handle<JetTagCollection> jetTags_softMuneg;

	if(doLifeTimeTagging_){
		iEvent.getByToken(ImpactParameterTagInfos_, tagInfo);
		iEvent.getByToken(TrackCountingHighEffBJetTags_, jetTags_TCHighEff);
		iEvent.getByToken(NegativeTrackCountingHighEffBJetTags_, jetTags_NegTCHighEff);
		iEvent.getByToken(TrackCountingHighPurBJetTags_, jetTags_TCHighPur);
		iEvent.getByToken(NegativeTrackCountingHighPurBJetTags_, jetTags_NegTCHighPur);
		iEvent.getByToken(JetProbabilityBJetTags_, jetTags_JP);
		iEvent.getByToken(PositiveOnlyJetProbabilityBJetTags_, jetTags_PosJP);
		iEvent.getByToken(NegativeOnlyJetProbabilityBJetTags_, jetTags_NegJP);
		iEvent.getByToken(JetBProbabilityBJetTags_, jetTags_JB);
		iEvent.getByToken(NegativeOnlyJetBProbabilityBJetTags_, jetTags_NegJB);
		iEvent.getByToken(PositiveOnlyJetBProbabilityBJetTags_, jetTags_PosJB);
		iEvent.getByToken(SecondaryVertexTagInfos_, tagInfoSVx);
		iEvent.getByToken(SecondaryVertexNegativeTagInfos_, tagInfoNegSVx);
		iEvent.getByToken(SimpleSecondaryVertexHighEffBJetTags_, jetTags_SvtxHighEff);
		iEvent.getByToken(NegativeSimpleSecondaryVertexHighEffBJetTags_, jetTags_negSvtxHighEff);
		iEvent.getByToken(SimpleSecondaryVertexHighPurBJetTags_, jetTags_SvtxHighPur);
		iEvent.getByToken(NegativeSimpleSecondaryVertexHighEffBJetTags_, jetTags_negSvtxHighPur);
		iEvent.getByToken(CombinedSecondaryVertexBJetTags_, jetTags_CombinedSvtx);
		iEvent.getByToken(NegativeCombinedSecondaryVertexBJetTags_, jetTags_negCombinedSvtx);
		iEvent.getByToken(PositiveCombinedSecondaryVertexBJetTags_, jetTags_posCombinedSvtx);
		//iEvent.getByToken(NegativeSoftPFMuonByPtBJetTags_, jetTags_softMuneg);
		//iEvent.getByToken(PositiveSoftPFMuonByPtBJetTags_, jetTags_softMu);
	}


	// get tower information
	edm::Handle<CaloTowerCollection> towers;
	if(doTower){
		iEvent.getByToken(TowerSrc_,towers);
	}

	// FILL JRA TREE
	jets_.b = b;
	jets_.nref = 0;


	if(doTrigger_){
		fillL1Bits(iEvent);
		fillHLTBits(iEvent);
	}

	for(unsigned int j = 0; j < jets->size(); ++j){
		const reco::Jet& jet = (*jets)[j];

		if(jet.pt() < jetPtMin_) continue;
		if (useJEC_ && usePat_){
			jets_.rawpt[jets_.nref]=(*patjets)[j].correctedJet("Uncorrected").pt();
		}

		if(doLifeTimeTagging_){
			int ith_tagged =    this->TaggedJet(jet,jetTags_SvtxHighEff);
			if(ith_tagged >= 0){
				jets_.discr_ssvHighEff[jets_.nref] = (*jetTags_SvtxHighEff)[ith_tagged].second;
				const SecondaryVertexTagInfo &tagInfoSV = (*tagInfoSVx)[ith_tagged];
				jets_.nsvtx[jets_.nref]     = tagInfoSV.nVertices();

				if ( jets_.nsvtx[jets_.nref] > 0) {

					jets_.svtxntrk[jets_.nref]  = tagInfoSV.nVertexTracks(0);

					// this is the 3d flight distance, for 2-D use (0,true)
					Measurement1D m1D = tagInfoSV.flightDistance(0);
					jets_.svtxdl[jets_.nref]    = m1D.value();
					jets_.svtxdls[jets_.nref]   = m1D.significance();
					Measurement1D m2D = tagInfoSV.flightDistance(0,true);
					jets_.svtxdl2d[jets_.nref] = m2D.value();
					jets_.svtxdls2d[jets_.nref] = m2D.significance();
					const Vertex& svtx = tagInfoSV.secondaryVertex(0);

					double svtxM = svtx.p4().mass();
					double svtxPt = svtx.p4().pt();
					jets_.svtxm[jets_.nref]    = svtxM; 
					jets_.svtxpt[jets_.nref]   = svtxPt;

					//try out the corrected mass (http://arxiv.org/pdf/1504.07670v1.pdf)
					//mCorr=srqt(m^2+p^2sin^2(th)) + p*sin(th)
					double sinth = svtx.p4().Vect().Unit().Cross(tagInfoSV.flightDirection(0).unit()).Mag2();
					sinth = sqrt(sinth);
					jets_.svtxmcorr[jets_.nref] = sqrt(pow(svtxM,2)+(pow(svtxPt,2)*pow(sinth,2)))+svtxPt*sinth;

					if(svtx.ndof()>0)jets_.svtxnormchi2[jets_.nref]  = svtx.chi2()/svtx.ndof();
				}
			}
			ith_tagged    = this->TaggedJet(jet,jetTags_negSvtxHighEff);
			if(ith_tagged >= 0) jets_.ndiscr_ssvHighEff[jets_.nref]   = (*jetTags_negSvtxHighEff)[ith_tagged].second;
			ith_tagged    = this->TaggedJet(jet,jetTags_SvtxHighPur);
			if(ith_tagged >= 0) jets_.discr_ssvHighPur[jets_.nref]  = (*jetTags_SvtxHighPur)[ith_tagged].second;
			ith_tagged    = this->TaggedJet(jet,jetTags_negSvtxHighPur);
			if(ith_tagged >= 0) jets_.ndiscr_ssvHighPur[jets_.nref] = (*jetTags_negSvtxHighPur)[ith_tagged].second;

			ith_tagged          = this->TaggedJet(jet,jetTags_CombinedSvtx);
			if(ith_tagged >= 0) jets_.discr_csvSimple[jets_.nref]  = (*jetTags_CombinedSvtx)[ith_tagged].second;
			ith_tagged          = this->TaggedJet(jet,jetTags_negCombinedSvtx);
			if(ith_tagged >= 0) jets_.ndiscr_csvSimple[jets_.nref] = (*jetTags_negCombinedSvtx)[ith_tagged].second;
			ith_tagged  = this->TaggedJet(jet,jetTags_posCombinedSvtx);
			if(ith_tagged >= 0) jets_.pdiscr_csvSimple[jets_.nref] = (*jetTags_posCombinedSvtx)[ith_tagged].second;

			if(ith_tagged >= 0){
				ith_tagged = this->TaggedJet(jet,jetTags_JP);
				jets_.discr_prob[jets_.nref]  = (*jetTags_JP)[ith_tagged].second;

				const TrackIPTagInfo& tagInfoIP= (*tagInfo)[ith_tagged];

				jets_.nIPtrk[jets_.nref] = tagInfoIP.tracks().size();
				jets_.nselIPtrk[jets_.nref] = tagInfoIP.selectedTracks().size();
				if (doLifeTimeTaggingExtras_) {

					TrackRefVector selTracks=tagInfoIP.selectedTracks();

					GlobalPoint pv(tagInfoIP.primaryVertex()->position().x(),tagInfoIP.primaryVertex()->position().y(),tagInfoIP.primaryVertex()->position().z());

					for(int it=0;it<jets_.nselIPtrk[jets_.nref] ;it++)
					{
						jets_.ipJetIndex[jets_.nIP + it]= jets_.nref;
						reco::btag::TrackIPData data = tagInfoIP.impactParameterData()[it];
						jets_.ipPt[jets_.nIP + it] = selTracks[it]->pt();
						jets_.ipEta[jets_.nIP + it] = selTracks[it]->eta();
						jets_.ipDxy[jets_.nIP + it] = selTracks[it]->dxy(tagInfoIP.primaryVertex()->position());
						jets_.ipDz[jets_.nIP + it] = selTracks[it]->dz(tagInfoIP.primaryVertex()->position());
						jets_.ipChi2[jets_.nIP + it] = selTracks[it]->normalizedChi2();
						jets_.ipNHit[jets_.nIP + it] = selTracks[it]->numberOfValidHits();
						jets_.ipNHitPixel[jets_.nIP + it] = selTracks[it]->hitPattern().numberOfValidPixelHits();
						jets_.ipNHitStrip[jets_.nIP + it] = selTracks[it]->hitPattern().numberOfValidStripHits();
						jets_.ipIsHitL1[jets_.nIP + it]  = selTracks[it]->hitPattern().hasValidHitInFirstPixelBarrel();
						jets_.ipProb0[jets_.nIP + it] = tagInfoIP.probabilities(0)[it];
						jets_.ip2d[jets_.nIP + it] = data.ip2d.value();
						jets_.ip2dSig[jets_.nIP + it] = data.ip2d.significance();
						jets_.ip3d[jets_.nIP + it] = data.ip3d.value();
						jets_.ip3dSig[jets_.nIP + it] = data.ip3d.significance();
						jets_.ipDist2Jet[jets_.nIP + it] = data.distanceToJetAxis.value();
						jets_.ipClosest2Jet[jets_.nIP + it] = (data.closestToJetAxis - pv).mag();  //decay length
					}

					jets_.nIP += jets_.nselIPtrk[jets_.nref];

				}
			}
			ith_tagged = this->TaggedJet(jet,jetTags_PosJP);
			if(ith_tagged >= 0) jets_.pdiscr_prob[jets_.nref] = (*jetTags_PosJP)[ith_tagged].second;
			ith_tagged   = this->TaggedJet(jet,jetTags_NegJP);
			if(ith_tagged >= 0) jets_.ndiscr_prob[jets_.nref] = (*jetTags_NegJP)[ith_tagged].second;

			ith_tagged = this->TaggedJet(jet,jetTags_JB);
			if(ith_tagged >= 0) jets_.discr_probb[jets_.nref]  = (*jetTags_JB)[ith_tagged].second;
			ith_tagged = this->TaggedJet(jet,jetTags_NegJB);
			if(ith_tagged >= 0) jets_.ndiscr_probb[jets_.nref] = (*jetTags_NegJB)[ith_tagged].second;
			ith_tagged = this->TaggedJet(jet,jetTags_PosJB);
			if(ith_tagged >= 0) jets_.pdiscr_probb[jets_.nref] = (*jetTags_PosJB)[ith_tagged].second;

			ith_tagged    = this->TaggedJet(jet,jetTags_TCHighEff);
			if(ith_tagged >= 0) jets_.discr_tcHighEff[jets_.nref]   = (*jetTags_TCHighEff)[ith_tagged].second;
			ith_tagged    = this->TaggedJet(jet,jetTags_TCHighPur);
			if(ith_tagged >= 0) jets_.discr_tcHighPur[jets_.nref]   = (*jetTags_TCHighPur)[ith_tagged].second;
			ith_tagged    = this->TaggedJet(jet,jetTags_NegTCHighEff);
			if(ith_tagged >= 0) jets_.ndiscr_tcHighEff[jets_.nref]   = (*jetTags_NegTCHighEff)[ith_tagged].second;
			ith_tagged    = this->TaggedJet(jet,jetTags_NegTCHighPur);
			if(ith_tagged >= 0) jets_.ndiscr_tcHighPur[jets_.nref]   = (*jetTags_NegTCHighPur)[ith_tagged].second;

			//ith_tagged = this->TaggedJet(jet,jetTags_softMu);
			//if(ith_tagged >= 0){
			//  if ( (*jetTags_softMu)[ith_tagged].second     > -100000 )
			//    jets_.discr_muByPt[jets_.nref]  = (*jetTags_softMu)[ith_tagged].second;
			//}
			//ith_tagged = this->TaggedJet(jet,jetTags_softMuneg);
			//if(ith_tagged >= 0){
			//  float SoftMN = 0;
			//  if ( (*jetTags_softMuneg)[ith_tagged].second  > -100000 )
			//    SoftMN = ((*jetTags_softMuneg)[ith_tagged].second);
			//  if ( SoftMN > 0 ) SoftMN = -SoftMN;
			//  jets_.ndiscr_muByPt[jets_.nref] = SoftMN;
			//}

			const PFCandidateCollection *pfCandidateColl = &(*pfCandidates);
			int pfMuonIndex = getPFJetMuon(jet, pfCandidateColl);


			if(pfMuonIndex >=0){
				const reco::PFCandidate muon = pfCandidateColl->at(pfMuonIndex);
				jets_.mupt[jets_.nref]    =  muon.pt();
				jets_.mueta[jets_.nref]   =  muon.eta();
				jets_.muphi[jets_.nref]   =  muon.phi();
				jets_.mue[jets_.nref]     =  muon.energy();
				jets_.mudr[jets_.nref]    =  reco::deltaR(jet,muon);
				jets_.muptrel[jets_.nref] =  getPtRel(muon, jet);
				jets_.muchg[jets_.nref]   =  muon.charge();
			}else{
				jets_.mupt[jets_.nref]    =  0.0;
				jets_.mueta[jets_.nref]   =  0.0;
				jets_.muphi[jets_.nref]   =  0.0;
				jets_.mue[jets_.nref]     =  0.0;
				jets_.mudr[jets_.nref]    =  9.9;
				jets_.muptrel[jets_.nref] =  0.0;
				jets_.muchg[jets_.nref]   = 0;
			}
		}

		if(doHiJetID_){
			// Jet ID variables

			jets_.muMax[jets_.nref] = 0;
			jets_.muSum[jets_.nref] = 0;
			jets_.muN[jets_.nref] = 0;

			jets_.eMax[jets_.nref] = 0;
			jets_.eSum[jets_.nref] = 0;
			jets_.eN[jets_.nref] = 0;

			jets_.neutralMax[jets_.nref] = 0;
			jets_.neutralSum[jets_.nref] = 0;
			jets_.neutralN[jets_.nref] = 0;

			jets_.photonMax[jets_.nref] = 0;
			jets_.photonSum[jets_.nref] = 0;
			jets_.photonN[jets_.nref] = 0;
			jets_.photonHardSum[jets_.nref] = 0;
			jets_.photonHardN[jets_.nref] = 0;

			jets_.chargedMax[jets_.nref] = 0;
			jets_.chargedSum[jets_.nref] = 0;
			jets_.chargedN[jets_.nref] = 0;
			jets_.chargedHardSum[jets_.nref] = 0;
			jets_.chargedHardN[jets_.nref] = 0;

			jets_.trackMax[jets_.nref] = 0;
			jets_.trackSum[jets_.nref] = 0;
			jets_.trackN[jets_.nref] = 0;
			jets_.trackHardSum[jets_.nref] = 0;
			jets_.trackHardN[jets_.nref] = 0;

			jets_.hcalSum[jets_.nref] = 0;
			jets_.ecalSum[jets_.nref] = 0;

			jets_.genChargedSum[jets_.nref] = 0;
			jets_.genHardSum[jets_.nref] = 0;

			jets_.signalChargedSum[jets_.nref] = 0;
			jets_.signalHardSum[jets_.nref] = 0;

			jets_.subid[jets_.nref] = -1;

			for(unsigned int icand = 0; icand < tracks->size(); ++icand){
				const reco::Track& track = (*tracks)[icand];
				if(useQuality_ ){
					bool goodtrack = track.quality(reco::TrackBase::qualityByName(trackQuality_));
					if(!goodtrack) continue;
				}

				double dr = deltaR(jet,track);
				if(dr < rParam){
					double ptcand = track.pt();
					jets_.trackSum[jets_.nref] += ptcand;
					jets_.trackN[jets_.nref] += 1;

					if(ptcand > hardPtMin_){
						jets_.trackHardSum[jets_.nref] += ptcand;
						jets_.trackHardN[jets_.nref] += 1;

					}
					if(ptcand > jets_.trackMax[jets_.nref]) jets_.trackMax[jets_.nref] = ptcand;

				}
			}

			for(unsigned int icand = 0; icand < pfCandidates->size(); ++icand){
				const reco::PFCandidate& track = (*pfCandidates)[icand];
				double dr = deltaR(jet,track);
				if(dr < rParam){
					double ptcand = track.pt();
					int pfid = track.particleId();

					switch(pfid){

						case 1:
							jets_.chargedSum[jets_.nref] += ptcand;
							jets_.chargedN[jets_.nref] += 1;
							if(ptcand > hardPtMin_){
								jets_.chargedHardSum[jets_.nref] += ptcand;
								jets_.chargedHardN[jets_.nref] += 1;
							}
							if(ptcand > jets_.chargedMax[jets_.nref]) jets_.chargedMax[jets_.nref] = ptcand;
							break;

						case 2:
							jets_.eSum[jets_.nref] += ptcand;
							jets_.eN[jets_.nref] += 1;
							if(ptcand > jets_.eMax[jets_.nref]) jets_.eMax[jets_.nref] = ptcand;
							break;

						case 3:
							jets_.muSum[jets_.nref] += ptcand;
							jets_.muN[jets_.nref] += 1;
							if(ptcand > jets_.muMax[jets_.nref]) jets_.muMax[jets_.nref] = ptcand;
							break;

						case 4:
							jets_.photonSum[jets_.nref] += ptcand;
							jets_.photonN[jets_.nref] += 1;
							if(ptcand > hardPtMin_){
								jets_.photonHardSum[jets_.nref] += ptcand;
								jets_.photonHardN[jets_.nref] += 1;
							}
							if(ptcand > jets_.photonMax[jets_.nref]) jets_.photonMax[jets_.nref] = ptcand;
							break;

						case 5:
							jets_.neutralSum[jets_.nref] += ptcand;
							jets_.neutralN[jets_.nref] += 1;
							if(ptcand > jets_.neutralMax[jets_.nref]) jets_.neutralMax[jets_.nref] = ptcand;
							break;

						default:
							break;

					}
				}
			}

			// Calorimeter fractions

			//   for(unsigned int i = 0; i < hbheHits->size(); ++i){
			// 	const HBHERecHit & hit= (*hbheHits)[i];
			// 	math::XYZPoint pos = getPosition(hit.id(),vtx);
			// 	double dr = deltaR(jet.eta(),jet.phi(),pos.eta(),pos.phi());
			// 	if(dr < rParam){
			// 	  jets_.hcalSum[jets_.nref] += getEt(pos,hit.energy());
			// 	}
			//   }

			//   for(unsigned int i = 0; i < hfHits->size(); ++i){
			// 	const HFRecHit & hit= (*hfHits)[i];
			// 	math::XYZPoint pos = getPosition(hit.id(),vtx);
			// 	double dr = deltaR(jet.eta(),jet.phi(),pos.eta(),pos.phi());
			// 	if(dr < rParam){
			// 	  jets_.hcalSum[jets_.nref] += getEt(pos,hit.energy());
			// 	}
			//   }


			//   for(unsigned int i = 0; i < ebHits->size(); ++i){
			// 	const EcalRecHit & hit= (*ebHits)[i];
			// 	math::XYZPoint pos = getPosition(hit.id(),vtx);
			// 	double dr = deltaR(jet.eta(),jet.phi(),pos.eta(),pos.phi());
			// 	if(dr < rParam){
			// 	  jets_.ecalSum[jets_.nref] += getEt(pos,hit.energy());
			// 	}
			//   }

			//   for(unsigned int i = 0; i < eeHits->size(); ++i){
			// 	const EcalRecHit & hit= (*eeHits)[i];
			// 	math::XYZPoint pos = getPosition(hit.id(),vtx);
			// 	double dr = deltaR(jet.eta(),jet.phi(),pos.eta(),pos.phi());
			// 	if(dr < rParam){
			// 	  jets_.ecalSum[jets_.nref] += getEt(pos,hit.energy());
			// 	}
			//   }

			if(doTower){
				// changing it to take things from towers
				for(unsigned int i = 0; i < towers->size(); ++i){

					const CaloTower & hit= (*towers)[i];
					double dr = deltaR(jet.eta(), jet.phi(), hit.p4(vtx).Eta(), hit.p4(vtx).Phi());
					if(dr < rParam){
						jets_.ecalSum[jets_.nref] += hit.emEt(vtx);
						jets_.hcalSum[jets_.nref] += hit.hadEt(vtx);
					}

				}
			}

		}
		// Jet ID for CaloJets


		if(doMatch_){

			// Alternative reconstruction matching (PF for calo, calo for PF)

			double drMin = 100;
			for(unsigned int imatch = 0 ; imatch < matchedjets->size(); ++imatch){
				const reco::Jet& mjet = (*matchedjets)[imatch];

				double dr = deltaR(jet,mjet);
				if(dr < drMin){
					jets_.matchedPt[jets_.nref] = mjet.pt();

					if(usePat_){
						const pat::Jet& mpatjet = (*patmatchedjets)[imatch];
						jets_.matchedRawPt[jets_.nref] = mpatjet.correctedJet("Uncorrected").pt();
						jets_.matchedPu[jets_.nref] = mpatjet.pileup();
					}
					jets_.matchedR[jets_.nref] = dr;
					drMin = dr;
				}
			}

		}
		//     if(etrk.quality(reco::TrackBase::qualityByName(qualityString_))) pev_.trkQual[pev_.nTrk]=1;

		if(doHiJetID_){

			/////////////////////////////////////////////////////////////////
			// Jet core pt^2 discriminant for fake jets
			// Edited by Yue Shi Lai <ylai@mit.edu>

			// Initial value is 0
			jets_.discr_fr01[jets_.nref] = 0;
			// Start with no directional adaption, i.e. the fake rejection
			// axis is the jet axis
			float pseudorapidity_adapt = jets_.jteta[jets_.nref];
			float azimuth_adapt = jets_.jtphi[jets_.nref];

			// Unadapted discriminant with adaption search
			for (size_t iteration = 0; iteration < 2; iteration++) {
				float pseudorapidity_adapt_new = pseudorapidity_adapt;
				float azimuth_adapt_new = azimuth_adapt;
				float max_weighted_perp = 0;
				float perp_square_sum = 0;

				for (size_t index_pf_candidate = 0;
						index_pf_candidate < pfCandidates->size();
						index_pf_candidate++) {
					const reco::PFCandidate &p =
						(*pfCandidates)[index_pf_candidate];

					switch (p.particleId()) {
						//case 1:	// Charged hadron
						//case 3:	// Muon
						case 4:	// Photon
							{
								const float dpseudorapidity =
									p.eta() - pseudorapidity_adapt;
								const float dazimuth =
									reco::deltaPhi(p.phi(), azimuth_adapt);
								// The Gaussian scale factor is 0.5 / (0.1 * 0.1)
								// = 50
								const float angular_weight =
									exp(-50.0F * (dpseudorapidity * dpseudorapidity +
												dazimuth * dazimuth));
								const float weighted_perp =
									angular_weight * p.pt() * p.pt();
								const float weighted_perp_square =
									weighted_perp * p.pt();

								perp_square_sum += weighted_perp_square;
								if (weighted_perp >= max_weighted_perp) {
									pseudorapidity_adapt_new = p.eta();
									azimuth_adapt_new = p.phi();
									max_weighted_perp = weighted_perp;
								}
							}
						default:
							break;
					}
				}
				// Update the fake rejection value
				jets_.discr_fr01[jets_.nref] = std::max(
						jets_.discr_fr01[jets_.nref], perp_square_sum);
				// Update the directional adaption
				pseudorapidity_adapt = pseudorapidity_adapt_new;
				azimuth_adapt = azimuth_adapt_new;
			}
		}
		jets_.jtpt[jets_.nref] = jet.pt();
		jets_.jtenergy[jets_.nref] = jet.energy();
		jets_.jteta[jets_.nref] = jet.eta();
		jets_.jtphi[jets_.nref] = jet.phi();
		jets_.jty[jets_.nref] = jet.eta();
		jets_.jtpu[jets_.nref] = jet.pileup();
		jets_.jtm[jets_.nref] = jet.mass();

		if(usePat_){

			if(doStandardJetID_){
				jets_.fHPD[jets_.nref] = (*patjets)[j].jetID().fHPD;
				jets_.fRBX[jets_.nref] = (*patjets)[j].jetID().fRBX;
				jets_.n90[jets_.nref] = (*patjets)[j].n90();

				jets_.fSubDet1[jets_.nref] = (*patjets)[j].jetID().fSubDetector1;
				jets_.fSubDet2[jets_.nref] = (*patjets)[j].jetID().fSubDetector2;
				jets_.fSubDet3[jets_.nref] = (*patjets)[j].jetID().fSubDetector3;
				jets_.fSubDet4[jets_.nref] = (*patjets)[j].jetID().fSubDetector4;
				jets_.restrictedEMF[jets_.nref] = (*patjets)[j].jetID().restrictedEMF;
				jets_.nHCAL[jets_.nref] = (*patjets)[j].jetID().nHCALTowers;
				jets_.nECAL[jets_.nref] = (*patjets)[j].jetID().nECALTowers;
				jets_.apprHPD[jets_.nref] = (*patjets)[j].jetID().approximatefHPD;
				jets_.apprRBX[jets_.nref] = (*patjets)[j].jetID().approximatefRBX;

				//       jets_.n90[jets_.nref] = (*patjets)[j].jetID().hitsInN90;
				jets_.n2RPC[jets_.nref] = (*patjets)[j].jetID().numberOfHits2RPC;
				jets_.n3RPC[jets_.nref] = (*patjets)[j].jetID().numberOfHits3RPC;
				jets_.nRPC[jets_.nref] = (*patjets)[j].jetID().numberOfHitsRPC;

				jets_.fEB[jets_.nref] = (*patjets)[j].jetID().fEB;
				jets_.fEE[jets_.nref] = (*patjets)[j].jetID().fEE;
				jets_.fHB[jets_.nref] = (*patjets)[j].jetID().fHB;
				jets_.fHE[jets_.nref] = (*patjets)[j].jetID().fHE;
				jets_.fHO[jets_.nref] = (*patjets)[j].jetID().fHO;
				jets_.fLong[jets_.nref] = (*patjets)[j].jetID().fLong;
				jets_.fShort[jets_.nref] = (*patjets)[j].jetID().fShort;
				jets_.fLS[jets_.nref] = (*patjets)[j].jetID().fLS;
				jets_.fHFOOT[jets_.nref] = (*patjets)[j].jetID().fHFOOT;
			}

		}

		if(isMC_){

			for(UInt_t i = 0; i < genparts->size(); ++i){
				const reco::GenParticle& p = (*genparts)[i];
				if (p.status()!=1) continue;
				if (p.charge()==0) continue;
				double dr = deltaR(jet,p);
				if(dr < rParam){
					double ppt = p.pt();
					jets_.genChargedSum[jets_.nref] += ppt;
					if(ppt > hardPtMin_) jets_.genHardSum[jets_.nref] += ppt;
					if(p.collisionId() == 0){
						jets_.signalChargedSum[jets_.nref] += ppt;
						if(ppt > hardPtMin_) jets_.signalHardSum[jets_.nref] += ppt;
					}

				}
			}

		}

		if(isMC_ && usePat_){


			const reco::GenJet * genjet = (*patjets)[j].genJet();

			if(genjet){
				jets_.refpt[jets_.nref] = genjet->pt();
				jets_.refeta[jets_.nref] = genjet->eta();
				jets_.refphi[jets_.nref] = genjet->phi();
				jets_.refy[jets_.nref] = genjet->eta();
				jets_.refdphijt[jets_.nref] = reco::deltaPhi(jet.phi(), genjet->phi());
				jets_.refdrjt[jets_.nref] = reco::deltaR(jet.eta(),jet.phi(),genjet->eta(),genjet->phi());

				if(doSubEvent_){
					const GenParticle* gencon = genjet->getGenConstituent(0);
					jets_.subid[jets_.nref] = gencon->collisionId();
				}

			}else{
				jets_.refpt[jets_.nref] = -999.;
				jets_.refeta[jets_.nref] = -999.;
				jets_.refphi[jets_.nref] = -999.;
				jets_.refy[jets_.nref] = -999.;
				jets_.refdphijt[jets_.nref] = -999.;
				jets_.refdrjt[jets_.nref] = -999.;
			}
			jets_.refparton_flavorForB[jets_.nref] = (*patjets)[j].partonFlavour();


			// matched partons
			const reco::GenParticle & parton = *(*patjets)[j].genParton();


			/////////////////////////////////
			// jet production_channel tag  //
			/////////////////////////////////

			// set default value for no match.
			jets_.refjet_prodChannel[jets_.nref] = -1;
			jets_.ref_q_Twinfound[jets_.nref]=-1;
			jets_.ref_qfJet_dR[jets_.nref]=-999;
			jets_.ref_qfJet_dpt[jets_.nref]=-999;
			jets_.ref_q_pt[jets_.nref]=-999;
			jets_.ref_q_eta[jets_.nref]=-999;
			jets_.ref_q_phi[jets_.nref]=-999;
			jets_.ref_qTwin_pt[jets_.nref]=-999;
			jets_.ref_qTwin_eta[jets_.nref]=-999;
			jets_.ref_qTwin_phi[jets_.nref]=-999;
			jets_.ref_qTwin_dphi[jets_.nref]=-999;
			jets_.ref_qTwin_dtheta[jets_.nref]=-999;
			jets_.ref_qMom_pt[jets_.nref]=-999;
			jets_.ref_qMom_eta[jets_.nref]=-999;
			jets_.ref_qMom_phi[jets_.nref]=-999;


			myfile<<" iEvent.id().event() = "<<iEvent.id().event()<<" , jet index : j = "<<j<<"  , jets_.nref= "<<jets_.nref <<endl;
			if( abs( (*patjets)[j].partonFlavour() )==4 || abs( (*patjets)[j].partonFlavour() )==5 ){

				double Jetcone_dR=0.4;
				double Gen_q_pt=0;
				size_t Gen_q_index=0;
				int nMatch=0;


				Handle<reco::GenParticleCollection> genParticles;
				iEvent.getByToken(genParticleSrc_, genParticles); 
				for( size_t i = 0; i < genParticles->size(); ++ i ) {
					const GenParticle & genCand = (*genParticles)[i];  

            double d_eta=abs( (*jets)[j].eta() - (*genParticles)[i].eta() );
            double d_phi=abs( (*jets)[j].phi() - (*genParticles)[i].phi() );
            if (d_phi>TMath::Pi()) {d_phi-=TMath::TwoPi();}
            double delta_R= sqrt( pow(d_eta, 2.0)+pow(d_phi, 2.0) );

					if ( (*genParticles)[i].status()==71 && ( abs( (*genParticles)[i].pdgId() )==4 || abs( (*genParticles)[i].pdgId() )==5 ) ){
					h_qJet_dR_all -> Fill(delta_R); // plot dR for b,c quark with patjet.flavor =4 or 5
					}
					if ( (*genParticles)[i].status()==71 && (*genParticles)[i].pdgId()==(*patjets)[j].partonFlavour() ){ // matched flavor -> matching dR 

						myfile<<"\ndelta_R = "<<delta_R<<", (*genParticles)[i].status() = "<<(*genParticles)[i].status()<<endl;
						h_qJet_dR_flavormatched -> Fill(delta_R);

						if ( delta_R<Jetcone_dR ){ // save as candidate and search for best match.
							nMatch++;
							if ( (*genParticles)[i].pt() > Gen_q_pt ) {
								jets_.ref_qfJet_dR[jets_.nref] = delta_R;
								jets_.ref_qfJet_dpt[jets_.nref] = (*jets)[j].pt() - (*genParticles)[i].pt();
								Gen_q_pt = (*genParticles)[i].pt();
								Gen_q_index = i ;
								myfile<<"Gen_q_pt = "<<Gen_q_pt<<" , Gen_q_index = "<<Gen_q_index<<" ,Gen_q_pdgId = "<<(*genParticles)[i].pdgId()<<endl;
								myfile<<"jet pt = "<<(*jets)[j].pt()<<" , jet eta = "<< (*jets)[j].eta() <<" , jet phi = "<<(*jets)[j].phi()<<endl;
								myfile<<"Gen_q pt = "<<(*genParticles)[i].pt()<<" , Gen_q eta = "<< (*genParticles)[i].eta() <<" , Gen_q phi = "<<(*genParticles)[i].phi()<<endl;
							}
						}
						myfile<<"\tnMatch = "<<nMatch<<endl;

					}	


				} // end     for( size_t i = 0; i < genParticles->size(); ++ i ) 
				if (nMatch>0){ // using the matched candidate to trace history


					int Quark21_tag=0; // for flavor excitation
					int Quark23_tag=0; // if Quark21_tag=0, Quark23!=0, gg->bb/cc or qq->bb/cc
					int DMotherGluon21_tag=0; // gg->bb/cc
					int DMotherQuark21_tag=0; // flavor must be different, search for qq->bb/cc

					int Quark31_tag=0; // for flavor excitation in sub hard process
					int Quark33_tag=0; // for gluon excitation in sub hard process
					int DMotherGluon31_tag=0; // gg->bb/cc
					int DMotherQuark31_tag=0; // flavor must be different, search for qq->bb/cc

					int DMotherGluon41_tag=0; // initial shower type 1 if Quark21_tag==0 , could be similar to flavor excitation
					int DMotherGluon43_tag=0; // initial shower type 2 , purely outgoing. 

					int MotherGluon23_tag=0; // tag as gluon splitting process , need to trace more back
					int MotherGluon33_tag=0;  // gluon splitting type 2 , from subhardest process.
					int MotherGluon41n43_tag=0; // another initial shower-----initial shower type 3, shoud not exit!

					int Quark41_tag=0; // initail state radiation but dauguter participate the hard process
					int Quark43_tag=0; // pure initial state radiation 
					
					int MotherAllOthers23_tag=0; // if Mothergluon is none of above status, trace its mother particle, 23 and 33 gluon splitting.
					int MotherAllOthers33_tag=0; //
					int MotherAllOthers41n43_tag=0; // 4143 as initial state radiation
					int MotherBeamParticleProton_tag=0;

					myfile<<"Hello, start to trace history, Gen_q_index = "<< Gen_q_index<<" , jet index : j = "<<j<<endl;
					myfile<<"jet ID = "<< (*genParticles)[Gen_q_index].pdgId()<<endl;
					myfile<<"(*genParticles)[Gen_q_index].status() = "<<(*genParticles)[Gen_q_index].status()<<" , # of mothers = "<<(*genParticles)[Gen_q_index].numberOfMothers()<<endl;
					int n_motherOfmatch=(*genParticles)[Gen_q_index].numberOfMothers();
					if ( n_motherOfmatch >= 1){
						int mother_layer=0;
						mother_layer++;
						const reco::Candidate * GenMomCandidate = (*genParticles)[Gen_q_index].mother(); 
						if ( n_motherOfmatch >= 2){
							myfile<<"n_motherOfmatch>1 , n_motherOfmatch = "<<n_motherOfmatch<<endl;
							int flavorMatched = 0;
							for (int i_mother = 0; i_mother < n_motherOfmatch ;++ i_mother){
								const reco::Candidate * mother_candidate = (*genParticles)[Gen_q_index].mother(i_mother);
								int mother_ID = mother_candidate->pdgId();
								int mother_status = mother_candidate->status();
								myfile<<"mother_ID = "<< mother_ID << ", mother_status = "<<mother_status<<endl;
								if (mother_ID == (*genParticles)[Gen_q_index].pdgId() ) {// currently assuming only one mother will have the same type, consider more and choose best candidate later 
									GenMomCandidate = (*genParticles)[Gen_q_index].mother(i_mother);
									flavorMatched++;
									myfile<<"i_mother"<< i_mother <<" has the same pdgid"<<endl;
								}

							} // end for (int i_mother = 0; i_mother < n_mother ;++ i_mother)
							if (flavorMatched == 0){myfile<<"flavorMatche=0, status 71 quark has no same pdgID mother!!!!!"<<endl;}
							myfile<<"flavorMatched number for a 71quark's mother ="<<flavorMatched<<endl;
						} // end if ( n_motherOfmatch >= 2)
						myfile<<"mother 1 status = "<<GenMomCandidate->status()<<endl;

						while ( GenMomCandidate->numberOfMothers() >= 1){
							mother_layer++;
							if(mother_layer>9999){
								myfile<<"warning !!! mother_layer>9999, some problemsin tracing mother history, terminate tracing now"<<endl;
								break;
							}

							if( GenMomCandidate->status() ==21){ Quark21_tag++; } // for flavor excitation
							if( GenMomCandidate->status() ==23){ Quark23_tag++; } // if Quark21_tag=0, Quark23!=0, gg->bb/cc or qq->bb/c
							if( GenMomCandidate->status() ==31){ Quark31_tag++; } // flavor excitation 2nd type
							if( GenMomCandidate->status() ==33){ Quark33_tag++; } // if Quark21_tag=0, Quark33!=0, gg->bb/cc or qq->bb/c
							if( GenMomCandidate->status() ==41){ Quark41_tag++; } // if >1 ,initail state radiation- flavor excitaion like
							if( GenMomCandidate->status() ==43){ Quark43_tag++; } // if >1 and 41tag=0, pure initail state radiation.


							myfile<<" GenMomCandidate->numberOfMothers() = " << GenMomCandidate->numberOfMothers()<<endl;	
							int mother_index=0;
							int flavorMatched = 0;
							int n_mother = GenMomCandidate->numberOfMothers() ; 
							for (int i_mother =0; i_mother<n_mother; i_mother++){
								int mother_ID = GenMomCandidate->mother(i_mother)->pdgId();
								int mother_status = GenMomCandidate->mother(i_mother)->status();
								myfile<<"mother "<<i_mother<<" 's ID = "<< mother_ID<<", status = "<<mother_status<<endl;
								if (mother_ID == GenMomCandidate->pdgId() ){
									mother_index=i_mother;
									flavorMatched++;}
							} // end for (int i_mother =0; i_mother<n_mother; i_mother++)		
							if (flavorMatched >= 1){
								GenMomCandidate = GenMomCandidate->mother(mother_index);
							}
							if (flavorMatched >= 2){ myfile<<"Warning !!! flavorMatched >2 in mother match, must find the best match"<<endl;}

							if (flavorMatched ==0){ // b or c first generation is here , check their mother

								jets_.ref_q_pt[jets_.nref]=GenMomCandidate->pt();
                jets_.ref_q_eta[jets_.nref]=GenMomCandidate->eta();
                jets_.ref_q_phi[jets_.nref]=GenMomCandidate->phi();

								myfile<<"\nNo flavorMatched, b or c quark created here"<<endl;
								myfile<<"number of first generation b quark's mother is "<< GenMomCandidate->numberOfMothers()<<endl;
								int n_mother = GenMomCandidate->numberOfMothers();
								for (int i_mother =0; i_mother<n_mother; i_mother++){
									int mother_ID = GenMomCandidate->mother(i_mother)->pdgId();
									int mother_status = GenMomCandidate->mother(i_mother)->status();
									if ( mother_ID == 21 && mother_status == 21){DMotherGluon21_tag++; }
                  else if ( (abs(mother_ID) == 1 || abs(mother_ID) == 2 || abs(mother_ID) == 3 || abs(mother_ID) == 4 || abs(mother_ID) == 5) && mother_status ==21 && mother_ID != GenMomCandidate->pdgId() ){DMotherQuark21_tag++; } // qq->q'q', q!=q'
									else if ( mother_ID == 21 && mother_status == 31){DMotherGluon31_tag++; }
                  else if ( (abs(mother_ID) == 1 || abs(mother_ID) == 2 || abs(mother_ID) == 3 || abs(mother_ID) == 4 || abs(mother_ID) == 5) && mother_status ==31 && mother_ID != GenMomCandidate->pdgId() ){DMotherQuark31_tag++; }


									else if ( mother_ID == 21 && mother_status == 41){DMotherGluon41_tag++; }
									else if ( mother_ID == 21 && mother_status == 43){DMotherGluon43_tag++; }
									else if ( mother_ID == 21 && mother_status == 23){ MotherGluon23_tag++; }
									else if ( mother_ID == 21 && mother_status == 33){ MotherGluon33_tag++; }
//									else if ( mother_ID == 21 && mother_status == 61){ MotherGluon61_tag++; }
								}
								// read and calculate dphi between qq_pair here
								// using first mother to search twin sister. if not found, print out error message.
								const reco::Candidate * FirstMomCandidate=GenMomCandidate->mother();

                jets_.ref_qMom_pt[jets_.nref]=FirstMomCandidate->pt();
                jets_.ref_qMom_eta[jets_.nref]=FirstMomCandidate->eta();
                jets_.ref_qMom_phi[jets_.nref]=FirstMomCandidate->phi();

								int n_daughtersOfMom  = FirstMomCandidate->numberOfDaughters();
								//		const reco::Candidate * TwinCandidate= FirstMomCandidate->daughter(0);
								int twinfound=0;
								int twin_index=0;
								for (int i_dau = 0 ; i_dau<n_daughtersOfMom ; i_dau++){
									if (FirstMomCandidate->daughter(i_dau)->pdgId() == GenMomCandidate->pdgId()*-1 ){
										twinfound++;
										twin_index=i_dau;
									}
								} //end for (int i_dau = 0 ; i_dau<n_daughtersOfMom ; i_dau++)
								jets_.ref_q_Twinfound[jets_.nref]=twinfound;
								if (twinfound>=1){ // calculate dphi
									const reco::Candidate * TwinCandidate= FirstMomCandidate->daughter(twin_index);
									myfile<<"TwinCandidate pt"<<TwinCandidate->pt()<<endl;

                jets_.ref_qTwin_pt[jets_.nref]=TwinCandidate->pt();
                jets_.ref_qTwin_eta[jets_.nref]=TwinCandidate->eta();
                jets_.ref_qTwin_phi[jets_.nref]=TwinCandidate->phi();
								jets_.ref_qTwin_dphi[jets_.nref]=reco::deltaPhi(TwinCandidate->phi() , GenMomCandidate->phi() );
								jets_.ref_qTwin_dtheta[jets_.nref]= TMath::ACos( ( TwinCandidate->px()*GenMomCandidate->px()+TwinCandidate->py()*GenMomCandidate->py()+TwinCandidate->pz()*GenMomCandidate->pz() )	/ (TwinCandidate->p()*GenMomCandidate->p()) );

									if (twinfound>=2){myfile<<"more than one twin found, need to handle this case"<<endl;} // try add one twin_found status to record
								}
								break; // once found the first generation of heavy quark pair, break. for gluon splitting case, trace later.
							} 
						} // end while ( GenMomCandidate->numberOfMothers() >=1 )

						int traceFurther=0;
						while (Quark21_tag==0 && Quark23_tag==0 && Quark31_tag==0 && Quark33_tag==0 && Quark41_tag == 0 && DMotherGluon21_tag==0 && DMotherQuark21_tag==0&& DMotherGluon31_tag==0 && DMotherQuark31_tag==0  && DMotherGluon41_tag==0 && DMotherGluon43_tag==0 && MotherGluon23_tag==0 && MotherGluon33_tag==0 && MotherGluon41n43_tag ==0 && GenMomCandidate->numberOfMothers()>=1){
							mother_layer++;
							int n_mother = GenMomCandidate->numberOfMothers();
							int motherisgluon=0;
              myfile<<"GenMomCandidate pdgId = "<<GenMomCandidate->pdgId()<<" , GenMomCandidate->numberOfMothers() = " << GenMomCandidate->numberOfMothers()<<endl;

							for (int i_mother =0; i_mother<n_mother; i_mother++){

								int mother_ID = GenMomCandidate->mother(i_mother)->pdgId();
								int mother_status = GenMomCandidate->mother(i_mother)->status();
                myfile<<"mother "<<i_mother<<" 's ID = "<< mother_ID<<", status = "<<mother_status<<endl;

								if ( mother_ID == 21 && mother_status == 23){ MotherGluon23_tag++; break;}
								else if (  mother_ID == 21 && mother_status == 33){ MotherGluon33_tag++; break;}
								else if ( mother_ID == 21 && ( mother_status == 41 || mother_status == 43) ){ MotherGluon41n43_tag++; break;}
								if ( mother_ID ==21) {
									GenMomCandidate = GenMomCandidate->mother(i_mother);
									motherisgluon++;
									break;
								}
							}
							if (motherisgluon==0){ // for Mothergluon != 23,33,41n43
								traceFurther=1;
								myfile<<"quark mother is not quark and not gluon ,handle this case"<<endl; 
								break;}	
							if (mother_layer>9999){myfile<<"can not trace to origin, some problems here"<<endl; break;}
						} // end of while (Quark...) : mother gluon search

	
							int flavorchanged =0;
							int numberOfMother_traceFurther=0;
						while (traceFurther==1 && MotherAllOthers23_tag ==0 && MotherAllOthers33_tag==0 && MotherAllOthers41n43_tag==0 && GenMomCandidate->numberOfMothers()>=1 ){
							myfile<<"inside the traceFurther while loop"<<endl;
							int flavormatched=0;
              int n_mother = GenMomCandidate->numberOfMothers();
							mother_layer++;
							if (n_mother>1){
								myfile<<"n_mother >1 in traceFurther while loop, be careful!"<<endl;
								numberOfMother_traceFurther++;
							}
              for (int i_mother =0; i_mother<n_mother; i_mother++){
                int mother_ID = GenMomCandidate->mother(i_mother)->pdgId();
                int mother_status = GenMomCandidate->mother(i_mother)->status();
                myfile<<"mother "<<i_mother<<" 's ID = "<< mother_ID<<", status = "<<mother_status<<endl;				
								if (GenMomCandidate->pdgId() == mother_ID){
									flavormatched++;
									GenMomCandidate = GenMomCandidate->mother(i_mother);
									break;	
								}
							} // enf for( i_mother)
							if (flavormatched==0){
								flavorchanged++;
								GenMomCandidate = GenMomCandidate->mother();
                myfile<<"flavorchanged , new flavor is "<<GenMomCandidate->pdgId();
							}
						
							if (GenMomCandidate->status()== 23) {MotherAllOthers23_tag++;} // the following should auto exit the while
              if (GenMomCandidate->status()== 33) {MotherAllOthers33_tag++;}
              if (GenMomCandidate->status()== 41 || GenMomCandidate->status()== 43 ) {MotherAllOthers41n43_tag++;}
							if (GenMomCandidate->pdgId()==2212){
								MotherBeamParticleProton_tag++;
								myfile<<"reach origin"<<endl; 							
								break;
								}					
              if (mother_layer>9999){myfile<<"can not trace to origin, some problems here"<<endl; break;}

						} // end of while traceFurther
						if(flavorchanged >=1){myfile<<"flavor changed times = "<<flavorchanged<<endl;}
						if(numberOfMother_traceFurther >=1){myfile<<"numberOfMother in traceFurther = "<<numberOfMother_traceFurther<<endl;}

						myfile<<"mother layer = "<< mother_layer<<" ,status = "<<GenMomCandidate->status()<<", pdgId = "<< GenMomCandidate->pdgId()<<endl;



						myfile<<"mother_layer"<<mother_layer<<endl;
					} // end if if ( n_motherOfmatch >= 1)

					myfile <<"event"<<nth_event<<endl;
					myfile<<"####  Quark21_tag = " <<Quark21_tag<<" ,Quark23_tag = "<<Quark23_tag<<" ,DMotherGluon21_tag = "<<DMotherGluon21_tag<<" ,DMotherQuark21_tag = "<<DMotherQuark21_tag<<endl;
					myfile<<"#### Quark31_tag = " <<Quark31_tag<<" ,Quark33_tag = "<<Quark33_tag<<" ,DMotherGluon31_tag = "<<DMotherGluon31_tag<<" ,DMotherQuark31_tag = "<<DMotherQuark31_tag<<endl;
					myfile<<"####  DMotherGluon41_tag = "<<DMotherGluon41_tag<<" ,DMotherGluon43_tag = "<<DMotherGluon43_tag<<" ,MotherGluon23_tag ="<<MotherGluon23_tag<<" ,MotherGluon33_tag ="<<MotherGluon33_tag<<" ,MotherGluon41_tag = "<<MotherGluon41n43_tag<<endl;
					myfile<<"####  MotherAllOthers23_tag = "<<MotherAllOthers23_tag<<" ,MotherAllOthers33_tag = "<< MotherAllOthers33_tag<<" ,MotherAllOthers41n43_tag = "<<MotherAllOthers41n43_tag<<" , MotherBeamParticleProton_tag = "<< MotherBeamParticleProton_tag<<endl;
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


          if (jetProduction_channel == 0){ myfile<<"####  production_channel =0, check this #####"<<endl;}
					myfile<<"jet Production channel tag  = "<< jetProduction_channel<<"\n\n"<<endl;

					jets_.refjet_prodChannel[jets_.nref]=jetProduction_channel;

				} // end if (nmatch>0)

			} // end   if( abs( (*patjets)[j].partonFlavour() )==4 || abs( (*patjets)[j].partonFlavour() )==5 )



			if((*patjets)[j].genParton()){
				jets_.refparton_pt[jets_.nref] = parton.pt();
				jets_.refparton_flavor[jets_.nref] = parton.pdgId();

				if(saveBfragments_ && abs(jets_.refparton_flavorForB[jets_.nref])==5){

					usedStringPts.clear();

					// uncomment this if you want to know the ugly truth about parton matching -matt
					//if(jet.pt() > 50 &&abs(parton.pdgId())!=5 && parton.pdgId()!=21)
					// cout<<" Identified as a b, but doesn't match b or gluon, id = "<<parton.pdgId()<<endl;

					jets_.bJetIndex[jets_.bMult] = jets_.nref;
					jets_.bStatus[jets_.bMult] = parton.status();
					jets_.bVx[jets_.bMult] = parton.vx();
					jets_.bVy[jets_.bMult] = parton.vy();
					jets_.bVz[jets_.bMult] = parton.vz();
					jets_.bPt[jets_.bMult] = parton.pt();
					jets_.bEta[jets_.bMult] = parton.eta();
					jets_.bPhi[jets_.bMult] = parton.phi();
					jets_.bPdg[jets_.bMult] = parton.pdgId();
					jets_.bChg[jets_.bMult] = parton.charge();
					jets_.bMult++;
					saveDaughters(parton);
				}


			} else {
				jets_.refparton_pt[jets_.nref] = -999;
				jets_.refparton_flavor[jets_.nref] = -999;
			}


		} // end  if(isMC_ && usePat_)

		jets_.nref++; // notice jets_nref value change here 


	} // end    for(unsigned int j = 0; j < jets->size(); ++j)



	if(isMC_){

		if(useHepMC_) {
			edm::Handle<HepMCProduct> hepMCProduct;
			iEvent.getByToken(eventInfoTag_,hepMCProduct);
			const HepMC::GenEvent* MCEvt = hepMCProduct->GetEvent();

			std::pair<HepMC::GenParticle*,HepMC::GenParticle*> beamParticles = MCEvt->beam_particles();
			if(beamParticles.first != 0)jets_.beamId1 = beamParticles.first->pdg_id();
			if(beamParticles.second != 0)jets_.beamId2 = beamParticles.second->pdg_id();
		}

		edm::Handle<GenEventInfoProduct> hEventInfo;
		iEvent.getByToken(eventGenInfoTag_,hEventInfo);
		//jets_.pthat = hEventInfo->binningValues()[0];

		// binning values and qscale appear to be equivalent, but binning values not always present
		jets_.pthat = hEventInfo->qScale();

		edm::Handle<vector<reco::GenJet> >genjets;
		iEvent.getByToken(genjetTag_, genjets);

		jets_.ngen = 0;
		for(unsigned int igen = 0 ; igen < genjets->size(); ++igen){
			const reco::GenJet & genjet = (*genjets)[igen];

			float genjet_pt = genjet.pt();

			// threshold to reduce size of output in minbias PbPb
			if(genjet_pt>genPtMin_){


				jets_.genpt[jets_.ngen] = genjet_pt;
				jets_.geneta[jets_.ngen] = genjet.eta();
				jets_.genphi[jets_.ngen] = genjet.phi();
				jets_.geny[jets_.ngen] = genjet.eta();

				if(doSubEvent_){
					const GenParticle* gencon = genjet.getGenConstituent(0);
					jets_.gensubid[jets_.ngen] = gencon->collisionId();
				}

				// find matching patJet if there is one

				jets_.gendrjt[jets_.ngen] = -1.0;
				jets_.genmatchindex[jets_.ngen] = -1;

				for(int ijet = 0 ; ijet < jets_.nref; ++ijet){
					// poor man's matching, someone fix please
					if(fabs(genjet.pt()-jets_.refpt[ijet])<0.00001 &&
							fabs(genjet.eta()-jets_.refeta[ijet])<0.00001){

						jets_.genmatchindex[jets_.ngen] = (int)ijet;
						jets_.gendphijt[jets_.ngen] = reco::deltaPhi(jets_.refphi[ijet],genjet.phi());
						jets_.gendrjt[jets_.ngen] = sqrt(pow(jets_.gendphijt[jets_.ngen],2)+pow(fabs(genjet.eta()-jets_.refeta[ijet]),2));

						break;
					}
				}
				jets_.ngen++;
			}
		}

	}





	t->Fill();
	memset(&jets_,0,sizeof jets_);

}




//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::fillL1Bits(const edm::Event &iEvent)
{
	edm::Handle< L1GlobalTriggerReadoutRecord >  L1GlobalTrigger;

	iEvent.getByToken(L1gtReadout_, L1GlobalTrigger);
	const TechnicalTriggerWord&  technicalTriggerWordBeforeMask = L1GlobalTrigger->technicalTriggerWord();

	for (int i=0; i<64;i++)
	{
		jets_.l1TBit[i] = technicalTriggerWordBeforeMask.at(i);
	}
	jets_.nL1TBit = 64;

	int ntrigs = L1GlobalTrigger->decisionWord().size();
	jets_.nL1ABit = ntrigs;

	for (int i=0; i != ntrigs; i++) {
		bool accept = L1GlobalTrigger->decisionWord()[i];
		//jets_.l1ABit[i] = (accept == true)? 1:0;
		if(accept== true){
			jets_.l1ABit[i] = 1;
		}
		else{
			jets_.l1ABit[i] = 0;
		}

	}
}

//--------------------------------------------------------------------------------------------------
void HiInclusiveJetAnalyzer::fillHLTBits(const edm::Event &iEvent)
{
	// Fill HLT trigger bits.
	Handle<TriggerResults> triggerResultsHLT;
	iEvent.getByToken(hltResName_, triggerResultsHLT);

	const TriggerResults *hltResults = triggerResultsHLT.product();
	const TriggerNames & triggerNames = iEvent.triggerNames(*hltResults);

	jets_.nHLTBit = hltTrgNames_.size();

	for(size_t i=0;i<hltTrgNames_.size();i++){

		for(size_t j=0;j<triggerNames.size();++j) {

			if(triggerNames.triggerName(j) == hltTrgNames_[i]){

				//cout <<"hltTrgNames_(i) "<<hltTrgNames_[i]<<endl;
				//cout <<"triggerName(j) "<<triggerNames.triggerName(j)<<endl;
				//cout<<" result "<<triggerResultsHLT->accept(j)<<endl;
				jets_.hltBit[i] = triggerResultsHLT->accept(j);
			}

		}
	}
}

	int
HiInclusiveJetAnalyzer::getPFJetMuon(const pat::Jet& pfJet, const reco::PFCandidateCollection *pfCandidateColl)
{

	int pfMuonIndex = -1;
	float ptMax = 0.;


	for(unsigned icand=0;icand<pfCandidateColl->size(); icand++) {
		const reco::PFCandidate pfCandidate = pfCandidateColl->at(icand);

		int id = pfCandidate.particleId();
		if(abs(id) != 3) continue;

		if(reco::deltaR(pfJet,pfCandidate)>0.5) continue;

		double pt =  pfCandidate.pt();
		if(pt>ptMax){
			ptMax = pt;
			pfMuonIndex = (int) icand;
		}
	}

	return pfMuonIndex;

}


	double
HiInclusiveJetAnalyzer::getPtRel(const reco::PFCandidate lep, const pat::Jet& jet )
{

	float lj_x = jet.p4().px();
	float lj_y = jet.p4().py();
	float lj_z = jet.p4().pz();

	// absolute values squared
	float lj2  = lj_x*lj_x+lj_y*lj_y+lj_z*lj_z;
	float lep2 = lep.px()*lep.px()+lep.py()*lep.py()+lep.pz()*lep.pz();

	// projection vec(mu) to lepjet axis
	float lepXlj = lep.px()*lj_x+lep.py()*lj_y+lep.pz()*lj_z;

	// absolute value squared and normalized
	float pLrel2 = lepXlj*lepXlj/lj2;

	// lep2 = pTrel2 + pLrel2
	float pTrel2 = lep2-pLrel2;

	return (pTrel2 > 0) ? std::sqrt(pTrel2) : 0.0;
}

// Recursive function, but this version gets called only the first time
void
HiInclusiveJetAnalyzer::saveDaughters(const reco::GenParticle &gen){

	for(unsigned i=0;i<gen.numberOfDaughters();i++){
		const reco::Candidate & daughter = *gen.daughter(i);
		double daughterPt = daughter.pt();
		if(daughterPt<1.) continue;
		double daughterEta = daughter.eta();
		if(fabs(daughterEta)>3.) continue;
		int daughterPdgId = daughter.pdgId();
		int daughterStatus = daughter.status();
		// Special case when b->b+string, both b and string contain all daughters, so only take the string
		if(gen.pdgId()==daughterPdgId && gen.status()==3 && daughterStatus==2) continue;

		// cheesy way of finding strings which were already used
		if(daughter.pdgId()==92){
			for(unsigned ist=0;ist<usedStringPts.size();ist++){
				if(fabs(daughter.pt() - usedStringPts[ist]) < 0.0001) return;
			}
			usedStringPts.push_back(daughter.pt());
		}
		jets_.bJetIndex[jets_.bMult] = jets_.nref;
		jets_.bStatus[jets_.bMult] = daughterStatus;
		jets_.bVx[jets_.bMult] = daughter.vx();
		jets_.bVy[jets_.bMult] = daughter.vy();
		jets_.bVz[jets_.bMult] = daughter.vz();
		jets_.bPt[jets_.bMult] = daughterPt;
		jets_.bEta[jets_.bMult] = daughterEta;
		jets_.bPhi[jets_.bMult] = daughter.phi();
		jets_.bPdg[jets_.bMult] = daughterPdgId;
		jets_.bChg[jets_.bMult] = daughter.charge();
		jets_.bMult++;
		saveDaughters(daughter);
	}
}

// This version called for all subsequent calls
void
HiInclusiveJetAnalyzer::saveDaughters(const reco::Candidate &gen){

	for(unsigned i=0;i<gen.numberOfDaughters();i++){
		const reco::Candidate & daughter = *gen.daughter(i);
		double daughterPt = daughter.pt();
		if(daughterPt<1.) continue;
		double daughterEta = daughter.eta();
		if(fabs(daughterEta)>3.) continue;
		int daughterPdgId = daughter.pdgId();
		int daughterStatus = daughter.status();
		// Special case when b->b+string, both b and string contain all daughters, so only take the string
		if(gen.pdgId()==daughterPdgId && gen.status()==3 && daughterStatus==2) continue;

		// cheesy way of finding strings which were already used
		if(daughter.pdgId()==92){
			for(unsigned ist=0;ist<usedStringPts.size();ist++){
				if(fabs(daughter.pt() - usedStringPts[ist]) < 0.0001) return;
			}
			usedStringPts.push_back(daughter.pt());
		}

		jets_.bJetIndex[jets_.bMult] = jets_.nref;
		jets_.bStatus[jets_.bMult] = daughterStatus;
		jets_.bVx[jets_.bMult] = daughter.vx();
		jets_.bVy[jets_.bMult] = daughter.vy();
		jets_.bVz[jets_.bMult] = daughter.vz();
		jets_.bPt[jets_.bMult] = daughterPt;
		jets_.bEta[jets_.bMult] = daughterEta;
		jets_.bPhi[jets_.bMult] = daughter.phi();
		jets_.bPdg[jets_.bMult] = daughterPdgId;
		jets_.bChg[jets_.bMult] = daughter.charge();
		jets_.bMult++;
		saveDaughters(daughter);
	}
}

double HiInclusiveJetAnalyzer::getEt(math::XYZPoint pos, double energy){
	double et = energy*sin(pos.theta());
	return et;
}

math::XYZPoint HiInclusiveJetAnalyzer::getPosition(const DetId &id, reco::Vertex::Point vtx){
	const GlobalPoint& pos=geo->getPosition(id);
	math::XYZPoint posV(pos.x() - vtx.x(),pos.y() - vtx.y(),pos.z() - vtx.z());
	return posV;
}

int HiInclusiveJetAnalyzer::TaggedJet(Jet calojet, Handle<JetTagCollection > jetTags ) {
	double small = 1.e-5;
	int result = -1;

	for (size_t t = 0; t < jetTags->size(); t++) {
		RefToBase<Jet> jet_p = (*jetTags)[t].first;
		if (jet_p.isNull()) {
			continue;
		}
		if (DeltaR<Candidate>()( calojet, *jet_p ) < small) {
			result = (int) t;
		}
	}
	return result;
}


DEFINE_FWK_MODULE(HiInclusiveJetAnalyzer);

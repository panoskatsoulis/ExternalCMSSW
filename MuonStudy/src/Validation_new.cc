// -*- C++ -*-
//
// Package:    Personal/MuonStudy
// Class:      MuonStudy
// 
/**\class MuonStudy MuonStudy.cc Personal/MuonStudy/plugins/MuonStudy.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Panagiotis Katsoulis (UOI)
//         Created:  03 Sep 2017
// T&P Eff Implem.:  Yiannis Evangelou (UOI)
//            When:  Feb to Apr 2020
//


// system include files
#include <memory>
#include <iostream>
#include <fstream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
//
// class declaration - Data Formats

#include "DataFormats/FEDRawData/interface/FEDHeader.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhContainer.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThContainer.h"
#include "DataFormats/MuonReco/interface/Muon.h"

#include "EventFilter/L1TRawToDigi/interface/AMC13Spec.h"
#include "EventFilter/L1TRawToDigi/interface/Block.h"

#include "L1Trigger/L1TMuon/interface/RegionalMuonRawDigiTranslator.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "ExternalCMSSW/MuonStudy/interface/analysisTools.h"//ie ---
#include "TLorentzVector.h"
#include <algorithm>  // vector.sort

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class Validation : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit Validation(const edm::ParameterSet&);
  ~Validation();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  void otherCalculation(const edm::Handle<L1MuDTChambPhContainer>, TH2I&);
  void printEvent(const L1MuDTChambPhContainer*, const L1MuDTChambThContainer*, 
		  const l1t::RegionalMuonCandBxCollection*, const l1t::RegionalMuonCandBxCollection*, 
		  const std::string);
  int returnWheel_2sComp(const int&, const int&);
  int returnWheel_Janos(const int&, const int&);
  int returnWheel_Michalis(const int&, const int&);

  // edm::EDGetTokenT<FEDRawDataCollection> binToken_original;
  // edm::EDGetTokenT<FEDRawDataCollection> binToken_replica;

  edm::EDGetTokenT<L1MuDTChambPhContainer> phiToken;
  edm::EDGetTokenT<L1MuDTChambThContainer> etaToken;
  edm::EDGetTokenT<l1t::RegionalMuonCandBxCollection> token_bmtfData;
  edm::EDGetTokenT<l1t::RegionalMuonCandBxCollection> token_bmtfEmu;
  edm::EDGetTokenT< std::vector<reco::Muon> > token_recoMu;

  edm::Service<TFileService> fs;
  std::ofstream fout;
  TFile *file;

  std::string system;
  bool doEfficiency;

  TH2F phisHist;//All Etas 2D
  TH2F etasHist;
  TH2F ptHist;
  TH2F pt2Hist;
  TH2I sizeHist;
  TH2I bxHist;//not yet implemented
  TH1F phisData, phisEmu;
  TH1F etasData, etasEmu;
  TH1F ptData, ptEmu;
  TH1F pt2Data, pt2Emu;
  TH1I dxyData, dxyEmu;
  TH1I TA1Data, TA1Emu;
  TH1I TA2Data, TA2Emu;
  TH1I TA3Data, TA3Emu;
  TH1I TA4Data, TA4Emu;
  TH1F wheelData, wheelEmu;
  TH1I fineBitData, fineBitEmu;
  TH1F phisRatio;//ratios
  TH1F etasRatio;
  TH1F ptRatio;
  TH1F pt2Ratio;  

  TH1F eff_all;
  TH1F eff_pass;

  TH1F phi_online_0;
  TH1F phi_online_1;
  TH1F phi_online_2;
  TH1F phi_online_3;
  TH1F eta_online_0;
  TH1F eta_online_1;
  TH1F eta_online_2;
  TH1F eta_online_3;

  TH1F pt_online_0;

  TH1F phi_offline_0;
  TH1F phi_offline_1;
  TH1F phi_offline_2;
  TH1F phi_offline_3;
  TH1F eta_offline_0;
  TH1F eta_offline_1;
  TH1F eta_offline_2;
  TH1F eta_offline_3;

  TH1I chambers_hits;
  TH1I probes_gt2;
  TH1F dr_tag;
  TH1F dr_probe;
  TH1F dimuon;

  TH1F rej_phi;
  TH1F rej_eta;
  TH1F rej_DR;

  int phiBins, etaBins, ptBins, dxyBins, sizeBins, bxBins, fBBins;
  // ----------member data ---------------------------
  // float Dev;
  // float dphis, detas, dmuons;
  // int phiSize, etaSize, muonsSize;
  int notMatched;
  int filledSame;
  int filledBySearch;
  int mismatchedContainers;
  int ptMismatches;
  int pt2Mismatches;
  int dxyMismatches;
  int phiMismatches;
  int etaMismatches;
  int TA1Mismatches;
  int TA2Mismatches;
  int TA3Mismatches;
  int TA4Mismatches;
  int wheelMismatches;
  int sizeMismatches;
  int events;

  int eff_events_2pr;
  int eff_events_pass;
  int eff_events_final;

  int emuMuonSize;
  int dataMuonSize;

  //other calculations
  //placeholder
}; //end of analyzer class definition

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
Validation::Validation(const edm::ParameterSet& iConfig)

{

  // std::string labelFEDData_1("rawDataCollector");
  // binToken_original = consumes<FEDRawDataCollection>(edm::InputTag(labelFEDData_1));  

  system = iConfig.getParameter<std::string>("system");
  doEfficiency = iConfig.getParameter<bool>("doEfficiency");
  phiToken = consumes< L1MuDTChambPhContainer >(iConfig.getParameter<edm::InputTag>("phiHits"));
  etaToken = consumes< L1MuDTChambThContainer >(iConfig.getParameter<edm::InputTag>("etaHits"));
  token_bmtfData = consumes< l1t::RegionalMuonCandBxCollection >(iConfig.getParameter<edm::InputTag>("muonsData"));
  token_bmtfEmu = consumes< l1t::RegionalMuonCandBxCollection >(iConfig.getParameter<edm::InputTag>("muonsEmu"));
  token_recoMu = consumes< std::vector<reco::Muon> >(iConfig.getParameter<edm::InputTag>("muonsReco"));

  fout.open(system+"_mismatchesFile");
  usesResource("TFileService");
  file = &(fs -> file());

  //Binning
  phiBins = 100;
  etaBins = 56; //etaBins = 56;
  ptBins = 300; //ptBins = 150;
  dxyBins = 4;
  sizeBins = 21;
  bxBins = 7;
  fBBins = 4;

  phisHist = TH2F("muonPhis","#phi(#mu) Comparison", phiBins, -20, 80, phiBins, -20, 80);
  etasHist = TH2F("muonEtas","#eta(#mu) Comparison", etaBins, -140, 140, etaBins, -140, 140);
  ptHist = TH2F("muonPt","Pt(#mu) Comparison", ptBins/3, 0, 300, ptBins/3, 0, 300);
  pt2Hist = TH2F("muonPt2","Pt_{2}(#mu) Comparison", ptBins/3, 0, 300, ptBins/3, 0, 300);
  sizeHist = TH2I("bmtfSize","##mu Comparison", sizeBins, 0, 20, sizeBins, 0, 20);
  bxHist = TH2I("muonBx","BX Comparison", bxBins, -3, 3, bxBins, -3, 3);//not yet implemented

  phisData = TH1F("phisData","#phi BMTF", phiBins, -20, 80);
  etasData = TH1F("etasData","#eta BMTF", etaBins, -140, 140);
  ptData = TH1F("ptData","Pt BMTF", ptBins, 0, 300);
  pt2Data = TH1F("pt2Data","Pt_{2} BMTF", ptBins, 0, 300);
  dxyData = TH1I("dxyData","dxy BMTF", dxyBins, 0, 4);
  fineBitData = TH1I("fineBitData","fineBit data", fBBins, -1, 2);
  TA1Data = TH1I("TA1Data","TA1 data", 16, 0, 16);
  TA2Data = TH1I("TA2Data","TA2 data", 16, 0, 16);
  TA3Data = TH1I("TA3Data","TA3 data", 16, 0, 16);
  TA4Data = TH1I("TA4Data","TA4 data", 16, 0, 16);
  wheelData = TH1F("wheelData", "wheel data", 9, -4.5, 4.5);

  phisEmu = TH1F("phisEmu","#phi BMTF", phiBins, -20, 80);
  etasEmu = TH1F("etasEmu","#eta BMTF", etaBins, -140, 140);
  ptEmu = TH1F("ptEmu","Pt BMTF", ptBins, 0, 300);
  pt2Emu = TH1F("pt2Emu","Pt_{2} BMTF", ptBins, 0, 300);
  dxyEmu = TH1I("dxyEmu","dxy BMTF", dxyBins, 0, 4);
  fineBitEmu = TH1I("fineBitEmu","fineBit emu", fBBins, -1, 2);
  TA1Emu = TH1I("TA1Emu","TA1 emu", 16, 0, 16);
  TA2Emu = TH1I("TA2Emu","TA2 emu", 16, 0, 16);
  TA3Emu = TH1I("TA3Emu","TA3 emu", 16, 0, 16);
  TA4Emu = TH1I("TA4Emu","TA4 emu", 16, 0, 16);
  wheelEmu = TH1F("wheelEmu", "wheel emu", 9, -4.5, 4.5);

  eff_all  = TH1F("eff_all", " L1 efficiency all",  30, 0., 150.);
  eff_pass = TH1F("eff_pass"," L1 efficiency pass",30, 0., 150.);
  
  phi_online_0 = TH1F("phi_online_0"," phi online all (step 1)",40, -3.14, 3.14);
  phi_online_1 = TH1F("phi_online_1"," phi online all (step 2)",40, -3.14, 3.14);
  phi_online_2 = TH1F("phi_online_2"," phi online matched tag (step 3)",40, -3.14, 3.14);
  phi_online_3 = TH1F("phi_online_3"," phi online matched probe (step 4)",40, -3.14, 3.14);

  eta_online_0 = TH1F("eta_online_0"," eta online all  (step 1)",40,-1.,1.);
  eta_online_1 = TH1F("eta_online_1"," eta online all  (step 2)",40,-1.,1.);
  eta_online_2 = TH1F("eta_online_2"," eta online matched tag (step 3)",40,-1.,1.);
  eta_online_3 = TH1F("eta_online_3"," eta online matched probe (step 4)",40,-1.,1.);

  pt_online_0 = TH1F("pt_online_0","pt online all (step 1)", 70, 0., 140.);

  phi_offline_0 = TH1F("phi_offline_0"," phi offline all probes (step 1)",40, -3.14, 3.14);
  phi_offline_1 = TH1F("phi_offline_1"," phi offline all probes (step 1)",40, -3.14, 3.14);
  phi_offline_2 = TH1F("phi_offline_2"," phi offline tag Muons (step 3)",40, -3.14, 3.14);
  phi_offline_3 = TH1F("phi_offline_3"," phi offline probe Muons (step 4)",40, -3.14, 3.14);

  eta_offline_0 = TH1F("eta_offline_0"," eta offline all probes (step 1)",40,-1.,1.);
  eta_offline_1 = TH1F("eta_offline_1"," eta offline all probes (step 1)",40,-1.,1.);
  eta_offline_2 = TH1F("eta_offline_2"," eta offline tag Muons (step 3)",40,-1.,1.);
  eta_offline_3 = TH1F("eta_offline_3"," eta offline probe Muons (step 4)",40,-1.,1.);

  chambers_hits = TH1I("chambers_hits","hits on CSC & DT",10,0,10);
  probes_gt2 = TH1I("probes_gt2"," nr of pprobs >=2",10,0,10);
  dr_tag = TH1F("dr_tag", "DR for tag mu",60,0.,0.3);
  dr_probe = TH1F("dr_probe", "DR for probe mu",60,0.,0.3);
  dimuon = TH1F("dimuon", "inv mass dimuon",100,50.,150.);

  rej_phi = TH1F("rej_phi","rejected probe phi",40, -3.14,3.14);
  rej_eta = TH1F("rej_eta","rejected probe ",40,-1.,1.);
  rej_DR = TH1F("rej_DR","rejected probe DR",50,0.,1.);

  //Other Calculations
  notMatched = 0;
  filledSame = 0;
  filledBySearch = 0;
  mismatchedContainers = 0;
  ptMismatches = 0;
  pt2Mismatches = 0;
  dxyMismatches = 0;
  phiMismatches = 0;
  etaMismatches = 0;
  sizeMismatches = 0;
  wheelMismatches = 0;
  events = 0;

  eff_events_2pr=0;
  eff_events_pass=0;
  eff_events_final=0;

  emuMuonSize = 0;
  dataMuonSize = 0;

}


Validation::~Validation()
{

  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
  void
Validation::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  //Phi Collections
  edm::Handle<L1MuDTChambPhContainer> phiInput;
  iEvent.getByToken(phiToken,phiInput);

  //Eta Collections
  edm::Handle<L1MuDTChambThContainer> etaInput;
  iEvent.getByToken(etaToken,etaInput);

  //Muons Collections
  edm::Handle<l1t::RegionalMuonCandBxCollection> bmtfDataMuons;
  iEvent.getByToken(token_bmtfData,bmtfDataMuons);

  edm::Handle<l1t::RegionalMuonCandBxCollection> bmtfEmuMuons;
  iEvent.getByToken(token_bmtfEmu,bmtfEmuMuons);

  //Reco Muons Collections
  edm::Handle< vector<reco::Muon> > recoMuons;
  if (doEfficiency)
    iEvent.getByToken(token_recoMu,recoMuons);

  //Count events
  events++;

  //====================== other calculations =========================================================


  //====================== muons Handler ==============================================================

  for (auto imuA : *bmtfDataMuons ) {
    //data 1D histograms
    phisData.Fill(imuA.hwPhi());
    etasData.Fill(imuA.hwEta());
    ptData.Fill(imuA.hwPt());
    pt2Data.Fill(imuA.hwPt2());
    dxyData.Fill(imuA.hwDXY());
    fineBitData.Fill(imuA.hwHF());
    TA1Data.Fill(imuA.trackAddress().at(2));
    TA2Data.Fill(imuA.trackAddress().at(3));
    TA3Data.Fill(imuA.trackAddress().at(4));
    TA4Data.Fill(imuA.trackAddress().at(5));
    wheelData.Fill(returnWheel_Janos(imuA.trackAddress().at(0),
          imuA.trackAddress().at(1)));
  }

  for (auto imuB : *bmtfEmuMuons ) {
    //emu 1D histograms
    phisEmu.Fill(imuB.hwPhi());
    etasEmu.Fill(imuB.hwEta());
    ptEmu.Fill(imuB.hwPt());
    pt2Emu.Fill(imuB.hwPt2());
    dxyEmu.Fill(imuB.hwDXY());
    fineBitEmu.Fill(imuB.hwHF());
    TA1Emu.Fill(imuB.trackAddress().at(2));
    TA2Emu.Fill(imuB.trackAddress().at(3));
    TA3Emu.Fill(imuB.trackAddress().at(4));
    TA4Emu.Fill(imuB.trackAddress().at(5));
    //wheelEmu.Fill(returnWheel_2sComp(imuB.trackAddress().at(0),
    //imuB.trackAddress().at(1)));
    wheelEmu.Fill(returnWheel_Janos(imuB.trackAddress().at(0),
          imuB.trackAddress().at(1)));
  }


  std::map<int, std::vector<l1t::RegionalMuonCand>> dataPerProc;
  for (auto dataMuon: *bmtfDataMuons) {
    dataPerProc[dataMuon.processor()].push_back(dataMuon);
  }
  if (dataPerProc.size() > 12)
    std::cout << "More than 12 processors reported for DATA..." << std::endl;

  std::map<int, std::vector<l1t::RegionalMuonCand>> emuPerProc;
  for (auto emuMuon: *bmtfEmuMuons) {
    emuPerProc[emuMuon.processor()].push_back(emuMuon);
  }
  if (emuPerProc.size() > 12)
    std::cout << "More than 12 processors reported for EMU..." << std::endl;


  bool mismatch = false;
  for (int proc = 0; proc <= 11; proc++) {

    auto imuA = dataPerProc[proc].begin();
    for (auto imuB = emuPerProc[proc].begin(); imuB != emuPerProc[proc].end(); imuB++) {
      if (imuA == dataPerProc[proc].end())
        break;

      phisHist.Fill(imuA->hwPhi(), imuB->hwPhi());
      etasHist.Fill(imuA->hwEta(), imuB->hwEta());
      ptHist.Fill(imuA->hwPt(), imuB->hwPt());
      pt2Hist.Fill(imuA->hwPt2(), imuB->hwPt2());

      //Check for Mismatches
      if (imuA->hwPhi() != imuB->hwPhi()) {
        phiMismatches++;
        mismatch = true;
      }
      if (imuA->hwEta() != imuB->hwEta()) {
        etaMismatches++;
        mismatch = true;
      }
      if (imuA->hwPt() != imuB->hwPt()) {
        ptMismatches++;
        mismatch = true;
      }
      if (returnWheel_Janos(imuA->trackAddress().at(0), imuA->trackAddress().at(1))
          != returnWheel_Janos(imuB->trackAddress().at(0), imuB->trackAddress().at(1)) ) {
        wheelMismatches++;
        mismatch = true;
      }
      if (system == "KMTF" && imuA->hwPt2() != imuB->hwPt2()) {
        pt2Mismatches++;
        mismatch = true;
      }
      if (system == "KMTF" && imuA->hwDXY() != imuB->hwDXY()) {
        dxyMismatches++;
        mismatch = true;
      }

      imuA++;
    }

  } //for_proc


  //Size Histogram 2D
  sizeHist.Fill((*bmtfDataMuons).size(), (*bmtfEmuMuons).size());
  emuMuonSize += (*bmtfEmuMuons).size();
  dataMuonSize += (*bmtfDataMuons).size();

  if ((*bmtfDataMuons).size() != (*bmtfEmuMuons).size()) {
    mismatchedContainers++;
    mismatch = true;
  }


  if (mismatch)
    printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "GENERAL");



  // Calculate BMTF Efficiencies -----
  if (doEfficiency) { //will be calculated ONLY if the switch 'doEfficiency' is selected (defaults into False)

    const std::vector<reco::Muon> *offMuons = recoMuons.product(); // IMPORTANT - you can ONLY read this vector!
    std::vector<reco::Muon> tagMuons, probeMuons ;

    std::vector<l1t::RegionalMuonCand> L1Muons;
    for(auto l1mu : *bmtfDataMuons) {
      // remove BX +- muons (prosexos)!
      L1Muons.push_back(l1mu);
    }


    //ie ------------------------- STEP 1 ------------------------------

    double etaCut = 0.83;  // eta cut
    double ptCut  = 26.;   // Pt Cut
    double DR     = 0.12;  // DR cut for matching --> changed from 1.0 to 0.12 ***
    TLorentzVector Mu1, Mu2, diMu;

    for(auto itrig : L1Muons){
      double L1Eta = analysisTools::etaFromBmtf(itrig.hwEta());
      double L1Phi = analysisTools::phiFromBmtf(itrig.hwPhi(), itrig.processor()); 
      double L1Pt   = analysisTools::ptFromBmtf(itrig.hwPt());
      eta_online_0.Fill(L1Eta);
      phi_online_0.Fill(L1Phi);
      pt_online_0.Fill(L1Pt);
    }

    for(auto imu : *offMuons){
      if(imu.passed(imu.PFIsoTight)  && fabs(imu.eta()) < etaCut )   {   probeMuons.push_back(imu);
        chambers_hits.Fill(imu.numberOfChambersCSCorDT());
        phi_offline_0.Fill(imu.phi());
        eta_offline_0.Fill(imu.eta());
      }
    }

    //ie ----------------------- STEP 2 -----------------------------------------

    int TagProbe;        // if muon is tagged, TagPos shows the probe muon position
    int L1Pos;           // if muon is tagged, L1Pos shows the trigger muon position

    if(probeMuons.size() >= 2) { // only events with at least 2 probes  -----
      eff_events_2pr++;
      probes_gt2.Fill(probeMuons.size());

      //     std::reverse(begin(probeMuons), end(probeMuons)); // tested o.k.

      for(auto imu : probeMuons) {
        phi_offline_1.Fill(imu.phi());
        eta_offline_1.Fill(imu.eta());
      }

      TagProbe = 0;   
      for(auto imu : probeMuons) {
        if(imu.pt() < ptCut) break;
        L1Pos=0;  
        for(auto itrig : L1Muons){
          double L1Eta = analysisTools::etaFromBmtf(itrig.hwEta());
          double L1Phi = analysisTools::phiFromBmtf(itrig.hwPhi(), itrig.processor()); 
          double L1Pt   = analysisTools::ptFromBmtf(itrig.hwPt());
          phi_online_1.Fill(L1Phi);
          eta_online_1.Fill(L1Eta);
          dr_tag.Fill(sqrt((L1Phi-imu.phi())*(L1Phi-imu.phi()) + (L1Eta-imu.eta())*(L1Eta-imu.eta())));

          // pt-cut for probe and online muons. Match probe with one online muon within DR<0.1 cone
          if(L1Pt > ptCut && ((L1Phi-imu.phi())*(L1Phi-imu.phi()) + (L1Eta-imu.eta())*(L1Eta-imu.eta())) < DR*DR) {
            tagMuons.push_back(imu);  // tag found !!!

            phi_offline_2.Fill(imu.phi());
            eta_offline_2.Fill(imu.eta());
            phi_online_2.Fill(L1Phi);
            eta_online_2.Fill(L1Eta);

            break; // to avoid double match with another L1 mu
          }
          L1Pos++;
        }
        if(tagMuons.size()==1) break; // keep events with only one tag
        TagProbe++;
      }

      //ie  ------------------------------ STEP 3 -----------------------------------

      //    check invariant mass of a tag-probe pair
      int massOK = 0;
      int iProbe = 0; // this is the position of the "probe" muon (and TagProbe is the probe muon that was tagged)
      if(tagMuons.size() ==1){  // if tag found
        for(auto imu : probeMuons){
          if(iProbe==TagProbe) { iProbe++; continue; }  // skip the tag-probe muon
          Mu1.SetPtEtaPhiM(probeMuons[TagProbe].pt(),probeMuons[TagProbe].eta(),probeMuons[TagProbe].phi(),0.105658);
          Mu2.SetPtEtaPhiM(imu.pt(),imu.eta(),imu.phi(),0.105658);
          diMu = Mu1 + Mu2;
          dimuon.Fill(diMu.M());
          if(diMu.M() > 71 && diMu.M() < 111) {
            // remove tag muon and also its online partner
            probeMuons.erase(probeMuons.begin() + TagProbe);
            L1Muons.erase(L1Muons.begin() + L1Pos); 
            massOK = 1;
            break;  // "probe" found
          }
          iProbe++;
        }
      } 

      //ie  ------------------------------ STEP 4----------------------------            

      // fill histos of efficiencies with "probe" muon
      if(massOK==1) { 
        double DeltaR2 ;
        eff_events_pass++;
        eff_all.Fill(probeMuons[iProbe].pt());

        phi_offline_3.Fill(probeMuons[iProbe].phi());
        eta_offline_3.Fill(probeMuons[iProbe].eta());

        for(auto itrig : L1Muons){
          double L1Eta = analysisTools::etaFromBmtf(itrig.hwEta());
          double L1Phi = analysisTools::phiFromBmtf(itrig.hwPhi(), itrig.processor()); 
          DeltaR2 = (L1Phi-probeMuons[iProbe].phi()) * (L1Phi-probeMuons[iProbe].phi()) + 
            (L1Eta-probeMuons[iProbe].eta()) * (L1Eta- probeMuons[iProbe].eta());

          dr_probe.Fill(sqrt(DeltaR2));

          if(DeltaR2 < DR*DR ) {
            eff_pass.Fill(probeMuons[iProbe].pt());
            eff_events_final++;
            phi_online_3.Fill(L1Phi);
            eta_online_3.Fill(L1Eta);
            break ;
          } else {
            rej_phi.Fill(L1Phi);
            rej_eta.Fill(L1Eta);
            rej_DR.Fill(sqrt(DeltaR2));
          }
        }
      }
     
    } //end of at least 2  probeMuons 


  } //end of the 'doEfficiency' scope
} //end of analyze method


// ------------ method called once each job just before starting event loop  ------------
  void 
Validation::beginJob()
{

}

// ------------ method called once each job just after ending the event loop  ------------
  void 
Validation::endJob() 
{
  //Ratios
  TH1F phiRatio("phiRatio", "phi Ratio", phiBins, -20, 80);
  TH1F etaRatio("etaRatio", "eta Ratio", etaBins, -140, 140);
  TH1F ptRatio("ptRatio", "pt Ratio", ptBins, 0, 300);
  TH1F pt2Ratio("pt2Ratio", "pt2 Ratio", ptBins, 0, 300);
  TH1F dxyRatio("dxyRatio", "dXY Ratio", dxyBins, 0, 3);
  for (int bin = 1; bin <= phiBins; bin++) {
    if (phisData.GetBinContent(bin) > 0)
      phiRatio.SetBinContent(bin, phisEmu.GetBinContent(bin)/phisData.GetBinContent(bin));
  }
  for (int bin = 1; bin <= etaBins; bin++) {
    if (etasData.GetBinContent(bin) > 0)
      etaRatio.SetBinContent(bin, etasEmu.GetBinContent(bin)/etasData.GetBinContent(bin));
  }
  for (int bin = 1; bin <= ptBins; bin++) {
    if (ptData.GetBinContent(bin) > 0)
      ptRatio.SetBinContent(bin, ptEmu.GetBinContent(bin)/ptData.GetBinContent(bin));
  }
  for (int bin = 1; bin <= ptBins; bin++) {
    if (pt2Data.GetBinContent(bin) > 0)
      pt2Ratio.SetBinContent(bin, pt2Emu.GetBinContent(bin)/pt2Data.GetBinContent(bin));
  }
  for (int bin = 1; bin <= dxyBins; bin++) {
    if (dxyData.GetBinContent(bin) > 0)
      dxyRatio.SetBinContent(bin, dxyEmu.GetBinContent(bin)/dxyData.GetBinContent(bin));
  }

  //TFileDirectory dir;
  file->mkdir(system.c_str());

  if (file -> cd(system.c_str())) {//it returns true in case of success
    phisHist.Write();
    etasHist.Write();
    ptHist.Write();
    pt2Hist.Write();
    sizeHist.Write();

    phiRatio.Write();
    etaRatio.Write();
    ptRatio.Write();
    pt2Ratio.Write();
    dxyRatio.Write();

    phisData.Write();
    etasData.Write();
    ptData.Write();
    pt2Data.Write();
    dxyData.Write();
    fineBitData.Write();
    TA1Data.Write();
    TA2Data.Write();
    TA3Data.Write();
    TA4Data.Write();
    wheelData.Write();

    phisEmu.Write();
    etasEmu.Write();
    ptEmu.Write();
    pt2Emu.Write();
    dxyEmu.Write();
    fineBitEmu.Write();
    TA1Emu.Write();
    TA2Emu.Write();
    TA3Emu.Write();
    TA4Emu.Write();
    wheelEmu.Write();

    eff_all.Write();
    eff_pass.Write();
   
    phi_online_0.Write();
    phi_online_1.Write();
    phi_online_2.Write();
    phi_online_3.Write();
    eta_online_0.Write();
    eta_online_1.Write();
    eta_online_2.Write();
    eta_online_3.Write();

    pt_online_0.Write();

    phi_offline_0.Write();
    phi_offline_1.Write();
    phi_offline_2.Write();
    phi_offline_3.Write();
    eta_offline_0.Write();
    eta_offline_1.Write();
    eta_offline_2.Write();
    eta_offline_3.Write();

    chambers_hits.Write();
    probes_gt2.Write();
    dr_tag.Write();
    dr_probe.Write();
    dimuon.Write();

    rej_phi.Write();
    rej_eta.Write();
    rej_DR.Write();

  }
  else
    fout << "Cannot save the output to the file." << std::endl;

  fout << std::endl;
  fout << "SUMMARY" << std::endl;
  fout << "===================================================" << std::endl;
  fout << "Processed Events: " << events << std::endl;
  fout << "Emulator #Muons:  " << emuMuonSize << std::endl;

  fout << "Data #Muons:      " << dataMuonSize << std::endl;
  fout << "pt Mismatches:    " << ptMismatches << " (" << std::setprecision(2) << 100.*ptMismatches/emuMuonSize  << "%)" << std::endl;
  if (system == "KMTF") {
    fout << "pt2 Mismatches:   " << pt2Mismatches << " (" << std::setprecision(2) << 100.*pt2Mismatches/emuMuonSize << "%)" << std::endl;
    fout << "dxy Mismatches:   " << dxyMismatches << " (" << std::setprecision(2) << 100.*dxyMismatches/emuMuonSize << "%)" << std::endl;
  }
  fout << "phi Mismatches:   " << phiMismatches   << " (" << std::setprecision(2) << 100.*phiMismatches/emuMuonSize << "%)" << std::endl;
  fout << "eta Mismatches:   " << etaMismatches   << " (" << std::setprecision(2) << 100.*etaMismatches/emuMuonSize << "%)" << std::endl;
  fout << "wheel Mismatches: " << wheelMismatches << " (" << std::setprecision(2) << 100.*wheelMismatches/emuMuonSize << "%)" << std::endl;

  fout << "size Mismatches:  " << mismatchedContainers << " (" << std::setprecision(2) << 100.*(fabs(emuMuonSize-dataMuonSize)/(float)emuMuonSize) << "%)" << std::endl;
  //fout << "Muons not matched: " << muonsMissing << "\tpercent: " << 100*muonsMissing/events << "%" << std::endl;

  if (doEfficiency) { // guard of unused vars in case of 'validation plots only' mode
    fout << "Events with >= 2 Probes  mu: " << eff_events_2pr   << std::endl;
    fout << "Events which pass to efficiency: " << eff_events_pass << std::endl;
    fout << "Events which pass final selection: " << eff_events_final << std::endl;
  }

  fout.close();
}

// ------------ method Calculates anythng else the user needs in parallel with the validation procedure  ------------
void
Validation::otherCalculation(const edm::Handle<L1MuDTChambPhContainer> phiHits, TH2I& hist2D) {
  return;  
}

void
Validation::printEvent(const L1MuDTChambPhContainer* phis, const L1MuDTChambThContainer* etas,
    const l1t::RegionalMuonCandBxCollection* muons_A, const l1t::RegionalMuonCandBxCollection* muons_B,
    const std::string type = "UNSPECIFIED") {
  fout << "------- NEW [" << type << "] MISMATCH EVENT -------" << std::endl;

  for (int proc=0; proc<12; proc++) {
    fout << ">>> processor=" << proc << " <<<" << std::endl;

    for (int bx=-2; bx<=2; bx++) {
      fout << "--> BX=" << bx << std::endl;

      fout << "Wheel\tTs2tag\tStation\tRPC-bit\tQuality\tPhiB\tPhi" << std::endl;
      for (auto phi : *(phis->getContainer()) ) {
        if (proc != phi.scNum())
          continue;

        if (bx != phi.bxNum())
          continue;

        fout << phi.whNum() << "\t";
        fout << phi.Ts2Tag() << "\t";
        fout << phi.stNum() << "\t";
        fout << phi.RpcBit() << "\t";
        fout << phi.code() << "\t";
        fout << phi.phiB() << "\t";
        fout << phi.phi() << std::endl;
        // fout << "processor=" << phi.scNum() << std::endl;
        // fout << "BX=" << phi.bxNum() << "\t";
      }

      fout << "Wheel\tStation\tQuality\tEtaHits" << std::endl;
      for (auto eta : *(etas->getContainer()) ) {
        if (proc != eta.scNum())
          continue;

        if (bx != eta.bxNum())
          continue;

        fout << eta.whNum() << "\t";
        fout << eta.stNum() << "\t";

        //fout << "etaQual=";
        for (int i=0; i<7; i++)
          fout << eta.quality(i);
        fout << "\t";

        //fout << "etaPos=";
        for (int i=0; i<7; i++)
          fout << eta.position(i);
        fout << std::endl;

        // fout << "processor=" << eta.scNum() << std::endl;
        // fout << "BX=" << eta.bxNum() << "\t";
      }

    }//bx iteration 

    fout << std::endl;
  }//proc iteration

  for (int bx = muons_A->getFirstBX(); bx <= muons_A->getLastBX(); bx++) {
    //  for (auto muon : *muons_A ) {
    for (auto muon = muons_A->begin(bx); muon < muons_A->end(bx); muon++) {

      std::map<int, int> mu_TA = muon->trackAddress();
      //if (mu_TA.size() < 5)
      //return;

      fout << std::endl;
      fout << "__________DATA_____________________________________" << std::endl;
      fout << "HW_Processor=\t" << muon->processor() << std::endl;
      fout << "HW_BX=\t\t" << bx << std::endl;
      fout << "HW_Link=\t" << muon->link() << std::endl;  
      fout << "HW_Pt=\t\t" << muon->hwPt() << std::endl;
      fout << "HW_Phi=\t\t" << muon->hwPhi() << std::endl;
      fout << "HW_Eta=\t\t" << muon->hwEta() << std::endl;
      fout << "HW_fineBit=\t" << muon->hwHF() << std::endl;
      fout << "HW_Quality=\t" << muon->hwQual() << std::endl;
      fout << "HW_TrckAddress=\t" << analysisTools::toHex(mu_TA.at(2)) << analysisTools::toHex(mu_TA.at(3));
      fout << analysisTools::toHex(mu_TA.at(4)) << analysisTools::toHex(mu_TA.at(5)) << std::endl;
      fout << "HW_Sign=\t" << muon->hwSign() << std::endl;
      fout << "HW_SignValid=\t" << muon->hwSignValid() << std::endl;

    }//muons data
  }


  for (int bx = muons_B->getFirstBX(); bx <= muons_B->getLastBX(); bx++) {
    //  for (auto muon : *muons_B ) {
    for (auto muon = muons_B->begin(bx); muon < muons_B->end(bx); muon++) {

      std::map<int, int> mu_TA = muon->trackAddress();
      //if (mu_TA.size() < 5)
      //return;

      fout << std::endl;
      fout << "__________EMULATOR_________________________________" << std::endl;
      fout << "SW_Processor=\t" << muon->processor() << std::endl;
      fout << "SW_BX=\t\t" << bx << std::endl;
      fout << "SW_Link=\t" << muon->link() << std::endl;  
      fout << "SW_Pt=\t\t" << muon->hwPt() << std::endl;
      fout << "SW_Phi=\t\t" << muon->hwPhi() << std::endl;
      fout << "SW_Eta=\t\t" << muon->hwEta() << std::endl;
      fout << "SW_fineBit=\t" << muon->hwHF() << std::endl;
      fout << "SW_Quality=\t" << muon->hwQual() << std::endl;
      fout << "SW_TrckAddress=\t" << analysisTools::toHex(mu_TA.at(2)) << analysisTools::toHex(mu_TA.at(3));
      fout << analysisTools::toHex(mu_TA.at(4)) << analysisTools::toHex(mu_TA.at(5)) << std::endl;
      fout << "SW_Sign=\t" << muon->hwSign() << std::endl;
      fout << "SW_SignValid=\t" << muon->hwSignValid() << std::endl;

    }//muons emulator

  }//muons_BX iteration

  fout << std::endl << std::endl;
  }


  int
    Validation::returnWheel_2sComp(const int& side, const int& wheel) {
      if (side == 1 && wheel == 1)
        return -3;
      else if (side == 1 && wheel == 2)
        return -2;
      else if (side == 1 && wheel == 3)
        return -1;
      else if (side == 0 && wheel == 0)
        return 0;
      else if (side == 0 && wheel == 1)
        return 1;
      else if (side == 0 && wheel == 2)
        return 2;
      else if (side == 0 && wheel == 3)
        return 3;
      else {
        std::cout << "Unrecognized pair of side and wheel.." << std::endl;
        return -99;
      }
    }


  int
    Validation::returnWheel_Janos(const int& side, const int& wheel) {
      if (side == 1 && wheel == 3)
        return -3;
      else if (side == 1 && wheel == 2)
        return -2;
      else if (side == 1 && wheel == 1)
        return -1;
      else if ( (side == 1 && wheel == 0) || (side == 0 && wheel == 0) )
        return 0;
      else if (side == 0 && wheel == 1)
        return 1;
      else if (side == 0 && wheel == 2)
        return 2;
      else if (side == 0 && wheel == 3)
        return 3;
      else {
        std::cout << "Unrecognized pair of side and wheel.." << std::endl;
        return -99;
      }
    }


  int
    Validation::returnWheel_Michalis(const int& side, const int& wheel) {
      if (side == 1 && wheel == 3)
        return -3;
      else if (side == 1 && wheel == 2)
        return -2;
      else if (side == 1 && wheel == 1)
        return -1;
      else if (side == 1 && wheel == 0)
        return 4;//return error
      else if (side == 0 && wheel == 0)
        return 0;
      else if (side == 0 && wheel == 1)
        return 1;
      else if (side == 0 && wheel == 2)
        return 2;
      else if (side == 0 && wheel == 3)
        return 3;
      else {
        std::cout << "Unrecognized pair of side and wheel.." << std::endl;
        return -4;//return serious error
      }
    }

  /*/// printPairMuons

  //std::map<int, int> muA_TA = muon_A.trackAddress(), muB_TA = muon_B->trackAddress();
  //if (muA_TA.size() < 5 || muB_TA.size() < 5)
  //return;

  //    std::cout << std::endl;
  //    std::cout << "HW-Muon" << "\t\t\t| " << "SW-Muon" << std::endl;
  //    std::cout << "HW_Processor=\t" << muon_A.processor() << "\t| " << "SW_Processor=\t" << muon_B->processor() << std::endl;
  //    std::cout << "HW_Link=\t" << muon_A.link() << "\t| " << "SW_Link=\t" << muon_B->link() << std::endl;  
  //    std::cout << "HW_Pt=\t\t" << muon_A.hwPt() << "\t| " << "SW_Pt=\t" << muon_B->hwPt() << std::endl;
  //    std::cout << "HW_Phi=\t\t" << muon_A.hwPhi() << "\t| " << "SW_Phi=\t" << muon_B->hwPhi() << std::endl;
  //    std::cout << "HW_Eta=\t\t" << muon_A.hwEta() << "\t| " << "SW_Eta=\t" << muon_B->hwEta() << std::endl;
  //    std::cout << "HW_fineBit=\t" << muon_A.hwHF() << "\t| " << "SW_fineBit=\t" << muon_B->hwHF() << std::endl;
  //    std::cout << "HW_Quality=\t" << muon_A.hwQual() << "\t| " << "SW_Quality=\t" << muon_B->hwQual() << std::endl;
  //    //std::cout << "HW_TrckAddress=\t" << analysisTools::toHex(muA_TA.at(2)) << analysisTools::toHex(muA_TA.at(3));
  //    //std::cout << analysisTools::toHex(muA_TA.at(4)) << analysisTools::toHex(muA_TA.at(5)) << "\t| ";
  //    //std::cout << "SW_TrckAddress= " << analysisTools::toHex(muB_TA.at(2)) << analysisTools::toHex(muB_TA.at(3));
  //    //std::cout << analysisTools::toHex(muB_TA.at(4)) << analysisTools::toHex(muB_TA.at(5)) << std::endl;
  //    std::cout << "HW_Sign=\t" << muon_A.hwSign() << "\t| " << "SW_Sign=\t" << muon_B->hwSign() << std::endl;
  //    std::cout << "HW_SignValid=\t" << muon_A.hwSignValid() << "\t| " << "SW_SignValid=\t" << muon_B->hwSignValid() << std::endl;

*/


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
Validation::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {

  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("phiHits", edm::InputTag("bmtfDigis"));
  desc.add<edm::InputTag>("etaHits", edm::InputTag("bmtfDigis"));
  desc.add<edm::InputTag>("muonsData", edm::InputTag("bmtfDigis:BMTF"));
  desc.add<edm::InputTag>("muonsEmu", edm::InputTag("simBmtfDigis:BMTF"));
  desc.add<edm::InputTag>("muonsReco", edm::InputTag("muons"));
  desc.add<std::string>("system", "BMTF");
  desc.add<bool>("doEfficiency", false);

  descriptions.add("muonStudy",desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(Validation);

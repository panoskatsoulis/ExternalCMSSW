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
// Original Author:  Panagiotis Katsoulis
//         Created:  Sun, 03 Sep 2017 19:10:07 GMT
//
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

#include "EventFilter/L1TRawToDigi/interface/AMC13Spec.h"
#include "EventFilter/L1TRawToDigi/interface/Block.h"

#include "L1Trigger/L1TMuon/interface/RegionalMuonRawDigiTranslator.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//#include "TH1.h"
#include "TH1F.h"
#include "TH1I.h"
//#include "TH2.h"
#include "TH2F.h"
#include "TH2I.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "MyModule/MuonStudy/interface/analysisTools.h"

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class MuonStudy : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit MuonStudy(const edm::ParameterSet&);
  ~MuonStudy();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  void otherCalculation(const edm::Handle<L1MuDTChambPhContainer>, TH2I&);
  void printEvent(const L1MuDTChambPhContainer*, const L1MuDTChambThContainer*, 
		  const l1t::RegionalMuonCandBxCollection*, const l1t::RegionalMuonCandBxCollection*, 
		  const std::string);

  // edm::EDGetTokenT<FEDRawDataCollection> binToken_original;
  // edm::EDGetTokenT<FEDRawDataCollection> binToken_replica;

  edm::EDGetTokenT<L1MuDTChambPhContainer> phiToken;
  edm::EDGetTokenT<L1MuDTChambThContainer> etaToken;
  edm::EDGetTokenT<l1t::RegionalMuonCandBxCollection> token_bmtfData;
  edm::EDGetTokenT<l1t::RegionalMuonCandBxCollection> token_bmtfEmu;
  
  edm::Service<TFileService> fs;
  std::ofstream fout;
  TFile *file;

  std::string system;

  TH2F phisHist;//All Etas 2D
  TH2F etasHist;
  TH2F ptHist;
  TH2F pt2Hist;
  TH2I sizeHist;
  TH2I bxHist;//not yet implemented
  TH2F phisCoarse;//Coarse Etas 2D
  TH2F etasCoarse;
  TH2F ptCoarse;
  TH2I sizeCoarse;
  TH1F phisData, phisEmu;
  TH1F etasData, etasEmu;
  TH1F ptData, ptEmu;
  TH1F pt2Data, pt2Emu;
  TH1I dxyData, dxyEmu;
  TH1I fineBitData, fineBitEmu;
  TH1F phisRatio;//ratios
  TH1F etasRatio;
  TH1F ptRatio;
  TH1F pt2Ratio;  
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
  int sizeMismatches;
  int events;
  int emuMuonSize;
  int dataMuonSize;
  //other calculations
  TH2I tsTagHist;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
MuonStudy::MuonStudy(const edm::ParameterSet& iConfig)

{
  
  // std::string labelFEDData_1("rawDataCollector");
  // binToken_original = consumes<FEDRawDataCollection>(edm::InputTag(labelFEDData_1));  

  system = iConfig.getParameter<std::string>("system");
  phiToken = consumes< L1MuDTChambPhContainer >(iConfig.getParameter<edm::InputTag>("phiHits"));
  etaToken = consumes< L1MuDTChambThContainer >(iConfig.getParameter<edm::InputTag>("etaHits"));
  token_bmtfData = consumes< l1t::RegionalMuonCandBxCollection >(iConfig.getParameter<edm::InputTag>("muonsData"));
  token_bmtfEmu = consumes< l1t::RegionalMuonCandBxCollection >(iConfig.getParameter<edm::InputTag>("muonsEmu"));

  fout.open(system+"_mismatchesFile");
  usesResource("TFileService");
  file = &(fs -> file());

  //Binning
  phiBins = 100;
  etaBins = 280; //etaBins = 56;
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

  // phisCoarse = TH2F("muonPhis_coarse","phis comparison", phiBins, -20, 80, phiBins, -20, 80);
  // etasCoarse = TH2F("muonEtas_coarse","eta comparison", etaBins, -140, 140, etaBins, -140, 140);
  // ptCoarse = TH2F("muonPt_coarse","pT comparison", ptBins, 0, 300, ptBins, 0, 300);
  // sizeCoarse = TH2I("bmtfSize_coarse","Muons# comparison", sizeBins, 0, 20, sizeBins, 0, 20);

  phisData = TH1F("phisData","#phi BMTF", phiBins, -20, 80);
  etasData = TH1F("etasData","#eta BMTF", etaBins, -140, 140);
  ptData = TH1F("ptData","Pt BMTF", ptBins, 0, 300);
  pt2Data = TH1F("pt2Data","Pt_{2} BMTF", ptBins, 0, 300);
  dxyData = TH1I("dxyData","dxy BMTF", dxyBins, 0, 4);
  fineBitData = TH1I("fineBitData","fineBit data", fBBins, -1, 2);

  phisEmu = TH1F("phisEmu","#phi BMTF", phiBins, -20, 80);
  etasEmu = TH1F("etasEmu","#eta BMTF", etaBins, -140, 140);
  ptEmu = TH1F("ptEmu","Pt BMTF", ptBins, 0, 300);
  pt2Emu = TH1F("pt2Emu","Pt_{2} BMTF", ptBins, 0, 300);
  dxyEmu = TH1I("dxyEmu","dxy BMTF", dxyBins, 0, 4);
  fineBitEmu = TH1I("fineBitEmu","fineBit emu", fBBins, -1, 2);

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
  events = 0;
  emuMuonSize = 0;
  dataMuonSize = 0;

  //tsTagHist = TH2I("tsTagHist","TsTag comparison", 2, 0, 2, 2, 0, 2);
}


MuonStudy::~MuonStudy()
{
 
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
MuonStudy::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

  /*   
       #ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
       ESHandle<SetupData> pSetup;
       iSetup.get<SetupRecord>().get(pSetup);
       #endif
  */

  // //Bin Collections
  // edm::Handle<FEDRawDataCollection> binRAW;
  // iEvent.getByToken(binToken_original, binRAW);

  // edm::Handle<FEDRawDataCollection> binRAWPacked;
  // iEvent.getByToken(binToken_replica, binRAWPacked);


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


  //Check Event Validity
  //if (not phiInput.isValid() && not etaInput.isValid())

  //Count events
  events++;

  //====================== other calculations =========================================================
  
  //otherCalculation(phiInput, tsTagHist);

  //====================== muons Handler ==============================================================
   
  // if (bmtfDataMuons -> size() != bmtfEmuMuons -> size()) {
  //   //   std::cout << "muons size not equal" << std::endl;
  //   //   std::cout << "coll-A:" << bmtfDataMuons -> size()
  // 	       << " coll-B:" << bmtfEmuMuons -> size() << std::endl << std::endl;
  //   muonsSize++;
  // }

  // Validation Cuts
  for (auto imuA : *bmtfDataMuons )
    if (imuA.hwPt() < 25) return;
  //-------- this is an OR -------------------
  for (auto imuB : *bmtfEmuMuons )
    if (imuB.hwPt() < 25) return;

  //MAIN CODE
  for (auto imuA : *bmtfDataMuons ) {
      //data 1D histograms
      phisData.Fill(imuA.hwPhi());
      etasData.Fill(imuA.hwEta());
      ptData.Fill(imuA.hwPt());
      pt2Data.Fill(imuA.hwPt2());
      dxyData.Fill(imuA.hwDXY());
      fineBitData.Fill(imuA.hwHF());
  }

  for (auto imuB : *bmtfEmuMuons ) {
      //emu 1D histograms
      phisEmu.Fill(imuB.hwPhi());
      etasEmu.Fill(imuB.hwEta());
      ptEmu.Fill(imuB.hwPt());
      pt2Emu.Fill(imuB.hwPt2());
      dxyEmu.Fill(imuB.hwDXY());
      fineBitEmu.Fill(imuB.hwHF());
  }


  if ( (*bmtfDataMuons).size() == (*bmtfEmuMuons).size() ) {
    auto imuB = bmtfEmuMuons -> begin();
    for (auto imuA : *bmtfDataMuons ) {

      // //test muon print - prints muon pairs for each muons' iterations (they loop together!)
      // printMuonPair(&imuA, &(*imuB));

      // //OLD BLOCK of CODE
      // //data 1D histograms
      // phisData.Fill(imuA.hwPhi());
      // etasData.Fill(imuA.hwEta());
      // ptData.Fill(imuA.hwPt());
      // pt2Data.Fill(imuA.hwPt2());
      // dxyData.Fill(imuA.hwDXY());
      // fineBitData.Fill(imuA.hwHF());
      // //emu 1D histograms
      // phisEmu.Fill(imuB->hwPhi());
      // etasEmu.Fill(imuB->hwEta());
      // ptEmu.Fill(imuB->hwPt());
      // pt2Emu.Fill(imuB->hwPt2());
      // dxyEmu.Fill(imuB->hwDXY());
      // fineBitEmu.Fill(imuB->hwHF());
      

      //identify correct muon - 2D histograms
      bool filled(false);//, sameMuon(false);
      if (imuA.hwPhi() != imuB->hwPhi() ||
      	  imuA.hwEta() != imuB->hwEta() ||
      	  imuA.hwPt() != imuB->hwPt() ||
      	  imuA.hwPt2() != imuB->hwPt2()) {

	for (auto emuMuon: *bmtfEmuMuons ) {//iterate in emulator muons and find a match
	  // if ( (imuA.hwPhi() >= emuMuon.hwPhi()-2 || imuA.hwPhi() <= emuMuon.hwPhi()+2) &&
	  //      (imuA.hwEta() >= emuMuon.hwEta()-2 || imuA.hwEta() >= emuMuon.hwEta()+2) &&
	  //      (imuA.hwPt() >= emuMuon.hwPt()-2 || imuA.hwPt() >= emuMuon.hwPt()+2) &&
	  //      (imuA.hwPt2() >= emuMuon.hwPt2()-2 || imuA.hwPt2() >= emuMuon.hwPt2()+2) ) {


	  if (imuA.hwPhi() == emuMuon.hwPhi() &&
	      imuA.hwEta() == emuMuon.hwEta() &&
	      imuA.hwPt() == emuMuon.hwPt() &&
	      imuA.hwPt2() == emuMuon.hwPt2()) {
	    
	    phisHist.Fill(imuA.hwPhi(), emuMuon.hwPhi());
	    etasHist.Fill(imuA.hwEta(), emuMuon.hwEta());
	    ptHist.Fill(imuA.hwPt(), emuMuon.hwPt());
	    pt2Hist.Fill(imuA.hwPt2(), emuMuon.hwPt2());

	    // if (imuA.hwHF() == 0) {
	    //   phisCoarse.Fill(imuA.hwPhi(), emuMuon.hwPhi());
	    //   etasCoarse.Fill(imuA.hwEta(), emuMuon.hwEta());
	    //   ptCoarse.Fill(imuA.hwPt(), emuMuon.hwPt());
	    // }

	    filled = true;
	    break;
	  }
	}

      }//muon is diff
      // else
      // 	sameMuon = true;

      if (filled)
	filledBySearch++;
      // else if (!filled && sameMuon) {
      // 	phisHist.Fill(imuA.hwPhi(), imuB->hwPhi());
      // 	etasHist.Fill(imuA.hwEta(), imuB->hwEta());
      // 	ptHist.Fill(imuA.hwPt(), imuB->hwPt());

      // 	if (imuA.hwHF() == 0) {
      // 	  phisCoarse.Fill(imuA.hwPhi(), imuB->hwPhi());
      // 	  etasCoarse.Fill(imuA.hwEta(), imuB->hwEta());
      // 	  ptCoarse.Fill(imuA.hwPt(), imuB->hwPt());
      // 	}

      // 	filledSame++;
      // }
      else {
	phisHist.Fill(imuA.hwPhi(), imuB->hwPhi());
      	etasHist.Fill(imuA.hwEta(), imuB->hwEta());
      	ptHist.Fill(imuA.hwPt(), imuB->hwPt());
      	pt2Hist.Fill(imuA.hwPt2(), imuB->hwPt2());

	// if (imuA.hwHF() == 0) {
	//   phisCoarse.Fill(imuA.hwPhi(), imuB->hwPhi());
	//   etasCoarse.Fill(imuA.hwEta(), imuB->hwEta());
	//   ptCoarse.Fill(imuA.hwPt(), imuB->hwPt());
	// }

	//notMatched++;
      }


      //Mismatches
      if (imuA.hwPhi() != imuB->hwPhi()) {
	phiMismatches++;
	printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "PHI");
	//printMuonPair(&imuA, &(*imuB));
      }
      if (imuA.hwEta() != imuB->hwEta()) {
	etaMismatches++;
	printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "ETA");
	//printMuonPair(&imuA, &(*imuB));
      }
      if (imuA.hwPt() != imuB->hwPt()) {
	ptMismatches++;
	printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "PT");
	//printMuonPair(&imuA, &(*imuB));
      }
      if (system == "KMTF" && imuA.hwPt2() != imuB->hwPt2()) {
	pt2Mismatches++;
	printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "PT2");
	//printMuonPair(&imuA, &(*imuB));
      }
      if (system == "KMTF" && imuA.hwDXY() != imuB->hwDXY()) {
	dxyMismatches++;
	printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "DXY");
	//printMuonPair(&imuA, &(*imuB));
      }


      //next emu muon
      imuB++;

    }//A-itr

  }
  else {
    mismatchedContainers++;

    if ( ((*bmtfDataMuons).size() != 0) && ((*bmtfEmuMuons).size() != 0) ) {
      auto imuB = bmtfEmuMuons -> begin();
      for (auto imuA : *bmtfDataMuons ) {

	bool filled(false);
	if (imuA.hwPhi() != imuB->hwPhi() ||
	    imuA.hwEta() != imuB->hwEta() ||
	    imuA.hwPt() != imuB->hwPt() ||
	    imuA.hwPt2() != imuB->hwPt2()) {

	  for (auto emuMuon: *bmtfEmuMuons ) {//iterate in emulator muons and find a match

	  // if ( (imuA.hwPhi() >= emuMuon.hwPhi()-2 || imuA.hwPhi() <= emuMuon.hwPhi()+2) &&
	  //      (imuA.hwEta() >= emuMuon.hwEta()-2 || imuA.hwEta() >= emuMuon.hwEta()+2) &&
	  //      (imuA.hwPt() >= emuMuon.hwPt()-2 || imuA.hwPt() >= emuMuon.hwPt()+2) &&
	  //      (imuA.hwPt2() >= emuMuon.hwPt2()-2 || imuA.hwPt2() >= emuMuon.hwPt2()+2) ) {

	    if (imuA.hwPhi() == emuMuon.hwPhi() &&
	    	imuA.hwEta() == emuMuon.hwEta() &&
	    	imuA.hwPt() == emuMuon.hwPt() &&
	    	imuA.hwPt2() == emuMuon.hwPt2()) {
	    
	      phisHist.Fill(imuA.hwPhi(), emuMuon.hwPhi());
	      etasHist.Fill(imuA.hwEta(), emuMuon.hwEta());
	      ptHist.Fill(imuA.hwPt(), emuMuon.hwPt());
	      pt2Hist.Fill(imuA.hwPt2(), emuMuon.hwPt2());

	      // if (imuA.hwHF() == 0) {
	      // 	phisCoarse.Fill(imuA.hwPhi(), emuMuon.hwPhi());
	      // 	etasCoarse.Fill(imuA.hwEta(), emuMuon.hwEta());
	      // 	ptCoarse.Fill(imuA.hwPt(), emuMuon.hwPt());
	      // }

	      filled = true;
	      break;
	    }
	  }

	}//muon is diff

	if (filled)
	  filledBySearch++;
	else {
	  phisHist.Fill(imuA.hwPhi(), imuB->hwPhi());
	  etasHist.Fill(imuA.hwEta(), imuB->hwEta());
	  ptHist.Fill(imuA.hwPt(), imuB->hwPt());
	  pt2Hist.Fill(imuA.hwPt2(), imuB->hwPt2());

	  // if (imuA.hwHF() == 0) {
	  //   phisCoarse.Fill(imuA.hwPhi(), imuB->hwPhi());
	  //   etasCoarse.Fill(imuA.hwEta(), imuB->hwEta());
	  //   ptCoarse.Fill(imuA.hwPt(), imuB->hwPt());
	  // }
	}
      
	//Mismatches
	if (imuA.hwPhi() != imuB->hwPhi()) {
	  phiMismatches++;
	  printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "PHI");
	  //printMuonPair(&imuA, &(*imuB));
	}
	if (imuA.hwEta() != imuB->hwEta()) {
	  etaMismatches++;
	  printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "ETA");
	  //printMuonPair(&imuA, &(*imuB));
	}
	if (imuA.hwPt() != imuB->hwPt()) {
	  ptMismatches++;
	  printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "PT");
	  //printMuonPair(&imuA, &(*imuB));
	}
	if (system == "KMTF" && imuA.hwPt2() != imuB->hwPt2()) {
	  pt2Mismatches++;
	  printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "PT2");
	  //printMuonPair(&imuA, &(*imuB));
	}
	if (system == "KMTF" && imuA.hwDXY() != imuB->hwDXY()) {
	  dxyMismatches++;
	  printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "DXY");
	  //printMuonPair(&imuA, &(*imuB));
	}

	if (imuB == bmtfEmuMuons -> end())//since they are not of the same size
	  break;
	imuB++;
      }//muon A iter
    }
    else {
      if ((*bmtfDataMuons).size() == 0)
	fout << "data container was empty" << std::endl;
      else
	fout << "emu container was empty" << std::endl;
    }
  }//not same containers.
    
  //Size Histogram 2D
  sizeHist.Fill((*bmtfDataMuons).size(), (*bmtfEmuMuons).size());
  emuMuonSize +=(*bmtfEmuMuons).size();
  dataMuonSize += (*bmtfDataMuons).size();


  if ((*bmtfDataMuons).size() != (*bmtfEmuMuons).size())
    printEvent(phiInput.product(), etaInput.product(), bmtfDataMuons.product(), bmtfEmuMuons.product(), "SIZE");


//       if (system == "KMTF"){
// //	std::cout << "kmtf emu----------" << std::endl;
// 	for (int bx = bmtfEmuMuons->getFirstBX(); bx <= bmtfEmuMuons->getLastBX(); bx++) {
// 	  if (bmtfEmuMuons->size(bx) == 0)
// 	    continue;
// //	  std::cout << "BX = " << bx << std::endl;
// 	  for (auto bxMuon = bmtfEmuMuons->begin(bx); bxMuon != bmtfEmuMuons->end(bx); bxMuon++) {
// //	    std::cout << "proc=" << bxMuon->processor()
// 		      // << "  pt=" << bxMuon->hwPt()
// 		      // << " eta=" << bxMuon->hwEta()
// 		      // << " phi=" << bxMuon->hwPhi()
// 		      // << " dxy=" << bxMuon->hwDXY()
// 		      // << "  ch=" << bxMuon->hwSign()
// 		      // << "   Q=" << bxMuon->hwQual() 
// 		      // << std::endl;
// 	  }
// 	}//debug
//	std::cout << "---------------------------" << std::endl;//debug
//}

}//analyze method


// ------------ method called once each job just before starting event loop  ------------
void 
MuonStudy::beginJob()
{

}

// ------------ method called once each job just after ending the event loop  ------------
void 
MuonStudy::endJob() 
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

  // TList phiList;
  // phiList.Add(&phisData);
  // phiList.Add(&phisEmu);
  // TCanvas *phiCan = analysisTools::makeCanvas(phiList, "Phis Superimposed");

  // TList etaList;
  // etaList.Add(&etasData);
  // etaList.Add(&etasEmu);
  // TCanvas *etaCan = analysisTools::makeCanvas(etaList, "Etas Superimposed");

  // TList ptList;
  // ptList.Add(&ptData);
  // ptList.Add(&ptEmu);
  // TCanvas *ptCan = analysisTools::makeCanvas(ptList, "Pts Superimposed");

  // TList pt2List;
  // pt2List.Add(&pt2Data);
  // pt2List.Add(&pt2Emu);
  // TCanvas *pt2Can = analysisTools::makeCanvas(pt2List, "Pt2s Superimposed");

  //TFileDirectory dir;
  file->mkdir(system.c_str());

  if (file -> cd(system.c_str())) {//it returns true in case of success
    //Drawing Customization
    // phisHist.SetDrawOption("COLZ");
    // phisHist.GetXaxis()->SetTitle("Data");
    // phisHist.GetXaxis()->SetTitleOffset(1.2);
    // phisHist.GetYaxis()->SetTitle("Emulator");
    // phisHist.GetYaxis()->SetTitleOffset(1.2);
    // etasHist.SetDrawOption("COLZ");
    // etasHist.GetXaxis()->SetTitle("Data");
    // etasHist.GetXaxis()->SetTitleOffset(1.2);
    // etasHist.GetYaxis()->SetTitle("Emulator");
    // etasHist.GetYaxis()->SetTitleOffset(1.2);
    // ptHist.SetDrawOption("COLZ");
    // ptHist.GetXaxis()->SetTitle("Data");
    // ptHist.GetXaxis()->SetTitleOffset(1.2);
    // ptHist.GetYaxis()->SetTitle("Emulator");
    // ptHist.GetYaxis()->SetTitleOffset(1.2);
    // sizeHist.SetDrawOption("COLZ");
    // sizeHist.GetXaxis()->SetTitle("Data");
    // sizeHist.GetXaxis()->SetTitleOffset(1.2);
    // sizeHist.GetYaxis()->SetTitle("Emulator");
    // sizeHist.GetYaxis()->SetTitleOffset(1.2);

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

    // phisCoarse.Write();
    // etasCoarse.Write();
    // ptCoarse.Write();
    // sizeCoarse.Write();

    phisData.Write();
    etasData.Write();
    ptData.Write();
    pt2Data.Write();
    dxyData.Write();
    fineBitData.Write();

    phisEmu.Write();
    etasEmu.Write();
    ptEmu.Write();
    pt2Emu.Write();
    dxyEmu.Write();
    fineBitEmu.Write();

    // phiCan->Write();
    // etaCan->Write();
    // ptCan->Write();

    //other calculations
    //tsTagHist.Write();
  }
  else
    fout << "Cannot save the output to the file." << std::endl;

  fout << std::endl;
  fout << "SUMMARY" << std::endl;
  fout << "===================================================" << std::endl;
  fout << "Processed Events: " << events << std::endl;
  fout << "Emulator #Muons: " << emuMuonSize << std::endl;
  fout << "Data #Muons: " << dataMuonSize << std::endl;
  fout << "Not Matched: " << notMatched << std::endl;
  fout << "Filled by Search: " << filledBySearch << std::endl;
  fout << "Filled as Same: " << filledSame << std::endl;
  fout << "pt Mismatches: " << ptMismatches << std::endl;
  if (system == "KMTF") {
    fout << "pt2 Mismatches: " << pt2Mismatches << std::endl;
    fout << "dxy Mismatches: " << dxyMismatches << std::endl;
  }
  fout << "phi Mismatches: " << phiMismatches << std::endl;
  fout << "eta Mismatches: " << etaMismatches << std::endl;
  fout << "size Mismatches: " << mismatchedContainers << std::endl;
  //fout << "Muons not matched: " << muonsMissing << "\tpercent: " << 100*muonsMissing/events << "%" << std::endl;

  fout.close();
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MuonStudy::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {

  edm::ParameterSetDescription desc;
  desc.add<edm::InputTag>("phiHits", edm::InputTag("bmtfDigis"));
  desc.add<edm::InputTag>("etaHits", edm::InputTag("bmtfDigis"));
  desc.add<edm::InputTag>("muonsData", edm::InputTag("bmtfDigis:BMTF"));
  desc.add<edm::InputTag>("muonsEmu", edm::InputTag("simBmtfDigis:BMTF"));
  desc.add<std::string>("system", "BMTF");

  descriptions.add("muonStudy",desc);
}

// ------------ method Calculates anythng else the user needs in parallel with the validation procedure  ------------
void
MuonStudy::otherCalculation(const edm::Handle<L1MuDTChambPhContainer> phiHits, TH2I& hist2D) {

  int tsTag0_eventSize = 0, tsTag1_eventSize = 0;
  for (auto phi: *(phiHits->getContainer()) ) {//edm::Handlers arepointers to their products
    if (phi.bxNum() != 0)
      continue;

    if (phi.Ts2Tag() == 0)
      tsTag0_eventSize++;

    if (phi.Ts2Tag() == 1)
      tsTag1_eventSize++;
  }//phis

  if (tsTag0_eventSize == 0 && tsTag1_eventSize == 0)
    hist2D.Fill(0.,0.);
  else if (tsTag0_eventSize > 0 && tsTag1_eventSize == 0)
    hist2D.Fill(1.,0.);
  else if (tsTag0_eventSize == 0 && tsTag1_eventSize > 0)
    hist2D.Fill(0.,1.);
  else if (tsTag0_eventSize > 0 && tsTag1_eventSize > 0)
    hist2D.Fill(1.,1.);
  
}

void
MuonStudy::printEvent(const L1MuDTChambPhContainer* phis, const L1MuDTChambThContainer* etas,
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

//define this as a plug-in
DEFINE_FWK_MODULE(MuonStudy);

   // How to access data
   // for (auto imuB : *bmtfEmuMuons ) {
   //   //itrB++;
   //   if (imuA.hwEta() == imuB.hwEta() &&
   // 	 imuA.hwHF() == imuB.hwHF() &&
   // 	 imuA.hwPt() == imuB.hwPt() &&
   // 	 imuA.hwPhi() == imuB.hwPhi() &&
   // 	 imuA.hwQual() == imuB.hwQual() &&
   // 	 imuA.hwSign() == imuB.hwSign() &&
   // 	 imuA.hwSignValid() == imuB.hwSignValid() &&
   // 	 imuA.link() == imuB.link() &&
   // 	 imuA.processor() == imuB.processor()) {// &&
   //     //imuA.dataword() == imuB.dataword()) {
   //     found = true;
   //     break;
   //   }
   // }//B-itr



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

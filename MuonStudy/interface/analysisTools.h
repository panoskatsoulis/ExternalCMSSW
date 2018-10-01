#ifndef ANALYSIS_TOOLS_H
#define ANALYSIS_TOOLS_H

//C++ includes
#include <cmath>
#include <time.h>

//ROOT includes
#include "TChain.h"
#include "TList.h"
#include "TIterator.h"
#include "TCanvas.h"
//#include "TH1.h"

//analysis Includes
//include "analysisObj.hpp"

namespace analysisTools {
  //definition of the functions
  std::string toHex(const long int);

  //implementation of the functions
  void printRun(const int run, int& prevRun) {
    if (prevRun != run) {
      std::cout << "Run Number: " << run << std::endl;
      prevRun = run;
    }
  }

  float etaFromBmtf(const int& bmtfEta) {
    return bmtfEta*0.010875;
  }

  float phiFromBmtf(const int& hwPhi, const int& proc) {
    float phi=proc*48+hwPhi;
    phi=phi/576*6.28-3.14*15/180;
    if (phi>3.14) phi=phi-6.28;
    
    return phi;
  }
  
  std::string trAddrFromBmtf(const int& muon, const std::vector<int>& hwTrAddr) {
    std::string trAddr = "";
    for (int i = 0; i < 4; i++)
      trAddr = trAddr + toHex(hwTrAddr[4*muon+i]);
    
    return trAddr;
  }

  bool testBmtfTrAddr(const int& muon, const std::vector<int>& hwTrAddr, const std::string& test) {
    bool result;
    for (int i = 0; i < 4; i++) {
      if (test[i] == toHex(hwTrAddr[4*muon+i]) || test[i] == 'X')
	result = true;
      else {
	result = false;
	break;
      }
    }
    
    return result;
  }

  /*
  TChain makeFileChain(const std::string treePath, std::string files) {
    TChain newChain(treePath.c_str());
    newChain.Add(files.c_str());
    return newChain;
  }
  */

  std::string toHex(const long int num) {
    std::string hex;
    if (num < 10) return std::to_string(num);
    else if (num == 10) return "A";
    else if (num == 11) return "B";
    else if (num == 12) return "C";
    else if (num == 13) return "D";
    else if (num == 14) return "E";
    else return "F";
  }

  /* TCanvas* makeCanvas(const TList& list, std::string canvName, const bool bldLeg = true) { */
  /*   TCanvas *canvas = new TCanvas(canvName.c_str(),("canvas: "+canvName).c_str(), 500, 500); */
  /* } */

  TCanvas* makeCanvas(const TList& list, std::string canvName, const bool bldLeg = true) {
    TCanvas *canvas = new TCanvas(canvName.c_str(),("canvas: "+canvName).c_str(), 500, 500);
    canvas -> cd();

    int color = 0;
    for (auto itr: list) {
      TH1 *hist = (TH1*)itr;
      hist -> Draw("same");
      hist -> SetLineColor(++color);
      hist -> SetStats(kFALSE);
    }

    /*//Alternative way, it destroys the inserted TList to get the correct TH1
    while (list.Last() != 0) {
      TH1F *hist = (TH1F*)list.Last();
      hist -> Draw("same");
      hist -> SetLineColor(++color);//first change color then return

      list.RemoveLast();//delete the last object of the list
    }
    */

    //TPaveLabel *title = new TPaveLabel(0.,30.,10.,10., canvTitle.c_str(),"brndc");
    //title -> Draw("same");

    if (bldLeg)
      canvas -> BuildLegend();

    return canvas;
  }
  
  float deltaR(const std::vector<float> obj1,
	       const std::vector<float> obj2){//vector fashion <phi,eta>
    return sqrt(pow(obj2[1]-obj1[1],2)+pow(obj2[2]-obj1[2],2));
  }
  
  void normHistograms(TList& list, const TH1& refHist) {
    
    int bins = refHist.GetNbinsX();

    for (auto listObj: list) {
      TH1 *hist = (TH1*)listObj;
      for (int bin = 0; bin < bins; bin++) {
	if (refHist.GetBinContent(bin) == 0)
	  continue;
	hist->SetBinContent(bin, hist->GetBinContent(bin)/refHist.GetBinContent(bin));
      }
    }//hists

  }

  TFile* write2File(TList& list, const std::string& name = "newFile.root"){
    TFile *file = new TFile(name.c_str(), "RECREATE");
    for (auto listObj: list)
      listObj -> Write(listObj -> GetName());
    
    file -> Close();
    return file;
  }

  void printEvent(const ULong_t& event, const ULong_t& freq, const ULong_t eventsPerSec = 40*pow(10.,6)) {
    if (event == 1 || event%freq == 0) { //Event number and System time
      //time_t *rawTime;
      //time(rawTime);
      //struct tm *printableTime = localtime(rawTime);
      std::cout << "Processing entry " << event << ", System Time: -" << std::endl;//asctime(printableTime);
    }
    if (event%eventsPerSec == 0) //Experiment time related to the Events
      std::cout << "\t(" << event/eventsPerSec << " sec)" << std::endl;
  }

  void printRate(const ULong_t& event, const ULong_t& count, const ULong_t& freq = 10*pow(10.,6),
		 const float LHCfactor = 40*pow(10.,6), const std::string info = "Rate") {
    if (event%freq == 0)//if LHC operate at less than 40 MHz, then the second holds less events
      std::cout << info << " = " << LHCfactor*count/event << std::endl;
  }

}//namespace

//Examples of usage
  /*
  //print trAddress information for this muon
  std::cout << "trAddr: ";	
  for (int n=0; n<4; n++)
  std::cout << analysisTools::toHex(data.bmtftrAddress[n]);
  std::cout << std::endl;
  */

#endif

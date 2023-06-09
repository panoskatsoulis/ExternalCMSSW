// -*- C++ -*-
//
// Package:    ExternalCMSSW/eventFLTR
// Class:      eventFLTR
//
/**\class eventFLTR eventFLTR.cc ExternalCMSSW/eventFLTR/plugins/eventFLTR.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Panos Katsoulis
//         Created:  Wed, 12 May 2021 09:38:18 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "L1Trigger/L1TMuon/interface/RegionalMuonRawDigiTranslator.h"

//
// class declaration
//

class eventFLTR : public edm::stream::EDFilter<> {
public:
  explicit eventFLTR(const edm::ParameterSet&);
  ~eventFLTR();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginStream(edm::StreamID) override;
  virtual bool filter(edm::Event&, const edm::EventSetup&) override;
  virtual void endStream() override;

  //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------
  edm::EDGetTokenT<l1t::RegionalMuonCandBxCollection> token_bmtfData;
  edm::EDGetTokenT<l1t::RegionalMuonCandBxCollection> token_bmtfEmu;
  bool eventFound = false;
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
eventFLTR::eventFLTR(const edm::ParameterSet& iConfig) {
  //now do what ever initialization is needed
  token_bmtfData = consumes< l1t::RegionalMuonCandBxCollection >(iConfig.getParameter<edm::InputTag>("muonsData"));
  token_bmtfEmu = consumes< l1t::RegionalMuonCandBxCollection >(iConfig.getParameter<edm::InputTag>("muonsEmu"));
}

eventFLTR::~eventFLTR() {
  // do anything here that needs to be done at destruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
}

//
// member functions
//

// ------------ method called on each new Event  ------------
bool eventFLTR::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;

  //Muons Collections
  edm::Handle<l1t::RegionalMuonCandBxCollection> bmtfDataMuons;
  iEvent.getByToken(token_bmtfData,bmtfDataMuons);
  edm::Handle<l1t::RegionalMuonCandBxCollection> bmtfEmuMuons;
  iEvent.getByToken(token_bmtfEmu,bmtfEmuMuons);

  // continue if event didnt found only
  if (eventFound) return false;

  // only events with 1 muon
  if (not (bmtfDataMuons->size()==1 or bmtfEmuMuons->size()==1))
    return false;

  // other selection
  auto dataMuon = bmtfDataMuons->begin(); 
  auto emuMuon = bmtfEmuMuons->begin(); 

  // if ( not (dataMuon->hwPt() != emuMuon->hwPt() and dataMuon->hwPtUnconstrained() == emuMuon->hwPtUnconstrained() and dataMuon->hwDXY() == emuMuon->hwDXY()) )
  //   return false;
  // if ( not (dataMuon->hwPt() == emuMuon->hwPt() and dataMuon->hwPtUnconstrained() != emuMuon->hwPtUnconstrained() and dataMuon->hwDXY() == emuMuon->hwDXY()) )
  //   return false;
  // if ( not (dataMuon->hwPt() == emuMuon->hwPt() and dataMuon->hwPtUnconstrained() == emuMuon->hwPtUnconstrained() and dataMuon->hwDXY() != emuMuon->hwDXY()) )
  //   return false;

  // if ( not (dataMuon->hwPt() != emuMuon->hwPt() and dataMuon->hwPtUnconstrained() != emuMuon->hwPtUnconstrained() and dataMuon->hwDXY() == emuMuon->hwDXY()) )
  //   return false;
  // if ( not (dataMuon->hwPt() == emuMuon->hwPt() and dataMuon->hwPtUnconstrained() != emuMuon->hwPtUnconstrained() and dataMuon->hwDXY() != emuMuon->hwDXY()) )
  //   return false;
  // if ( not (dataMuon->hwPt() != emuMuon->hwPt() and dataMuon->hwPtUnconstrained() == emuMuon->hwPtUnconstrained() and dataMuon->hwDXY() != emuMuon->hwDXY()) )
  //   return false;

  // if ( not (dataMuon->hwPt() != emuMuon->hwPt() and dataMuon->hwPtUnconstrained() != emuMuon->hwPtUnconstrained() and dataMuon->hwDXY() != emuMuon->hwDXY()) )
  //   return false;
  // if ( not (dataMuon->hwPt() == emuMuon->hwPt() and dataMuon->hwPtUnconstrained() == emuMuon->hwPtUnconstrained() and dataMuon->hwDXY() == emuMuon->hwDXY()) )
  //   return false;

  // if ( dataMuon == emuMuon )
  //   return false;

  if ( not (dataMuon->hwEta() == emuMuon->hwEta()) )
    return false;

  cout << hex << setw(8) << setfill('0')
       << "data word = " << dataMuon->dataword() << "\t emu word = " << emuMuon->dataword() << dec << endl; 
  cout << "data PT = " << dataMuon->hwPt() << "\t emu PT = " << emuMuon->hwPt() << endl;
  cout << "data PT2 = " << dataMuon->hwPtUnconstrained() << "\t emu PT2 = " << emuMuon->hwPtUnconstrained() << endl;
  cout << "data DXY = " << dataMuon->hwDXY() << "\t emu DXY = " << emuMuon->hwDXY() << endl;
  cout << "data PHI = " << dataMuon->hwPhi() << "\t emu PHI = " << emuMuon->hwPhi() << endl;
  cout << "data ETA = " << dataMuon->hwEta() << "\t emu ETA = " << emuMuon->hwEta() << endl;
  cout << "data HF = " << dataMuon->hwHF() << "\t emu HF = " << emuMuon->hwHF() << endl;
  cout << "data SIGN = " << dataMuon->hwSign() << "\t emu SIGN = " << emuMuon->hwSign() << endl;
  cout << "data SIGNVALID = " << dataMuon->hwSignValid() << "\t emu SIGN = " << emuMuon->hwSignValid() << endl;
  cout << "data QUAL = " << dataMuon->hwQual() << "\t emu QUAL = " << emuMuon->hwQual() << endl;
  cout << "data LINK = " << dataMuon->link() << "\t emu LINK = " << emuMuon->link() << endl;
  cout << "data IDX = " << dataMuon->muIdx() << "\t emu IDX = " << emuMuon->muIdx() << endl;
  cout << "data PROC = " << dataMuon->processor() << "\t emu PROC = " << emuMuon->processor() << endl;
  // cout << "data TF = " << dataMuon->trackFinderType() << "\t emu TF = " << emuMuon->trackFinderType() << endl;
  auto data_trkadd = dataMuon->trackAddress();
  auto emu_trkadd = emuMuon->trackAddress();
  cout << "data TRKADD = ";
  for (int i=0; i < 7; i++) cout << data_trkadd.at(i);
  cout << "\t emu TRKADD = ";
  for (int i=0; i < 7; i++) cout << emu_trkadd.at(i);
  cout << endl;

  eventFound = true;
  return true;
}

// ------------ method called once each stream before processing any runs, lumis or events  ------------
void eventFLTR::beginStream(edm::StreamID) {
  // please remove this method if not needed
}

// ------------ method called once each stream after processing all runs, lumis and events  ------------
void eventFLTR::endStream() {
  // please remove this method if not needed
}

// ------------ method called when starting to processes a run  ------------
/*
void
eventFLTR::beginRun(edm::Run const&, edm::EventSetup const&)
{ 
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void
eventFLTR::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void
eventFLTR::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void
eventFLTR::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void eventFLTR::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(eventFLTR);

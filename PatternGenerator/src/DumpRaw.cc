// -*- C++ -*-
//
// Original Package:    EventFilter/L1TRawToDigi
// Class:      DumpRaw
//
/**\class DumpRaw DumpRaw.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Matthias Wolf
//         Created:  Mon, 10 Feb 2014 14:29:40 GMT
//
//

// system include files
#include <iostream>
#include <iomanip>
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/FEDRawData/interface/FEDHeader.h"
//#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "DataFormats/FEDRawData/interface/FEDTrailer.h"

#include "EventFilter/L1TRawToDigi/interface/AMC13Spec.h"
//#include "EventFilter/L1TRawToDigi/interface/Block.h"

#include "EventFilter/L1TRawToDigi/plugins/PackingSetupFactory.h"

#include "EventFilter/L1TRawToDigi/plugins/implementations_stage2/L1TStage2Layer2Constants.h"

namespace l1t {
  class DumpRaw : public edm::stream::EDProducer<> {
  public:
    explicit DumpRaw(const edm::ParameterSet&);
    ~DumpRaw() override;

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    void produce(edm::Event&, const edm::EventSetup&) override;

    void beginRun(edm::Run const&, edm::EventSetup const&) override{};
    void endRun(edm::Run const&, edm::EventSetup const&) override{};
    void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override{};
    void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override{};

    // ----------member data ---------------------------
    edm::EDGetTokenT<FEDRawDataCollection> fedData_;
    std::vector<int> fedIds_;
    unsigned int minFeds_;
    unsigned int fwId_;
    unsigned int dmxFwId_;
    bool fwOverride_;

    std::unique_ptr<PackingSetup> prov_;

    // header and trailer sizes in chars
    int slinkHeaderSize_;
    int slinkTrailerSize_;
    int amcHeaderSize_;
    int amcTrailerSize_;
    int amc13HeaderSize_;
    int amc13TrailerSize_;

    bool tmtCheck_;

    bool ctp7_mode_;
    bool mtf7_mode_;
    bool debug_;
    int warnsa_;
    int warnsb_;
  };
}  // namespace l1t

namespace l1t {
  DumpRaw::DumpRaw(const edm::ParameterSet& config)
      : fedIds_(config.getParameter<std::vector<int>>("FedIds")),
        minFeds_(config.getParameter<unsigned int>("MinFeds")),
        fwId_(config.getParameter<unsigned int>("FWId")),
        dmxFwId_(config.getParameter<unsigned int>("DmxFWId")),
        fwOverride_(config.getParameter<bool>("FWOverride")),
        tmtCheck_(config.getParameter<bool>("TMTCheck")),
        ctp7_mode_(config.getUntrackedParameter<bool>("CTP7")),
        mtf7_mode_(config.getUntrackedParameter<bool>("MTF7")) {
    fedData_ = consumes<FEDRawDataCollection>(config.getParameter<edm::InputTag>("InputLabel"));

    slinkHeaderSize_ = config.getUntrackedParameter<int>("lenSlinkHeader");
    slinkTrailerSize_ = config.getUntrackedParameter<int>("lenSlinkTrailer");
    amcHeaderSize_ = config.getUntrackedParameter<int>("lenAMCHeader");
    amcTrailerSize_ = config.getUntrackedParameter<int>("lenAMCTrailer");
    amc13HeaderSize_ = config.getUntrackedParameter<int>("lenAMC13Header");
    amc13TrailerSize_ = config.getUntrackedParameter<int>("lenAMC13Trailer");

    debug_ = config.getUntrackedParameter<bool>("debug");
    warnsa_ = 0;
    warnsb_ = 0;
    std::cout << "finished constructor" << std::endl;
  }

  DumpRaw::~DumpRaw() {}

  //
  // member functions
  //

  // ------------ method called to produce the data  ------------
  void DumpRaw::produce(edm::Event& event, const edm::EventSetup& setup) {
    using namespace edm;

    edm::Handle<FEDRawDataCollection> feds;
    event.getByToken(fedData_, feds);

    if (!feds.isValid()) {
      LogError("L1T") << "Cannot unpack: no FEDRawDataCollection found";
      return;
    }

    unsigned valid_count = 0;
    for (const auto& fedId : fedIds_) {
      const FEDRawData& l1tRcd = feds->FEDData(fedId);

      if ((int)l1tRcd.size() < slinkHeaderSize_ + slinkTrailerSize_ + amc13HeaderSize_ + amc13TrailerSize_ +
                                   amcHeaderSize_ + amcTrailerSize_)
        continue;
      else
	valid_count++;

      const unsigned char* data = l1tRcd.data();
      FEDHeader header(data);
      FEDTrailer trailer(data + (l1tRcd.size() - slinkTrailerSize_));

      // FIXME Hard-coded firmware version for first 74x MC campaigns.
      // Will account for differences in the AMC payload, MP7 payload,
      // and unpacker setup.
      bool legacy_mc = fwOverride_ && ((fwId_ >> 24) == 0xff);

      amc13::Packet packet;
      if (!packet.parse((const uint64_t*)data,
                        (const uint64_t*)(data + slinkHeaderSize_),
                        (l1tRcd.size() - slinkHeaderSize_ - slinkTrailerSize_) / 8,
                        header.lvl1ID(),
                        header.bxID(),
                        legacy_mc,
                        mtf7_mode_)) {
        LogError("L1T") << "Could not extract AMC13 Packet.";
        return;
      }

      for (auto& amc : packet.payload()) {
        if (amc.size() == 0)
          continue;

        auto payload64 = amc.data();
        const uint32_t* start = (const uint32_t*)payload64.get();
        // Want to have payload size in 32 bit words, but AMC measures
        // it in 64 bit words → factor 2.
        const uint32_t* end = start + (amc.size() * 2);

        std::unique_ptr<Payload> payload;
        if (ctp7_mode_) {
          LogDebug("L1T") << "Using CTP7 mode";
          // CTP7 uses userData in AMC header
          payload = std::make_unique<CTP7Payload>(start, end, amc.header());
        } else if (mtf7_mode_) {
          LogDebug("L1T") << "Using MTF7 mode";
          payload = std::make_unique<MTF7Payload>(start, end);
        } else {
          LogDebug("L1T") << "Using MP7 mode";
          payload = std::make_unique<MP7Payload>(start, end, legacy_mc);
        }
        //unsigned fw = payload->getAlgorithmFWVersion();
        unsigned board = amc.blockHeader().getBoardID();
        unsigned amc_no = amc.blockHeader().getAMCNumber();
	std::cout << "----> Got new board " << board << " (amc:" << amc_no << ")" << std::endl;

	// kpanos
	int enabled_links[] = {0,1,2,3,4,5, 8,9,10,11,12,13, 16,17,18,19,20,21,22,23,24,25,26,27,28,29, 32,33,34,35};
	std::vector<std::string> frames;
	frames.resize(6,"");
	int prev_link = -1;
	//
        // getBlock() returns a non-null unique_ptr on success
        std::unique_ptr<Block> block;
        while ((block = payload->getBlock()).get()) { // loop links
	  block->amc(amc.header());
	  using namespace std;
	  //
	  auto link = block->header().getID()/2;
	  cout << "RAW link:" << block->header().getID() << endl;

	  for (int i_link=prev_link+1; i_link<72; i_link++) {
	    cout << "making link " << i_link << endl;
	    if ((int)link != i_link) { //fill default links
	      if (std::find(std::begin(enabled_links), std::end(enabled_links), i_link) != std::end(enabled_links)) {
		cout << "will write default link" << endl;
		frames[0] += "00200800";
		frames[1] += "00200800";
		frames[2] += "00200800";
		frames[3] += "00200800";
	      }
	      else {
		cout << "will write default link" << endl;
		frames[0] += "00000000";
		frames[1] += "00000000";
		frames[2] += "00000000";
		frames[3] += "00000000";
	      }
	      frames[4] += "00000000";
	      frames[5] += "00000000";
	      prev_link = i_link; // t know where is stopped for case that last "link" is < 72
	    }
	    else if (i_link < 61) { //fill link in hand with data
	      cout << "will write data link" << endl;
	      for (const auto& ibx : block->getBxBlocks((unsigned int)6, true)) {  //Bx iteration
		int bxNum = ibx.header().getBx();
		//Print out this BxBlock if input links
		if (bxNum == 0) {
		  cout << "BX " << bxNum << "\t link: " << link << endl
		       << hex;
		  int iw = 0;
		  for (auto word: ibx.payload()) {
		    cout << setw(8) << setfill('0') << word << endl;
		    stringstream hexstr;
		    hexstr << hex << setw(8) << setfill('0') << word;
		    cout << "hexstr" << iw << " " << hexstr.str() << endl;
		    frames[iw] += hexstr.str();
		    iw++;
		  }
		  cout << dec;
		}
	      }
	      prev_link = i_link;
	      break;
	    }
	    else { // this is for link 61 (the rest inks will be empty and filled by the loop below)
	      cout << "link 61 (OUTPUT)" << endl;
	      for (const auto& ibx : block->getBxBlocks((unsigned int)6, true))
		if (ibx.header().getBx() == 0)
		  for (auto word: ibx.payload()) cout << hex << setw(8) << setfill('0') << word << dec << endl;
	      break;
	    }
	  } // loop i_link	  
	} // loop unpacked links

	// add the rest empty links
	std::cout << "prev_link = " << prev_link << std::endl;
	for (int i_link=prev_link+1; i_link<72; i_link++) {
	  std::cout << "will write default link" << std::endl;
	  frames[0] += "00000000";
	  frames[1] += "00000000";
	  frames[2] += "00000000";
	  frames[3] += "00000000";
	  frames[4] += "00000000";
	  frames[5] += "00000000";
	}

	//// PRINT THE BLOCK ////////////////////////////////////////////////////////////////////////////////
	using namespace std;
	cout << "Printing 6 frames for the board " << (int)board << endl;
	for (auto frame: frames)
	  cout << "frame " << hex << frame << dec << endl;
      } // for board


    } // for feds
  }

  // ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
  void DumpRaw::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    // These parameters are part of the L1T/HLT interface, avoid changing if possible:
    desc.add<std::vector<int>>("FedIds", {})->setComment("required parameter:  default value is invalid");
    desc.add<std::string>("Setup", "")->setComment("required parameter:  default value is invalid");
    // These parameters have well defined  default values and are not currently
    // part of the L1T/HLT interface.  They can be cleaned up or updated at will:
    desc.add<unsigned int>("FWId", 0)->setComment(
        "Ignored unless FWOverride is true.  Calo Stage1:  32 bits: if the first eight bits are 0xff, will read the "
        "74x MC format.\n");
    desc.add<unsigned int>("DmxFWId", 0)
        ->setComment(
            "Ignored unless FWOverride is true.  Calo Stage1:  32 bits: if the first eight bits are 0xff, will read "
            "the 74x MC format.\n");
    desc.add<bool>("FWOverride", false)->setComment("Firmware version should be taken as FWId parameters");
    desc.add<bool>("TMTCheck", true)->setComment("Flag for turning on/off Calo Layer 2 TMT node check");
    desc.addUntracked<bool>("CTP7", false);
    desc.addUntracked<bool>("MTF7", false);
    desc.add<edm::InputTag>("InputLabel", edm::InputTag("rawDataCollector"));
    desc.addUntracked<int>("lenSlinkHeader", 8);
    desc.addUntracked<int>("lenSlinkTrailer", 8);
    desc.addUntracked<int>("lenAMCHeader", 8);
    desc.addUntracked<int>("lenAMCTrailer", 0);
    desc.addUntracked<int>("lenAMC13Header", 8);
    desc.addUntracked<int>("lenAMC13Trailer", 8);
    desc.addUntracked<bool>("debug", false)->setComment("turn on verbose output");
    desc.add<unsigned int>("MinFeds", 0)
        ->setComment("optional parameter:  warn if less than MinFeds non-empty FED ids unpacked.");
    descriptions.add("dumpRaw", desc);
  }
}  // namespace l1t

using namespace l1t;
//define this as a plug-in
DEFINE_FWK_MODULE(DumpRaw);

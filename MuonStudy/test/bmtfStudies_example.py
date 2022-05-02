import argparse
import ROOT##, math probably gonna be needed

## load root and required DataFormats
ROOT.gSystem.Load("libFWCoreFWLite.so")
ROOT.gSystem.Load("libDataFormatsFWLite.so")
ROOT.gROOT.SetBatch()        # don't pop up canvases
try:
    print("Running FWLiteEnabler::enable() ...", end=' ')
    ROOT.gROOT.ProcessLine("FWLiteEnabler::enable()")
    print("[ok]")
except:
    print("[error] check CMSSW environment")
    quit(1)

## parse arguments
parser = argparse.ArgumentParser()
parser.add_argument('--source', '-s', type=str, help='Comma separated list of files to create the cmssw source, or dataset for DBS query.')
parser.add_argument('--events', '-ev', default=-1, type=int, help='Number of events to run.')
parser.add_argument('--nfiles', type=int, help='# files to limit the DBS query.')
parser.add_argument('--site', type=str, help='unique site to use for the DBS query.')
parser.add_argument('--print-every', '-i', default=1, type=int, help='Print every this number of events.')
args = parser.parse_args()

## get files
if args.source.startswith('dbs:'):
    import os
    das_query = 'dasgoclient --query="file dataset='+args.source.strip('dbs:')
    if args.site:
        das_query += " site={}".format(args.site)
    das_query += '"'
    if args.nfiles:
        das_query += " -limit {}".format(args.nfiles)
    print("Will query for files with cmd:")
    print(das_query)
    query_out = os.popen(das_query)
    files = ['root://xrootd-cms.infn.it/'+_file.strip() for _file in query_out]
else:
    files = args.source.split(",")

print("Input files {}: {}".format(len(files),files))


## labels and handlers (MINIAODSIM)
from DataFormats.FWLite import Handle, Events
l1muons_label = ("gmtStage2Digis","Muon","RECO"); l1muons_handle = Handle('BXVector<l1t::Muon>')
## bxvec is actually a sophisticated vector, check this doxygen [1]
muons_label = ("slimmedMuons","","PAT"); muons_handle = Handle('std::vector<pat::Muon>')

## initialize the Events
events = Events(files) # here [2] for more

##------------------- analysis

## before evnt loop (TH1 class [5])
l1Muon_pt = ROOT.TH1F("l1Muon_pt", "L1 Muon Pt", 50, 0, 100) ## l1 pt in HW value (1 value/ 0.5 GeV)
muon_pt = ROOT.TH1F("muon_pt", "Offline Muon Pt", 50, 0, 50) ## offline pt in GeV

### event loop
for i, iEv in enumerate(events):
    if i % args.print_every == 0:
        print( "Running on event {} ...".format(i) )
    
    ## get items for the edm file
    try:
        iEv.getByLabel(l1muons_label, l1muons_handle); l1muons = l1muons_handle.product()
        iEv.getByLabel(muons_label, muons_handle);  muons = muons_handle.product()
    except:
        raise RuntimeError("Unable to initalize event handlers")
    ## ---------------------------

    ## make calculations
    for mu in l1muons:
        mu.setDebug(True)
        # mu is l1t::Muon [3]
        l1Muon_pt.Fill(mu.hwPt())
        print( "eta {}, tfMuonIndex {}, rank {}".format(mu.etaAtVtx(),mu.tfMuonIndex(), mu.hwRank()) )

    for mu in muons:
        # mu is PAT::Muon [4]
        muon_pt.Fill(mu.pt())
    ## ---------------------------

    if i > 10: break

## actions after the event loop
# make a canvas [6], draw, and save it
# otherwise make a TFile [7] to save multiple plots
c1 = ROOT.TCanvas(); c1.cd()
l1Muon_pt.Draw()
c1.Print("l1Muon_pt.png")
muon_pt.Draw() # overwrites
c1.Print("muon_pt.png")

'''
[1] https://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_12_3_1/doc/html/d4/d3e/classBXVector.html
[2] https://github.com/jmejiagu/MyFWLite_12/blob/main/examples/Jpsi_peak.py#L9-L37
[3] https://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_12_3_1/doc/html/d7/d17/classl1t_1_1Muon.html
[4] https://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_12_3_1/doc/html/d6/d13/classpat_1_1Muon.html
[5] https://root.cern.ch/doc/master/classTH1.html
[6] https://root.cern.ch/doc/master/classTCanvas.html
[7] https://root.cern.ch/doc/master/classTFile.html
'''

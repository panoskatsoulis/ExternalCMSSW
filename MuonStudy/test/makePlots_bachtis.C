#include "analysisTools.hpp"

TCanvas* canvas_data_mc(TH1*, TH1*, TString, bool,
			double, double, double, double,
			TString, TString);

void make(TFile* file, TString fileDir = "BMTF", bool makePng = false, TString run = "1", TString dir = ".") {

  //file->cd(fileDir);

  TH1F* phisData = (TH1F*)file->Get(fileDir+"/phisData");
  TH1F* etasData = (TH1F*)file->Get(fileDir+"/etasData");
  TH1F* ptData = (TH1F*)file->Get(fileDir+"/ptData");
  TH1F* pt2Data = (TH1F*)file->Get(fileDir+"/pt2Data");
  TH1I* dxyData = (TH1I*)file->Get(fileDir+"/dxyData");

  TH1F* phisEmu = (TH1F*)file->Get(fileDir+"/phisEmu");
  TH1F* etasEmu = (TH1F*)file->Get(fileDir+"/etasEmu");
  TH1F* ptEmu = (TH1F*)file->Get(fileDir+"/ptEmu");
  TH1F* pt2Emu = (TH1F*)file->Get(fileDir+"/pt2Emu");
  TH1I* dxyEmu = (TH1I*)file->Get(fileDir+"/dxyEmu");
  TH2I* size = (TH2I*)file->Get(fileDir+"/bmtfSize");

  TH1I* dataFineBit = (TH1I*)file->Get(fileDir+"/fineBitData");
  TH1I* emuFineBit = (TH1I*)file->Get(fileDir+"/fineBitEmu");

  TH2F* phis = (TH2F*)file->Get(fileDir+"/muonPhis");
  TH2F* etas = (TH2F*)file->Get(fileDir+"/muonEtas");
  TH2F* pt = (TH2F*)file->Get(fileDir+"/muonPt");
  TH2F* pt2 = (TH2F*)file->Get(fileDir+"/muonPt2");

  TCanvas *canv1 = canvas_data_mc(phisData, phisEmu, "#phi", true, -20, 80, 0, 1000, "", run);
  TCanvas *canv2 = canvas_data_mc(etasData, etasEmu, "#eta", true, -140, 140, 0, 1000, "", run);
  TCanvas *canv3 = canvas_data_mc(ptData, ptEmu, "Pt", true, 0, 300, 0, 1000, "", run);
  TCanvas *canv4 = canvas_data_mc(pt2Data, pt2Emu, "Pt_{2}", true, 0, 300, 0, 1000, "", run);
  TCanvas *canv5 = canvas_data_mc(dxyData, dxyEmu, "dxy", true, 0, 3, 0, 1000, "", run);//check maxy

  // TLegend *leg;
  // leg = new TLegend(0.2, 0.6, 0.3, 0.75);
  // leg->SetHeader( "CMS Internal | Run "+run, "C");
  // leg->AddEntry(phisData,TString("Muons = ")+TString(std::to_string((int)phisData->GetEntries()).c_str()), "" );
  // leg->SetTextFont(42);
  // leg->SetTextSizePixels(14);
  // leg->SetFillColor(kWhite);
  // leg->SetLineColor(kWhite);
  // leg->SetBorderSize(0);

  gStyle->SetOptStat(0);
  gPad->SetRightMargin(0.3);
  TCanvas *sizeCanv = new TCanvas("muonSize", "size", 550, 550);
  sizeCanv->cd(); size->Draw("COLZTEXT");// leg->Draw();
  size->GetYaxis()->SetTitleOffset(1.3);
  size->GetXaxis()->SetTitle("Data");
  size->GetYaxis()->SetTitle("Emulator");

  TCanvas *phisCanv = new TCanvas("phis", "phis", 550, 550);
  phisCanv->cd(); phis->Draw("COLZ");// leg->Draw();
  phis->GetYaxis()->SetTitleOffset(1.3);
  phis->GetXaxis()->SetTitle("Data");
  phis->GetYaxis()->SetTitle("Emulator");

  TCanvas *etasCanv = new TCanvas("etas", "etas", 550, 550);
  etasCanv->cd(); etas->Draw("COLZ");// leg->Draw();
  etas->GetYaxis()->SetTitleOffset(1.3);
  etas->GetXaxis()->SetTitle("Data");
  etas->GetYaxis()->SetTitle("Emulator");

  TCanvas *ptCanv = new TCanvas("pt", "pt", 550, 550);
  ptCanv->cd(); pt->Draw("COLZ");// leg->Draw();
  pt->GetYaxis()->SetTitleOffset(1.3);
  pt->GetXaxis()->SetTitle("Data");
  pt->GetYaxis()->SetTitle("Emulator");

  TCanvas *pt2Canv = new TCanvas("pt2", "pt2", 550, 550);
  pt2Canv->cd(); pt2->Draw("COLZ");// leg->Draw();
  pt2->GetYaxis()->SetTitleOffset(1.3);
  pt2->GetXaxis()->SetTitle("Data");
  pt2->GetYaxis()->SetTitle("Emulator");

  // TCanvas *fbCanvData = new TCanvas("fbCanvData", "fbData", 550, 550);
  // fbCanvData->cd(); 
  // dataFineBit->Draw();
  // dataFineBit->GetYaxis()->SetRangeUser(0, 30000);
  // fbCanvData->SetGridy(true);

  // TCanvas *fbCanvEmu = new TCanvas("fbCanvEmu", "fbEmu", 550, 550);
  // fbCanvEmu->cd();
  // emuFineBit->Draw();  
  // emuFineBit->GetYaxis()->SetRangeUser(0, 30000);
  // fbCanvEmu->SetGridy(true);

  // dataFineBit->GetXaxis()->SetTitle("fineEta");
  // emuFineBit->GetXaxis()->SetTitle("fineEta");

  // TList fbList;
  // fbList.Add(dataFineBit);
  // dataFineBit->GetYaxis()->SetRangeUser(0.1, 30000);
  // fbList.Add(emuFineBit);
  // emuFineBit->GetYaxis()->SetRangeUser(0.1, 30000);
  // TCanvas *fbCanv = analysisTools::makeCanvas(fbList, "fineBits_superimposed");
  // gPad->SetGridy(true);

  //PrintOuts
  std::cout << "# muons data: " << ptData->GetEntries() << std::endl;
  std::cout << "# muons emu: " << ptEmu->GetEntries() << std::endl;

  //Images
  if (makePng) {
    TImage *img = TImage::Create();
    img->FromPad(canv1);
    img->WriteImage(dir+"/compare_phi.png");
    img->FromPad(canv2);
    img->WriteImage(dir+"/compare_eta.png");
    img->FromPad(canv3);
    img->WriteImage(dir+"/compare_pt.png");
    img->FromPad(canv4);
    img->WriteImage(dir+"/compare_pt2.png");
    img->FromPad(canv5);
    img->WriteImage(dir+"/compare_dxy.png");
    img->FromPad(sizeCanv);
    img->WriteImage(dir+"/MuonsComparison.png");
    img->FromPad(phisCanv);
    img->WriteImage(dir+"/phiscomparison.png");
    img->FromPad(etasCanv);
    img->WriteImage(dir+"/etascomparison.png");
    img->FromPad(ptCanv);
    img->WriteImage(dir+"/pTcomparison.png");
    img->FromPad(pt2Canv);
    img->WriteImage(dir+"/pT2comparison.png");
    // img->FromPad(fbCanvData);
    // img->WriteImage(dir+"/"+dataFineBit->GetTitle()+".png");
    // img->FromPad(fbCanvEmu);
    // img->WriteImage(dir+"/"+emuFineBit->GetTitle()+".png");
  }

}//make

TCanvas* canvas_data_mc(TH1 * data_xs, TH1 * mc_xs, TString canvas, bool Logy,
			double minx, double maxx, double miny, double maxy,
			TString Labelx, TString RUN)
{
  TCanvas *data_mc_canvas = new TCanvas("compare_"+canvas, "compare_"+canvas, 550, 550);

  gStyle->SetPadBorderMode(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);
  
  //TPad *pad1 = new TPad("pad1","This is pad1",0.,0.40,1.,1.);
  //  TPad *pad2 = new TPad("pad2","This is pad2",0.,0.,1.,0.40);
  //pad1->SetFillColor(kWhite);
  //  pad2->SetFillColor(kWhite);
  //pad1->Draw();
  //  pad2->Draw();

  //pad1->cd();

  data_xs->Draw("P");
  mc_xs->Draw("same");

  //gPad->SetBottomMargin(0);
  //gPad->SetLeftMargin(0.1); gPad->SetRightMargin(0.06);
  //gPad->SetTickx();
  if(Logy) gPad->SetLogy();

  data_xs->GetYaxis()->SetTitle("#scale[1.4]{Muons}");
  data_xs->GetYaxis()->SetTitleOffset(1.4);
  data_xs->GetXaxis()->SetTitle("#scale[1.4]{"+canvas+"(#mu)}");
  data_xs->GetXaxis()->SetTitleOffset(1.1);

  //data style
  data_xs->SetMarkerColor(1);
  data_xs->SetLineColor(1);
  data_xs->SetMarkerSize(0.4);
  data_xs->SetMarkerStyle(2);

  //mc style
  mc_xs->SetLineColor(kRed);

  // pad2->cd();
                                                                          
  // gPad->SetTopMargin(0);
  // gPad->SetBottomMargin(0.2);
  // gPad->SetLeftMargin(0.1); gPad->SetRightMargin(0.06);
  // gPad->SetTickx();
  // gPad->SetGridy(true);

  // //Ratio
  // TH1F *data = (TH1F*)data_xs->Clone();
  // TH1F *emu = (TH1F*)mc_xs->Clone();
  // TH1F *ratio = new TH1F();
  // ratio -> SetBins(data_xs->GetNbinsX(),
  // 		   data_xs->GetXaxis()->GetXmin(), data_xs->GetXaxis()->GetXmax());
    
  // ratio -> SetTitle("Ratio");
  // data->Sumw2();
  // emu->Sumw2();

  // ratio -> Divide(data, emu, 1., 1., "B");

  // ratio->GetYaxis()->SetTitleOffset(0.8);
  
  // ratio->SetMaximum(1.5);
  // ratio->SetMinimum(0.5);
  // ratio->SetMarkerStyle(7);
  // ratio->SetMarkerSize(0.4);
  // ratio->SetMarkerColor(kBlack);

  // ratio->GetYaxis()->SetTitle("#frac{data}{emu}");
  // ratio->GetXaxis()->SetTitle(canvas+"(#mu_{BMTF})");

  // ratio->GetXaxis()->SetTitleSize(0.05);
  // ratio->GetXaxis()->SetLabelSize(0.05);
  // ratio->GetYaxis()->SetLabelSize(0.05);
  // ratio->GetYaxis()->SetTitleSize(0.05);

  // ratio->GetXaxis()->SetRangeUser(minx,maxx);

  // //ratio->GetXaxis()->SetTitleSize(0.1);
  // //ratio->GetYaxis()->SetTitleSize(0.1);

  // ratio->Draw("P");
  // //ratio->GetYaxis()->SetRangeUser(0.02,2.0);

  TLegend *leg;
  leg = new TLegend(0.75, 0.7, 0.95, 0.95);
  leg->SetHeader("#splitline{CMS Internal}{Run "+RUN+'}');
  leg->AddEntry(mc_xs,"Emulator");
  leg->AddEntry(data_xs,"Data");
  leg->SetTextFont(42);
  leg->SetFillColor(kWhite);
  leg->SetLineColor(kWhite);
  leg->SetBorderSize(0);
  leg->SetTextSizePixels(22);
  //pad1->cd();
  leg->Draw();

  return data_mc_canvas;
}

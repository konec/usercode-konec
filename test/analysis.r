{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("~/root/FigUtils.C");
  MyDefaultStyle->cd();
  gROOT.LoadMacro("~/root/RegionLabels.C");

  
//--
  TFile file("analysis.root");
  file.ls();


/*--------------------
  MyDefaultStyle->cd();
  cNumHP= new TCanvas("cNumHP","HP multiplicity",-2);
  gPad->SetLeftMargin(0.14);
  MyDefaultStyle->cd(); gPad->SetLogy();
  TH1D * h = hNumHP->ProjectionX("h","E");
  RegionLabels(h);
  h->SetYTitle("Number of seeds");
  h->SetTitleOffset(1.4,"y");
  h->SetMinimum(0.1);
  h->SetMarkerSize(1.7);
  h->SetMarkerColor(4);
  h->SetMarkerStyle(4);
  //h->DrawCopy("HIST P");
  h->DrawCopy("P");
//  h->Print("all");
  cNumHP->Print(0,".eps");
  h->Delete();
*/

//--------------------
  MyDefaultStyle->cd();
  cCPU = new TCanvas("cCPU","Timing...",-2);
  MyDefaultStyle->cd();
  gPad->SetLeftMargin(0.17);
  TH1D * h = hCPU->ProjectionX("h","E");
  RegionLabels(h);
  h->SetYTitle("real time [s]");
  h->SetTitleOffset(1.8,"y");
  h->SetMaximum(0.001);
  h->SetMinimum(0.);
  h->SetMarkerSize(1.7);
  h->SetMarkerColor(4);
  h->SetLineWidth(4);
  h->SetMarkerStyle(4);
  h->DrawCopy("HIST  P");
  h->SetMarkerStyle(5);
  h->DrawCopy("HIST  P SAME");
  h->Print("all");
  cCPU->Print(0,".eps");
  h->Delete();

//--------------------

  MyDefaultStyle->cd();
  cEfficPt = new TCanvas("cEfficPt","cEfficPt",-2);
  gPad->SetGrid(1,0);
  TH1D * heff = DivideErr(hEffPt_N,hEffPt_D,"hEffic","B");
  TH1D * heffAlgo = DivideErr(hEffAlgoPt_N,hEffAlgoPt_D,"hEfficAlgo","B");
  heff->SetMinimum(0.00);
  heff->SetMaximum(1.01);
  heff->SetXTitle("p_{T} [GeV/c]");
  heff->SetYTitle("efficiency");
  heff->SetMarkerStyle(25);
  heff->SetMarkerColor(4);
//  heff->GetXaxis()->CenterLabels(true);
  heff->DrawCopy("E");
  heffAlgo->SetMarkerColor(2);
  heffAlgo->SetMarkerStyle(20);
  cout <<"-------------------  effic (over): " << heff->GetBinContent(51)<<endl;
  cout <<"-------------------  effic (algo): " << heffAlgo->GetBinContent(51)<<endl;
  heffAlgo->DrawCopy("E same");
//  heffAlgo->Print("all");
  lab = new TLegend(0.55,0.16,0.86,0.33);
  lab->AddEntry(heffAlgo, "algorithmic");
  lab->AddEntry(heff, "effective");
  lab->Draw();
  cEfficPt.Print(0,".eps");

//--------------------

  MyDefaultStyle->cd();
  cEfficReg = new TCanvas("cEfficReg","cEfficReg",-2);
  gPad->SetGrid(1,0);
  TH1D * h= DivideErr(hEffReg_N,hEffReg_D,"hEffReg","B");
  h->SetMinimum(0.00);
  h->SetMaximum(1.01);
  RegionLabels(h);
  h->SetYTitle("efficiency");
  h->SetMarkerSize(1.7);
  h->SetMarkerColor(4);
  h->SetLineWidth(4);
  h->SetMarkerStyle(4);
  h->DrawCopy("HIST  P");
  h->SetMarkerStyle(5);
  h->DrawCopy("HIST  P SAME");
  cEfficReg->Print(0,".eps");
  h->Delete();
//--------------------


goto end;

  MyDefaultStyle->cd();
  gStyle->SetOptStat(111111);
  cEtaDiff = new TCanvas("cEtaDiff","cEtaDiff",-2);
  hEtaDiff->DrawCopy();
  cEtaDiff.Print(0,".eps");



  MyDefaultStyle->cd();
  cEfficEta = new TCanvas("cEfficEta","cEfficEta",-2);
//  hEffEta_N->Rebin(4);
//  hEffEta_D->Rebin(4);
  TH1D * heffEta = DivideErr(hEffEta_N,hEffEta_D,"hEffic","B");
  heffEta->SetMinimum(0.5);
  heffEta->SetMaximum(1.02);
  heffEta->SetXTitle("eta ");
  heffEta->SetYTitle("efficiency");
  heffEta->DrawCopy("E");
  cEfficEta.Print(0,".eps");
  
end:

    
}

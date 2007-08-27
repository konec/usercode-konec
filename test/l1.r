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


//--------------------
  MyDefaultStyle->cd();
  cPhiDiff= new TCanvas("cPhiDiff","HP multiplicity",-2);
  gPad->SetLeftMargin(0.14);
  gStyle->SetOptStat(111111);
  hPhiDiff->SetXTitle("#Delta #varphi");
  hPhiDiff->DrawCopy("");
  cPhiDiff.Print(0,".eps");

  MyDefaultStyle->cd();
  cEtaDiff= new TCanvas("cEtaDiff","HP multiplicity",-2);
  gStyle->SetOptStat(1111);
  gPad->SetLeftMargin(0.14);
  MyDefaultStyle->cd(); // gPad->SetLogy();
  hEtaDiff->SetXTitle("#Delta #eta");
  hEtaDiff->SetYTitle("arbitrary");
  hEtaDiff->DrawCopy("");
  cEtaDiff.Print(0,".eps");
  
  MyDefaultStyle->cd();
  cPt= new TCanvas("cPt","HP multiplicity",-2);
  gPad->SetLeftMargin(0.10);
  MyDefaultStyle->cd(); // gPad->SetLogy();
  hPt->SetXTitle("reconstructed p_T ");
  hPt->DrawCopy("");
  cPt.Print(0,".eps");
  
  MyDefaultStyle->cd();
  cCPU= new TCanvas("cCPU","HP multiplicity",-2);
  gPad->SetLeftMargin(0.10);
  MyDefaultStyle->cd(); // gPad->SetLogy();
  gStyle.SetOptStat(111111);
  hCPU->SetXTitle("cpu timer (s) ");
  hCPU->DrawCopy("");
  cCPU.Print(0,".eps");
  
end:

    
}

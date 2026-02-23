#include <TCanvas.h>
#include <TPad.h>
#include <TH1.h>
#include <TLegend.h>
#include "/data4/YuSiang/personalLib/Style/tdrstyle.h"
// #include "/data4/YuSiang/personalLib/Letter/Letter.h"

void SetMPadHis(TVirtualPad *PadM,TH1* hM,const string option){
  hM->GetXaxis()->SetTitleOffset(99);
  hM->GetXaxis()->SetLabelOffset(99);
  hM->GetYaxis()->SetTitleSize(0.05);
  hM->GetYaxis()->SetTitleOffset(1.2);
  hM->GetYaxis()->SetLabelSize(0.05);
  PadM->cd();
  hM->Draw(option.data());
}
void SetSPadHis(TVirtualPad *PadS,TH1* hS,const string option, const int EQ1Color = 3,const string optionOther = ""){
  hS->GetXaxis()->SetTitleOffset();
  float ScaleRatio = 0.7/0.3;
  hS->GetYaxis()->SetTitle("Data/MC");
  hS->GetXaxis()->SetTitleSize(0.05*ScaleRatio);
  hS->GetXaxis()->SetLabelSize(0.05*ScaleRatio);
  hS->GetYaxis()->SetTitleSize(0.05*ScaleRatio);
  hS->GetYaxis()->SetTitleOffset(1.2/ScaleRatio);
  hS->GetYaxis()->SetLabelSize(0.05*ScaleRatio);
  hS->GetXaxis()->SetLabelOffset(0);
  hS->GetYaxis()->SetNdivisions(505);
  hS->GetYaxis()->SetRangeUser(-.2,2.2);
  PadS->cd()->SetGrid(0,1);
  hS->Draw(option.data());
  if(int(optionOther.find("eq"))!=-1){
    double xMin = hS->GetXaxis()->GetXmin();
    double xMax = hS->GetXaxis()->GetXmax();
    cout<<xMin<<"\t"<<xMax<<endl;
    TH1F *heq1 = new TH1F(Form("heq1%s",hS->GetName()),"",1,xMin,xMax);
    heq1->Fill(1);
    heq1->SetLineStyle(2);
    heq1->SetLineWidth(2);
    heq1->SetLineColor(EQ1Color);
    heq1->Draw("h][same");
  }
}
void SetMSDiv(TVirtualPad *c){
  c->Divide(1, 2);
  c->cd()->SetPad(0,0,1,1);
  c->cd()->SetMargin(0,0,0,0);
  c->SetBorderSize(0);
  c->SetFillStyle(3000);

  c->cd(1)->SetPad(0.00, 0.3, 1.00, 1.00);
  c->cd(1)->SetMargin(0.12, 0.02, 0, 0.02);
  c->cd(1)->SetBottomMargin(0.02);
  // c->cd(1)->SetFillColor(2);
  c->SetFillStyle(3000);
  
  c->cd(2)->SetPad(0.00, 0.00, 1.00, 0.3);
  c->cd(2)->SetMargin(0.12, 0.02, 0.12, 0.00);
  c->cd(2)->SetBottomMargin(0.3);
  // c->cd(2)->SetFillColor(2);
  c->SetFillStyle(3000);
}
void SetMSDiv(
  TVirtualPad *c,TH1* hM,TH1* hS,
  const string optionM="][", const string optionS="]["
){
  SetMSDiv(c);
  SetMPadHis(c->cd(1),hM,optionM);
  SetSPadHis(c->cd(2),hS,optionS);
}
void SetMSDiv(
  TVirtualPad *c,TH1* hMU,TH1* hMD, TH1* RetDiv, const string option = "pe;histe;pe;LEq"
){
  std::vector<std::string> tokens;   std::string delimiter = ";";  std::string token;
  size_t start = 0, end;
  while ((end = option.find(delimiter, start)) != std::string::npos) {
      tokens.push_back(option.substr(start, end - start));
      start = end + delimiter.length();
  }
  tokens.push_back(option.substr(start)); // 加入最後一段
  for (size_t i = 0; i < tokens.size(); ++i) 
    std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);
  SetMSDiv(c);
  SetMPadHis(c->cd(1),hMU,tokens[0]);
  SetMPadHis(c->cd(1),hMD,tokens[1]+"same");
  hMU->Sumw2();
  hMD->Sumw2();
  hMU->GetYaxis()->UnZoom();
  RetDiv = (TH1*)hMU->Clone();
  // TH1F *hRDDMC = (TH1F*) hRD->Clone();

  RetDiv->Divide(hMD);
  SetSPadHis(c->cd(2),RetDiv,tokens[2],hMD->GetLineColor(),tokens[3]);
  if(hMU->GetMaximum()<hMD->GetMaximum()){
    int maxbin = hMD->GetMaximumBin();
    hMU->GetYaxis()->SetRangeUser(0,(hMD->GetBinContent(maxbin)+hMD->GetBinError(maxbin))*1.25);
  }
  if(int(tokens[3].find("l"))!=-1){
    TLegend *L = new TLegend(0.8,0.8,0.99,0.99);
    L->AddEntry(hMU,"Data",tokens[0].data());
    L->AddEntry(hMD,"MC","le");
    c->cd(1); L->Draw();
  }
    
}

void SampleMainSubPadDisplay() {
// void AdjustSubCanvasSize() {
  setTDRStyle();
  TCanvas *c = new TCanvas("c", "Canvas with adjusted sub-pads", 800+4, 800+28);
  TH1F *h1 = new TH1F("h1", "Histogram 1;Xaxis;Yaxis", 100, -3, 3);
  h1->FillRandom("gaus", 1000);

  TH1F *h2 = new TH1F("h2", "Histogram 2;Xaxis;Yaxis2", 100, -3, 3);
  h2->FillRandom("gaus", 1000);
  SetMSDiv(c,h1,h2);
  c->Update();
  c->Print("qwdio.gif");
}

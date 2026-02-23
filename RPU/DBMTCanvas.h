#pragma once
#include "TLatex.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "TVirtualPad.h"
#include "TStyle.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <algorithm>
const int TCanvasAutoConsumeX = 4;
const int TCanvasAutoConsumeY = 28;
void SetCanvasMargin(TCanvas* TCanvasObject,float x0,float y0,float x1,float y1){
  TCanvasObject->SetTopMargin(y1); TCanvasObject->SetBottomMargin(y0); 
  TCanvasObject->SetLeftMargin(x0); TCanvasObject->SetRightMargin(x1);
}
void SetCanvasMargin(TVirtualPad* TVPadObject,float x0,float y0,float x1,float y1){
  TVPadObject->SetTopMargin(y1); TVPadObject->SetBottomMargin(y0); 
  TVPadObject->SetLeftMargin(x0); TVPadObject->SetRightMargin(x1);
}
void SetNoMarginCanvas(TCanvas* TCanvasObject){ SetCanvasMargin(TCanvasObject,0,0,0,0);}
void SetNoMarginCanvas(TVirtualPad* TVPadObject){ SetCanvasMargin(TVPadObject,0,0,0,0);}

void DrawPdfPng(TCanvas* TCanvasObject,const char* FileTitle){
  TCanvasObject->cd();
  TCanvasObject->Print(Form("%s.pdf",FileTitle));
  TCanvasObject->Print(Form("%s.png",FileTitle));
}

void SetCTitleSetting(TCanvas* TCanvasObject, const float TitleSize,const int TitleFont=42){
  TPaveText *ptX = (TPaveText*)(TCanvasObject->GetPrimitive("title"));
  ptX->SetTextSize(TitleSize);
  ptX->SetTextFont(TitleFont);
  TCanvasObject->Modified();
}

void SetCTitleSetting(TVirtualPad* TVPadObject, const float TitleSize,const int TitleFont=42){
  TPaveText *ptX = (TPaveText*)(TVPadObject->GetPrimitive("title"));
  ptX->SetTextSize(TitleSize);
  ptX->SetTextFont(TitleFont);
  TVPadObject->Modified();
}

void DrawPadCloneOn(TCanvas* TCanvS, TCanvas* TCanvM){ 
  TCanvM->cd();
  TCanvS->DrawClonePad();
}
void DrawPadCloneOn(TCanvas* TCanvS, TVirtualPad* TCanvM){ 
  TCanvM->cd();
  TCanvS->DrawClonePad();
}
void DrawPadCloneOn(TVirtualPad* TCanvS, TCanvas* TCanvM){
  TCanvM->cd();
  TCanvS->Draw();
}
void DrawPadCloneOn(TVirtualPad* TCanvS, TVirtualPad* TCanvM){
  TCanvM->cd();
  TCanvS->Draw();
}
TCanvas *EasyCanvas(const char* CName, const int pixelX , const int pixelY, const int DnX = 1, const int DnY = 1, const string DOutline="true"){
  TCanvas *ctmp = new TCanvas(CName,CName,pixelX+TCanvasAutoConsumeX,pixelY+TCanvasAutoConsumeY);
  if(abs(DnX)<=1&&abs(DnY)<=1){ 
    return ctmp;
    // cout<<("DnX=DnY=1\n");
    // throw;
  }else if(abs(DnX)<1||abs(DnY)<1){
    cout<<"Error: DBMTCanvas.C: EasyCanvas(const int DnX, const int DnY) shouldn't use negative/0 DnX or DnY"<<endl;
    cout<<"    Please use (-1,-1) or ( 0, 0) or ( 1, 1) to present not to do ->Divide()."<<endl;
    throw;
  }
  ctmp->Divide(DnX,DnY);
  SetNoMarginCanvas(ctmp);
  ctmp->SetBorderSize(0);
  if(int(DOutline.find("t"))!=-1){
    for(int i=1; i<=DnX*DnY; i++){
      SetNoMarginCanvas(ctmp->cd(i));
      ctmp->cd(i)->SetBorderSize(0);
    }
  }
  return ctmp;
}
TCanvas *EasyCanvas(const char* CName, const int sizeX = 960, const string xyRate = "16:9", const int DnX = 1, const int DnY = 1, const string DOutline="true"){
  size_t posSp = xyRate.find(":");
  string Str_sizerx = xyRate.substr(0, posSp);
  string Str_sizery = xyRate.substr(posSp+1, xyRate.size()-1);
  float  sizeRx = atof(Str_sizerx.data()), sizeRy = atof(Str_sizery.data());
  int pixelX = sizeX;
  int pixelY = ceil(sizeX/sizeRx*sizeRy);
  return EasyCanvas(CName,pixelX,pixelY,DnX,DnY);
}
TCanvas *EasyCanvas(const char* CName, const int sizeX = 960, const float xyRate = 1.7777, const int DnX = 1, const int DnY = 1, const string DOutline="true"){
  return EasyCanvas(CName,sizeX,int(sizeX/xyRate),DnX,DnY);
}
TCanvas *EasyCanvas(const char* CName, const int sizeX = 960, const float xRate = 16.0, const float yRate = 9.0, const int DnX = 1, const int DnY = 1, const string DOutline="true"){
  return EasyCanvas(CName,sizeX,int(sizeX/xRate*yRate),DnX,DnY);
}

TCanvas *EasyCanvas(const char* CName, const string resCode="540p", const int DnX = 1, const int DnY = 1, const string DOutline="true"){
  int px = 0, py = 0;
  if(int(resCode.find("pv"))!=-1||int(resCode.find("pv"))!=-1){
    if     (int(resCode.find("540"))!=-1)  px  = 540;
    else if(int(resCode.find("1440"))!=-1) px = 1440;
    else if(int(resCode.find("144"))!=-1)  px = 144;
    else if(int(resCode.find("240"))!=-1)  px = 240;
    else if(int(resCode.find("360"))!=-1)  px = 360;
    else if(int(resCode.find("480"))!=-1)  px = 480;
    else if(int(resCode.find("720"))!=-1)  px = 720;
    else if(int(resCode.find("1080"))!=-1) px = 1080;
    else if(int(resCode.find("2160"))!=-1) px = 2160;
    py = ceil(px/9. * 16.);
  }else if(int(resCode.find("p"))!=-1||int(resCode.find("p"))!=-1){
    if     (int(resCode.find("540"))!=-1)  py  = 540;
    else if(int(resCode.find("1440"))!=-1) py = 1440;
    else if(int(resCode.find("144"))!=-1)  py = 144;
    else if(int(resCode.find("240"))!=-1)  py = 240;
    else if(int(resCode.find("360"))!=-1)  py = 360;
    else if(int(resCode.find("480"))!=-1)  py = 480;
    else if(int(resCode.find("720"))!=-1)  py = 720;
    else if(int(resCode.find("1080"))!=-1) py = 1080;
    else if(int(resCode.find("2160"))!=-1) py = 2160;
    px = ceil(py/9. * 16.);
  }else if(int(resCode.find("8k"))!=-1||int(resCode.find("8K"))!=-1||int(resCode.find("UHDP"))!=-1){
    px = 7680; py = 4320;
  }else if(int(resCode.find("4k"))!=-1||int(resCode.find("4K"))!=-1||int(resCode.find("UHD"))!=-1){
    px = 3840; py = 2160;
  }else if(int(resCode.find("2k"))!=-1||int(resCode.find("2K"))!=-1||int(resCode.find("WQHD"))!=-1){
    px = 2560; py = 1440;
  }else if(int(resCode.find("1k"))!=-1||int(resCode.find("1K"))!=-1||int(resCode.find("FHD"))!=-1){
    px = 1920; py = 1080;
  }else if(int(resCode.find("HD"))!=-1){
    px = 1280; py = 720;
  }else if(int(resCode.find("qHD"))!=-1){
    px = 960; py = 540;
  }else if(int(resCode.find("SD"))!=-1){
    px = 854; py = 480;
  }else if(int(resCode.find("nHD"))!=-1){
    px = 640; py = 360;
  }
  return EasyCanvas(CName,px,py,DnX,DnY,DOutline);

}

void LayoutProfile2D(TH2F* h2, TCanvas* c,string optX = "CT",string optY = "CT")
{
    TStyle* oldStyle = (TStyle*)gStyle->Clone("oldStyle");
    TGaxis::SetMaxDigits(3);
    gStyle->SetPaintTextFormat("1.2e");
    std::transform(optX.begin(), optX.end(), optX.begin(),
                 [](unsigned char c){ return std::tolower(c); });

    std::transform(optY.begin(), optY.end(), optY.begin(),
                 [](unsigned char c){ return std::tolower(c); });

    c->cd();
    c->Clear();

    // ---- Pads ----
    TPad* pad_main  = new TPad("pad_main",  "Main 2D",      0.25, 0.25, 1.00, 1.00);
    pad_main->SetLeftMargin(0.0);
    pad_main->SetBottomMargin(0.0);
    pad_main->SetTopMargin(0.05);
    pad_main->SetRightMargin(0.05);
    TPad* pad_left  = new TPad("pad_left", "Y Projection", 0.00, 0.00, 0.25, 1.00);
    pad_left->SetTopMargin(0.04);
    pad_left->SetRightMargin(0.01);
    pad_left->SetLeftMargin(0.4);
    pad_left->SetBottomMargin(0.25);
    TPad* pad_bottom= new TPad("pad_bottom","X Projection", 0.00, 0.00, 1.00, 0.25);
    pad_bottom->SetRightMargin(0.04);
    pad_bottom->SetTopMargin(0.01);
    pad_bottom->SetBottomMargin(0.4);
    pad_bottom->SetLeftMargin(0.25);

    pad_main->SetFillStyle(3000);
    pad_left->SetFillStyle(3000);
    pad_bottom->SetFillStyle(3000);
    pad_main->SetLineColorAlpha(0,0);
    pad_left->SetLineColorAlpha(0,0);
    pad_bottom->SetLineColorAlpha(0,0);
    pad_main->Draw();
    pad_left->Draw();
    pad_bottom->Draw();

    // ==== MAIN COLZ ====
    pad_main->cd();
    
    h2->GetZaxis()->SetMaxDigits(3);
    h2->SetMarkerSize(2);
    h2->Draw("COLZ text");
    
    const string Xtitle = h2->GetXaxis()->GetTitle();
    const string Ytitle = h2->GetYaxis()->GetTitle();
    // vector<double> vHistX, vHistY, vHistC={0,1,1.3};
    vector<double> vHistX, vHistY, vHistC={0,1,1.11};
    // =======================================
    //  LEFT → Y-axis Projection (rotated)
    // =======================================
    pad_left->cd();

    // 建 Y 投影
    TH1F* hY = (TH1F*)h2->ProjectionY("projY");
    for(int i=1;i<=h2->GetNbinsY();i++){
      vHistY.push_back ( h2->GetYaxis()->GetBinCenter(i) - h2->GetYaxis()->GetBinWidth(i)/2. );
      if(i==h2->GetNbinsY()) 
        vHistY.push_back ( h2->GetYaxis()->GetBinCenter(i) + h2->GetYaxis()->GetBinWidth(i)/2. );
      // cout<<vHistY.back()<<endl;
    }
    // 做成 2D colz 風格（投影當成 2D 1×N bins）
    TH2F* hY2 = new TH2F("hY2",Form(";Ratio;%s",Ytitle.data()), vHistC.size()-1, vHistC.data(), vHistY.size()-1, vHistY.data());

    for(int i=1;i<=hY->GetNbinsX();i++)
        hY2->SetBinContent(1, i, hY->GetBinContent(i));

    if(int(optY.find("ct"))!=-1) hY2->GetYaxis()->CenterLabels(1);
    h2->GetYaxis()->CenterLabels(1);
    hY2->GetXaxis()->CenterTitle(1);
    hY2->GetYaxis()->CenterTitle(1);
    
    hY2->GetXaxis()->SetTitleSize(0.14);
    hY2->GetYaxis()->SetTitleSize(0.14);
    hY2->GetXaxis()->SetTitleOffset(0.25);
    hY2->GetYaxis()->SetTitleOffset(1.4);
    hY2->GetXaxis()->SetLabelSize(0.14);
    hY2->GetYaxis()->SetLabelSize(0.14);
    hY2->GetXaxis()->SetLabelOffset(-0.083);
    hY2->GetYaxis()->SetLabelOffset(0.05);
    hY2->GetXaxis()->SetNdivisions(302);

    hY2->SetStats(0);
    hY2->Draw("COL");

    // --- TGraphErrors for Y ---
    int ny = hY->GetNbinsX();
    double maxNy = hY->GetMaximum();
    if (maxNy <= 0) maxNy = 1.0;   // 避免除以 0
    TGraphErrors* gY = new TGraphErrors(ny);
    for(int i=1;i<=ny;i++){
        double y  = hY->GetBinCenter(i);
        double n  = hY->GetBinContent(i);
        double en = hY->GetBinError(i);
        double n_scaled  = n / maxNy;
        double en_scaled = en / maxNy;
        gY->SetPoint(i-1, n_scaled, y);    // N on X-axis (horizontal)
        gY->SetPointError(i-1, en_scaled, 0);
    }

    gY->SetLineColor(2);
    gY->SetMarkerColor(2);
    gY->SetLineWidth(2);
    gY->Draw("L SAME");  // 疊加在 COLZ 上

    // =======================================
    //  BOTTOM → X-axis Projection
    // =======================================
    pad_bottom->cd();

    // 建 X 投影
    TH1F* hX = (TH1F*)h2->ProjectionX("projX");

    for(int i=1;i<=hX->GetNbinsX();i++){
      vHistX.push_back ( hX->GetXaxis()->GetBinCenter(i) - hX->GetXaxis()->GetBinWidth(i)/2. );
      if(i==hX->GetNbinsX()) 
        vHistX.push_back ( hX->GetXaxis()->GetBinCenter(i) + hX->GetXaxis()->GetBinWidth(i)/2. );
    }
    // 也做成 2D colz（Nbins × 1）
    TH2F* hX2 = new TH2F("hX2",Form(";%s;Ratio",Xtitle.data()), vHistX.size()-1, vHistX.data(), vHistC.size()-1, vHistC.data());

    for(int i=1;i<=hX->GetNbinsX();i++)
        hX2->SetBinContent(i, 1, hX->GetBinContent(i));

    if(int(optX.find("ct"))!=-1) hX2->GetXaxis()->CenterLabels(1);
    hX2->GetXaxis()->CenterTitle(1);
    hX2->GetYaxis()->CenterTitle(1);
    
    hX2->GetXaxis()->SetTitleSize(0.14);
    hX2->GetYaxis()->SetTitleSize(0.14);
    hX2->GetXaxis()->SetTitleOffset(1.2);
    hX2->GetYaxis()->SetTitleOffset(9999);
    hX2->GetXaxis()->SetLabelSize(0.14);
    hX2->GetYaxis()->SetLabelSize(0.14);
    hX2->GetXaxis()->SetLabelOffset(0.05);
    hX2->GetYaxis()->SetLabelOffset(0.01);
    hX2->GetYaxis()->SetNdivisions(302);
    
    hX2->SetStats(0);
    hX2->Draw("COL");

    // --- TGraphErrors for X ---
    int nx = hX->GetNbinsX();
    // 找最大值
    double maxNx = hX->GetMaximum();
    if (maxNx <= 0) maxNx = 1.0;   // 避免除以 0

    TGraphErrors* gX = new TGraphErrors(nx);
    for(int i = 1; i <= nx; i++){
        double x  = hX->GetBinCenter(i);
        double n  = hX->GetBinContent(i);
        double en = hX->GetBinError(i);

        double n_scaled  = n / maxNx;
        double en_scaled = en / maxNx;

        gX->SetPoint(i-1, x, n_scaled);       // X horizontal, N vertical (scaled)
        gX->SetPointError(i-1, 0, en_scaled); // Error also scaled
    }

    gX->SetLineColor(2);
    gX->SetMarkerColor(2);
    gX->SetLineWidth(2);
    gX->Draw("L SAME");
    pad_main->Update();  // 必須要，讓 stats box 生成
    c->cd();
    TPaveStats* st = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats")->Clone();
    if (st) {
        st->SetParent(c);        // 把 stats box 移到 canvas 上
        st->SetName("global_stats");

        // NDC 座標：左下角 (0.02~0.18, 0.02~0.15)
        st->SetX1NDC(0.001);
        st->SetX2NDC(0.20);
        st->SetY1NDC(0.001);
        st->SetY2NDC(0.16);

        st->SetTextColor(kBlack);
        st->Draw();              // 重新畫到整體 canvas
    }
    h2->SetStats(0);
    c->Modified();
    c->Update();
    gStyle->Reset("Default"); 
    gStyle->Copy(*oldStyle);
    delete oldStyle;
}

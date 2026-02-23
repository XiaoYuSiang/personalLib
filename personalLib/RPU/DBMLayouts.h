#include <vector>
#include <string>
#include <algorithm>
#include "TF2.h"
#include "TH2.h"
#include "TH1.h"
#include "TLatex.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TPaveStats.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "./DBMTF1/DBMTF1.h"
#include "DBMTgStyle.h"
#include "DBMTObject.h"
using namespace std;
using namespace FitInit;

TH2* ScaleTH2AxisInPlace(TH2* h2, double ScaleX, double ScaleY) {
    if (!h2) return nullptr;

    // 先複製 histogram，但改變 bin content 的位置
    int nx = h2->GetNbinsX();
    int ny = h2->GetNbinsY();
    double xlow = h2->GetXaxis()->GetXmin() * ScaleX;
    double xup  = h2->GetXaxis()->GetXmax() * ScaleX;
    double ylow = h2->GetYaxis()->GetXmin() * ScaleY;
    double yup  = h2->GetYaxis()->GetXmax() * ScaleY;

    TH2* h2_new = (TH2*)h2->Clone(Form("%s_temp", h2->GetName()));
    h2_new->SetBins(nx, xlow, xup, ny, ylow, yup);

    // 重新填入 content
    for (int ix = 1; ix <= nx; ix++) {
        for (int iy = 1; iy <= ny; iy++) {
            double content = h2->GetBinContent(ix, iy);
            double err     = h2->GetBinError(ix, iy);
            h2_new->SetBinContent(ix, iy, content);
            h2_new->SetBinError(ix, iy, err);
        }
    }

    // 刪掉舊的 histogram
    gDirectory->Delete(Form("%s;1", h2->GetName()));
    h2_new->SetName(h2->GetName());  // 使用原本名字

    return h2_new;
}

class LayoutProfile2DObj {
public:
  // 原始 2D 直方圖
  TH2* h2;

  // 投影
  TH1F* hX, *hY;

  // 投影對應 TGraphErrors
  TGraphErrors *gX, *gY;
  TGraph *gfx, *gfy;
  TF1 *fX, *fY;

  // 2D 形式顯示投影
  TH2D* hX2,* hY2;

  // Pads
  TPad* pad_main;
  TPad* pad_left;
  TPad* pad_bottom;

  // Stats box
  TPaveStats* stats;
  double NScaleX, NScaleY;
  // Canvas
  TCanvas* canvas;
  TCanvas* canvas2x1D;
  bool bfitX, bfitY;
  string FitOptX, FitOptY;
  
  void Print(string filePathName){ canvas->Print(filePathName.data()); };
  void Print2x1D(string filePathName){ canvas2x1D->Print(filePathName.data());};
  void SetOptsFit(string optX, string optY ){
    cout<<47<<" pfitX0 = "<<int(optX.find("fit"))<<endl;
    cout<<48<<" pfitY0 = "<<int(optY.find("fit"))<<endl;
    int pfitX0 = int(optX.find("fit")), pfitY0 = int(optY.find("fit"));
    bfitX = pfitX0!=-1; bfitY = pfitY0!=-1; 
    // double xmin = h2->GetXaxis()->GetXmin();
    // double xmax = h2->GetXaxis()->GetXmax();
    // double ymin = h2->GetYaxis()->GetXmin();
    // double ymax = h2->GetYaxis()->GetXmax();
    if(bfitX) {
      pfitX0 +=  4;
      FitOptX = optX.substr( pfitX0 , optX.find(")",pfitX0)-pfitX0);
    }
    if(bfitY) {
      pfitY0 +=  4;
      FitOptY = optY.substr( pfitY0 , optY.find(")",pfitY0)-pfitY0);
    }
    cout<<"FitMode? : X: "<<bfitX<<" ("<<FitOptX<<"),  Y: "<<bfitY<<" ("<<FitOptY<<")"<<endl;
  }
  void MakeTF1ToGraph(TF1 *f, TGraph *&g, bool transposed=false, int nPoints=200)
  {
      double xmin = f->GetXmin();
      double xmax = f->GetXmax();
      // if (g) delete g;   // 只有這時才安全
      g = new TGraph();
      g->SetLineWidth(2);
      g->SetLineColor(f->GetLineColor());
      for (int i=0; i<nPoints; i++) {
          double x = xmin + (xmax - xmin) * i / (nPoints - 1);
          double y = f->Eval(x);
          if (transposed) g->SetPoint(i, y, x);
          else            g->SetPoint(i, x, y);
      }
      
  }
  TH1F* GetH1X(string Name = ""){ return ((TH1F*) hX->Clone(Name.data())); };
  TH1F* GetH1Y(string Name = ""){ return ((TH1F*) hY->Clone(Name.data())); };
  TH1F* Getf1X(string Name = ""){ return ((TH1F*) fX->Clone(Name.data())); };
  TH1F* Getf1Y(string Name = ""){ return ((TH1F*) fY->Clone(Name.data())); };
  
  void Layout2x1D(TCanvas *c, string optX = "histe", string optY = "histe") {
    // cout<<124<<" "<<optX<<" "<<optY<<endl;
    vector<double> vPadMargins = GSGetPadMargins();
    GSPadMargins(0.150,0.130,0.05,0.050);
    canvas2x1D = c;
    canvas2x1D->Divide(2,1,0.001,0.001);
    std::transform(optX.begin(), optX.end(), optX.begin(),
                 [](unsigned char c){ return std::tolower(c); });
    std::transform(optY.begin(), optY.end(), optY.begin(),
                 [](unsigned char c){ return std::tolower(c); });
    // int posScaleX = int(optX.find("sta"));
    // int posScaleY = int(optX.find("sta"));
    if( int(optX.find("sta"))==-1 )
      hX->SetStats(0);
    else{
      hX->SetStats(1);
      optX.erase(optX.find("sta"), 3);
    } 
    if( int(optY.find("sta"))==-1 )
      hY->SetStats(0);
    else{
      hY->SetStats(1);
      optY.erase(optY.find("sta"), 3);
    } 
    
    hX->SetLineColor(1); hY->SetLineColor(1);
    canvas2x1D->cd(1);
    hX->Draw(optX.data());
    if(bfitX){
      fX->Draw("lsame");
      DrawFitInfo(fX,0.6,0.90,0.04);
    }
    canvas2x1D->cd(2);
    hY->Draw(optX.data());
    if(bfitY){
      fY->Draw("lsame");
      DrawFitInfo(fY,0.6,0.90,0.04);
    }
    GSPadMargins(vPadMargins);
  }
  void Layout2x1D(int graphSize, string optX , string optY) {
    canvas2x1D = new TCanvas(Form("LayoutProfile2x1DObj_%s",h2->GetName()),"",graphSize*2+4,graphSize+28);
    Layout2x1D(canvas2x1D,  optX ,  optY);
  }
  void Layout(string opt2D , string optX , string optY ) {
    canvas->cd();
    TStyle* oldStyle = (TStyle*)gStyle->Clone("oldStyle");
    TGaxis::SetMaxDigits(3);
    gStyle->SetPaintTextFormat("1.2e");
    gStyle->SetPaintTextFormat("1.2e");
    gStyle->SetOptStat(1);
    std::transform(opt2D.begin(), opt2D.end(), opt2D.begin(),
                 [](unsigned char c){ return std::tolower(c); });
                 
    std::transform(optX.begin(), optX.end(), optX.begin(),
                 [](unsigned char c){ return std::tolower(c); });

    std::transform(optY.begin(), optY.end(), optY.begin(),
                 [](unsigned char c){ return std::tolower(c); });

    cout<<"opt2D: "<<opt2D<<", optX: "<<optX<<", optY: "<<optY<<endl;
    SetOptsFit( optX, optY );
    // 建立三個子 Pad

    pad_main  = new TPad("pad_main",  "Main 2D",      0.25, 0.25, 1.00, 1.00);
    pad_main->SetLeftMargin(0.0);
    pad_main->SetBottomMargin(0.0);
    pad_main->SetTopMargin(0.05);
    pad_main->SetRightMargin(0.05);


    pad_left  = new TPad("pad_left", "Y Projection", 0.00, 0.00, 0.25, 1.00);
    pad_left->SetTopMargin(0.0375);
    pad_left->SetRightMargin(0.01);
    pad_left->SetLeftMargin(0.4);
    pad_left->SetBottomMargin(0.25);

    pad_bottom= new TPad("pad_bottom","X Projection", 0.00, 0.00, 1.00, 0.25);
    pad_bottom->SetRightMargin(0.0375);
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

    // ---- Main histogram ----
    pad_main->cd();
    // gStyle->SetTitleXOffset(1.25);
    // gStyle->SetTitleYOffset(1.25);
    h2->GetZaxis()->SetMaxDigits(3);
    h2->SetMarkerSize(2);
    // string opt2D = "colz scale(100,200)";
    int posScale = int(opt2D.find("scale("));
    if( posScale!=-1 ){
      int PosSxd = int(opt2D.find(",",posScale+6));
      int PosSyb = int(opt2D.find(")",PosSxd+1));
      string strScaleX = (opt2D.substr(posScale+6,PosSxd-posScale-6));
      string strScaleY = (opt2D.substr(PosSxd+1,PosSyb-1-PosSxd));
      double ScaleX = stof(strScaleX);
      double ScaleY = stof(strScaleY);
      h2 = ScaleTH2AxisInPlace(h2, ScaleX, ScaleY);
      opt2D.erase(posScale, PosSyb - posScale + 1);
    }
    h2->Draw(opt2D.data());
    
    const string Xtitle = h2->GetXaxis()->GetTitle();
    const string Ytitle = h2->GetYaxis()->GetTitle();
    // vector<double> vHistX, vHistY, vHistC={0,1,1.3};
    vector<double> vHistX, vHistY, vHistC={0,1,1.11};
    // =======================================
    //  LEFT → Y-axis Projection (rotated)
    // =======================================
    pad_left->cd();
    hY = (TH1F*)h2->ProjectionY(Form("%s_projY",h2->GetName()));
    hY ->GetXaxis()->SetTitleOffset(0.9);
    hY ->SetLineColor(2);
    hY ->SetLineWidth(2);
    hY ->GetYaxis()->UnZoom();
    NScaleY = hY->GetMaximum();
    hY ->Scale(1./hY->GetMaximum());
    hY ->GetYaxis()->SetRangeUser(0,1.2);
    hY ->GetYaxis()->SetTitle(Form("ratio(Max=%.1e)",NScaleY));
    if(bfitY){
      fY = MakeTF1Simple( hY, FitOptY.data() , 5 );
      fY->SetName(Form("fY_Gaus_%s",h2->GetName()));
      fY->SetTitle("Gaus fit-Y");
      fY->SetLineWidth(2);
      cout<<"BF fit "<<fY->GetParameter(1)<<"+-"<<fY->GetParameter(2)<<endl;
      double xmin, xmax;
      fY->GetRange(xmin, xmax);
      cout<<"Fit range "<<xmin<<"~"<<xmax<<endl;
      hY->Fit(fY,"R");
      cout<<"AT fit "<<fY->GetParameter(1)<<"+-"<<fY->GetParameter(2)<<endl;
      MakeTF1ToGraph(fY,gfy,true);
      cout<<" gfy "<<gfy<<endl;
    }
    
    for(int i=1;i<=h2->GetNbinsY();i++){
      vHistY.push_back ( h2->GetYaxis()->GetBinCenter(i) - h2->GetYaxis()->GetBinWidth(i)/2. );
      if(i==h2->GetNbinsY()) 
        vHistY.push_back ( h2->GetYaxis()->GetBinCenter(i) + h2->GetYaxis()->GetBinWidth(i)/2. );
      // cout<<vHistY.back()<<endl;
    }
    // 做成 2D colz 風格（投影當成 2D 1×N bins）
    TH2D* hY2 = new TH2D(Form("%s_hY2",h2->GetName()),Form(";Ratio;%s",Ytitle.data()), vHistC.size()-1, vHistC.data(), vHistY.size()-1, vHistY.data());

    for(int i=1;i<=hY->GetNbinsX();i++)
        hY2->SetBinContent(1, i, hY->GetBinContent(i));

    if(int(optY.find("ct"))!=-1) hY2->GetYaxis()->CenterTitle(1);
    if(int(optY.find("cl"))!=-1) hY2->GetYaxis()->CenterLabels(1);
    hY2->GetXaxis()->CenterTitle(1);
    
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
    gY = new TGraphErrors(ny);
    for(int i=1;i<=ny;i++){
        gY->SetPoint(i-1, hY->GetBinContent(i), hY->GetBinCenter(i));
        // N on X-axis (horizontal)
        gY->SetPointError(i-1, hY->GetBinError(i), hY->GetBinWidth(i)/2.);
    }
    gY->SetLineColor(2);
    gY->SetMarkerColor(2);
    gY->SetLineWidth(2);
    if(int(optY.find("gl"))!=-1)
      gY->Draw("lep SAME");
    else 
      gY->Draw("ep SAME");
    if(bfitY) gfy->Draw("lsame");

    // =======================================
    //  BOTTOM → X-axis Projection
    // =======================================
    pad_bottom->cd();
    // cout<<316<<endl;
    // 建 X 投影
    hX = (TH1F*)h2->ProjectionX(Form("%s_projX",h2->GetName()));
    hX ->GetXaxis()->SetTitleOffset(0.9);
    hX ->SetLineColor(2);
    hX ->SetLineWidth(2);
    hX ->GetYaxis()->UnZoom();
    NScaleX = hX->GetMaximum();
    hX ->Scale(1./hX->GetMaximum());
    hX ->GetYaxis()->SetRangeUser(0,1.2);
    hX ->GetYaxis()->SetTitle(Form("ratio(Max=%.1e)",NScaleX));
    if(bfitX){
      fX = MakeTF1Simple( hX , FitOptX.data(), 5 );
      fX->SetName(Form("fX_Gaus_%s",h2->GetName()));
      fX->SetTitle("Gaus fit-X");
      fX->SetLineWidth(2);
      cout<<"BF fit "<<fX->GetParameter(1)<<"+-"<<fX->GetParameter(2)<<endl;
      double xmin, xmax;
      fX->GetRange(xmin, xmax);
      hX->Fit(fX,"R");
      cout<<"AT fit "<<fX->GetParameter(1)<<"+-"<<fX->GetParameter(2)<<endl;
      cout<<" fX "<<fX<<endl;
      MakeTF1ToGraph(fX,gfx,false);
      cout<<" gfx "<<gfx<<endl;
      
    }
    // cout<<341<<endl;
    
    for(int i=1;i<=hX->GetNbinsX();i++){
      vHistX.push_back ( hX->GetXaxis()->GetBinCenter(i) - hX->GetXaxis()->GetBinWidth(i)/2. );
      if(i==hX->GetNbinsX()) 
        vHistX.push_back ( hX->GetXaxis()->GetBinCenter(i) + hX->GetXaxis()->GetBinWidth(i)/2. );
    }
    // 也做成 2D colz（Nbins × 1）
    TH2D* hX2 = new TH2D(Form("%s_hX2",h2->GetName()),Form(";%s;Ratio",Xtitle.data()), vHistX.size()-1, vHistX.data(), vHistC.size()-1, vHistC.data());
    
    for(int i=1;i<=hX->GetNbinsX();i++)
        hX2->SetBinContent(i, 1, hX->GetBinContent(i));

    if(int(optX.find("cl"))!=-1) hX2->GetXaxis()->CenterLabels(1);
    if(int(optX.find("ct"))!=-1) hX2->GetXaxis()->CenterTitle(1);
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
    double maxNx = hX->GetMaximum();
    if (maxNx <= 0) maxNx = 1.0;   // 避免除以 0
    gX = new TGraphErrors(nx);
    for(int i = 1; i <= nx; i++){
        gX->SetPoint(i-1, hX->GetBinCenter(i), hX->GetBinContent(i));
        // X horizontal, N vertical (scaled)
        gX->SetPointError(i-1, hX->GetBinWidth(i)/2., hX->GetBinError(i));
        // Error also scaled
    }

    // cout<<382<<endl;
    gX->SetLineColor(2);
    gX->SetMarkerColor(2);
    gX->SetLineWidth(2);
    if(int(optX.find("gl"))!=-1)
      gX->Draw("lep SAME");
    else 
      gX->Draw("ep SAME");
    if(bfitX) gfx->Draw("lsame");
    pad_main->Update();  // 必須要，讓 stats box 生成

    // cout<<394<<endl;
    canvas->cd();
    // ---- Stats box ----
    pad_main->Update();
    stats = (TPaveStats*)h2->GetListOfFunctions()->FindObject("stats");
    if(stats){
      stats = (TPaveStats*)stats->Clone();
      stats->SetParent(canvas);
      stats->SetName("global_stats");
      if(bfitX||bfitY){
        // stats->SetX1NDC(0.80); stats->SetX2NDC(0.96); stats->SetY1NDC(0.80); stats->SetY2NDC(0.96);
        stats->SetX1NDC(0.251); stats->SetX2NDC(0.40); stats->SetY1NDC(0.251); stats->SetY2NDC(0.40);
      }else{
        stats->SetX1NDC(0.001); stats->SetX2NDC(0.20); stats->SetY1NDC(0.001); stats->SetY2NDC(0.16);
      }
      stats->SetTextColor(kBlack);
      stats->Draw();
    }
    // TBox *box = new TBox(0.10, 0.84, 0.25, 0.96);
    // box->SetFillColorAlpha(kWhite, 0.4);   // 透明白色
    // box->SetLineColor(0);                 // 無邊框
    // box->Draw();
    if(bfitX) DrawFitInfo(fX, 0.01, 0.180, 0.025, 0b11011, 3);
    if(bfitY) DrawFitInfo(fY, 0.01, 0.080, 0.025, 0b11011, 3);
    // cout<<418<<endl;
    h2->SetStats(0);
    canvas->Modified();
    canvas->Update();
    // gStyle->Reset("Default"); 
    // TStyle* oldStyle = (TStyle*)gStyle->Clone("oldStyle");
    // TGaxis::SetMaxDigits(3);
    gStyle->Copy(*oldStyle);
    delete oldStyle;
    // cout<<418<<endl;
  }
  LayoutProfile2DObj(TH2* h2_, TCanvas *c_, string opt2D = "ColzText", string optX = "CTCLGL", string optY = "CTCLGL") {
    c_->Clear();
    UInt_t graphSize = c_->GetWw();
      cout<<graphSize<<endl;
    if(graphSize!=c_->GetWh()){
      if(graphSize<c_->GetWh()){
        graphSize = c_->GetWh();
      } 
      string CName = c_->GetName();
      // delete c_;
      cout<<graphSize<<endl;
      c_->SetCanvasSize(int(graphSize), int(graphSize));
      // c_ = *TCanvas(CName.data(),"",graphSize+4,graphSize+28);
    }
    canvas = c_;
    h2 = (TH2*)CheckNameAndClone(h2_);
    Layout( opt2D, optX, optY );
  }
  LayoutProfile2DObj(TH2* h2_, int graphSize, string opt2D = "ColzText", string optX = "CTCLGL", string optY = "CTCLGL") {
    h2 = (TH2*)CheckNameAndClone(h2_);
    canvas = new TCanvas(Form("LayoutProfile2DObj_%s",h2->GetName()),"",graphSize+4,graphSize+28);
    Layout( opt2D, optX, optY );
  }
};


class Layout2xH1CmpRatio {
public:
  // 投影
  TH1F* h1A, *h1B, *h1Ratio;

  TPad* pad_main;
  TPad* pad_bottom;
  TCanvas* canvas;
  bool bfitX, bfitY;
  string FitOptX, FitOptY;
  
  void Print(string filePathName){ canvas->Print(filePathName.data()); };
  Layout2xH1CmpRatio(int graphSize , TH1 *ha,TH1 *hb,string NorWay = "A",string optHa="pe",string optHb="he"){
    TH1F* h1A = (TH1F*) ha->Clone(Form("h1A_%s",ha->GetName()));
    TH1F* h1B = (TH1F*) hb->Clone(Form("h1B_%s",hb->GetName()));
    if(NorWay=="A"){
      h1A->Scale(1./h1A->Integral());
      h1B->Scale(1./h1B->Integral());
    }else if(NorWay=="M"){
      h1A->Scale(1./h1A->GetMaximum());
      h1B->Scale(1./h1B->GetMaximum());
    }
    double max = h1A->GetMaximum()>h1B->GetMaximum()
                  ?h1A->GetMaximum():h1B->GetMaximum();
    h1A->GetYaxis()->SetRangeUser(0,max*1.2);
    h1B->GetYaxis()->SetRangeUser(0,max*1.2);
    h1A->SetStats(0);
    h1B->SetStats(0);
    h1A->SetLineColor(1);
    h1A->SetLineWidth(2);
    h1A->SetMarkerColor(1);
    h1A->SetMarkerStyle(20);
    h1B->SetLineColor(2);
    h1B->SetLineWidth(2);
    h1A->GetYaxis()->SetTitleSize(0.08);
    h1A->GetYaxis()->SetTitleOffset(1.0);
    h1A->GetXaxis()->SetLabelOffset(99);
    h1A->GetYaxis()->SetLabelSize(0.073);
    
    // hY2->GetXaxis()->SetTitleSize(0.14);
    // hY2->GetYaxis()->SetTitleSize(0.14);
    // hY2->GetXaxis()->SetTitleOffset(0.25);
    // hY2->GetYaxis()->SetTitleOffset(1.4);
    // hY2->GetXaxis()->SetLabelSize(0.14);
    // hY2->GetYaxis()->SetLabelSize(0.14);
    // hY2->GetXaxis()->SetLabelOffset(-0.083);
    // hY2->GetYaxis()->SetLabelOffset(0.05);
    // hY2->GetXaxis()->SetNdivisions(302);
    
    TCanvas *canvas = new TCanvas(Form("Layout2xH1CmpRatiobj_%s_%s",h1A->GetName(),h1B->GetName()),"",graphSize+4,graphSize+28);
    canvas->cd();
    pad_main  = new TPad("pad_main",  "Main 2D",      0.0, 0.3, 1.00, 1.00);
    pad_main->SetLeftMargin(0.15);
    pad_main->SetBottomMargin(0.02);
    pad_main->SetTopMargin(0.05);
    pad_main->SetRightMargin(0.045);

    canvas->cd();
    pad_bottom= new TPad("pad_bottom","X Projection", 0.00, 0.00, 1.00, 0.3);
    pad_bottom->SetLeftMargin(0.15);
    pad_bottom->SetTopMargin(0.05);
    pad_bottom->SetBottomMargin(0.41);
    pad_bottom->SetRightMargin(0.045);

    pad_main->SetFillStyle(3000);
    pad_bottom->SetFillStyle(3000);
    pad_main->SetLineColorAlpha(0,0);
    pad_bottom->SetLineColorAlpha(0,0);

    canvas->cd();
    pad_main->Draw();
    canvas->cd();
    pad_bottom->Draw();
    pad_main->cd();
    h1Ratio->SetTitle("");
    h1A->Draw(optHa.data());
    h1B->Draw((optHb+"same").data());
    TH1F *h1Ratio = (TH1F*) h1A->Clone("h1Ratio");
    h1Ratio->SetTitle(Form(";%s;Ratio",h1A->GetXaxis()->GetTitle()));
    h1Ratio->GetXaxis()->SetTitleSize(0.14);
    // h1Ratio->GetYaxis()->SetTitleSize(0.14);
    h1Ratio->GetXaxis()->SetTitleOffset(0.84);
    h1Ratio->GetYaxis()->SetTitleOffset(0.40);
    h1Ratio->GetXaxis()->SetLabelSize(0.17);
    h1Ratio->GetYaxis()->SetLabelSize(0.17);
    h1Ratio->GetXaxis()->SetTitleSize(0.2);
    h1Ratio->GetYaxis()->SetTitleSize(0.19);
    h1Ratio->GetXaxis()->SetLabelOffset(0.05);
    h1Ratio->GetYaxis()->SetLabelOffset(0.01);
    h1Ratio->GetYaxis()->SetNdivisions(302);
    h1Ratio->GetYaxis()->SetRangeUser(0,2);
    h1Ratio->Divide(h1B);
    pad_bottom->cd();
    h1Ratio->Draw("pe");
    TF1 *f1 = new TF1(Form("f1_%s",h1Ratio->GetName()),"1",h1Ratio->GetXaxis()->GetXmin(),h1Ratio->GetXaxis()->GetXmax());
    f1->SetLineColor(1);
    f1->SetLineWidth(2);
    f1->SetLineStyle(2);
    f1->Draw("lsame");
    pad_main->cd();
    TLegend *leg = new TLegend(0.8,0.6,0.94,0.94);
    leg->SetBorderSize(0);      // 將邊框寬度設為 0 (最重要)
    leg->SetLineColorAlpha(0,0);      // 將邊框寬度設為 0 (最重要)
    // --- 進階：設定背景透明 (通常搭配使用) ---
    leg->SetFillStyle(3000);       // 設為 0 
    // --- 其他常用樣式設定 ---
    leg->SetTextFont(42);       // 使用標準無襯線字體
    // leg->SetTextSize(0.04);     // 設定文字大小
    leg->AddEntry(h1A,h1A->GetTitle(),"pel");
    leg->AddEntry(h1B,h1B->GetTitle(),"Fe");
    leg->Draw("same");
    
  }
  // void Layout(string opt2D , string optX , string optY ) {
    
  // }
};
void exampleLayoutProfile2DObj(){
    // 1. 初始化一個 100x100 Bins 的 2D 直方圖，X 與 Y 的物理範圍皆設定為 0 到 100
    TH2D *h2 = new TH2D("h2","h2",100,0,100,100,0,100);
    
    // 2. 定義一個 2D 高斯函數模型 (使用 ROOT 內建的 "xygaus" 模型)
    // 其公式為：[0]*exp(-0.5*((x-[1])/[2])**2-0.5*((y-[3])/[4])**2)
    TF2 *f2 = new TF2("f2", "xygaus", 0, 100, 0, 100);
    
    // 設定高斯參數：[0]振幅=1, [1]X均值=50, [2]X標準差=10, [3]Y均值=50, [4]Y標準差=10
    f2->SetParameters(1, 50, 10, 50, 10);
    
    // 3. 根據定義好的 2D 高斯分佈函數，隨機填入 100,000 個 Entries 來模擬探測器觀測數據
    h2->FillRandom("f2", 100000);
    
    // 4. 設定直方圖標題與軸標籤 (格式：大標題;X軸標籤;Y軸標籤;Z軸標籤)
    h2->SetTitle("2D Gassian Distribution;position X[cm];position Y[cm];count");
    
    // 5. 建立 Layout 佈局物件：這會自動將畫布切分為三個 Pad (主圖、左側投影、下方投影)
    // 參數：(直方圖指標, 畫布正方形尺寸, 2D繪圖選項, X投影擬合模式, Y投影擬合模式)
    // "cTFit(Gaus)" 會觸發內部的 MakeTF1Simple 並對投影進行高斯擬合
    LayoutProfile2DObj layout(h2, 1080, "colz", "cTFit(Gaus)", "cTFit(Gaus)");
    
    // 6. 將包含 2D 與 1D 投影的綜合畫布輸出為影像檔
    layout.Print(Form("./Example_2DAnd1D.gif"));
    
    // 7. 生成單獨的投影對比圖 (2x1D)：左邊顯示 X 投影，右邊顯示 Y 投影，及其對應的擬合結果
    layout.Layout2x1D(600, "hep", "hep");
    layout.Print2x1D(Form("./Example_2x1D.gif"));
    
    // 8. 準備展示座標軸數值縮放功能：將單位改為 mm 並更新標題
    h2->SetTitle("2D Gassian Distribution[mm];position X;position[mm] Y;count");
    
    // 9. 使用自定義的 "scale(X,Y)" 選項，這會觸發 ScaleTH2AxisInPlace 函式
    // 在此例中，將原本 0-100 的軸數值放大 10 倍變成 0-1000 (模擬 cm 轉 mm)
    string drawOpt = Form("colz scale(%f,%f)", 10.0, 10.0);
    
    // 10. 建立新的佈局物件以套用縮放效果，並輸出縮放後的綜合圖與投影圖
    LayoutProfile2DObj layoutBPADCW_mm(h2, 1080, drawOpt, "cTFit(Gaus)", "cTFit(Gaus)");
    layoutBPADCW_mm.canvas->Print(Form("Example_2DAnd1D_mm.gif"));
    
    layoutBPADCW_mm.Layout2x1D(600, "hep", "hep");
    layoutBPADCW_mm.Print2x1D(Form("./Example_2x1D_mm.gif"));
}


void exampleLayout2xH1CmpRatio(){
    // 1. 初始化一個 100x100 Bins 的 2D 直方圖，X 與 Y 的物理範圍皆設定為 0 到 100
    TH1D *h1Data = new TH1D("h1Data","h1Data;MeV;count",100,0,100);
    TH1D *h1MC = new TH1D("h1MC","h1MC;MeV;count",100,0,100);
    
    // 2. 定義一個 2D 高斯函數模型 (使用 ROOT 內建的 "xygaus" 模型)
    // 其公式為：[0]*exp(-0.5*((x-[1])/[2])**2-0.5*((y-[3])/[4])**2)
    TF2 *fD = new TF2("fD", "gaus", 0, 100, 0, 100);
    TF2 *fM = new TF2("fM", "gaus", 0, 100, 0, 100);
    
    // 設定高斯參數：[0]振幅=1, [1]X均值=50, [2]X標準差=10, [3]Y均值=50, [4]Y標準差=10
    fD->SetParameters(1, 50, 10);
    fM->SetParameters(1, 55, 8);
    
    // 3. 根據定義好的 2D 高斯分佈函數，隨機填入 100,000 個 Entries 來模擬探測器觀測數據
    h1Data->FillRandom("fD", 100000);
    h1MC->FillRandom("fM", 100000);
    
    Layout2xH1CmpRatio layout(640, h1Data, h1MC);
    
    // 6. 將包含 2D 與 1D 投影的綜合畫布輸出為影像檔
    // layout.Print(Form("./Example_2DAnd1D.gif"));
    
    // 7. 生成單獨的投影對比圖 (2x1D)：左邊顯示 X 投影，右邊顯示 Y 投影，及其對應的擬合結果
    // layout.Layout2x1D(600, "hep", "hep");
    // layout.Print2x1D(Form("./Example_2x1D.gif"));
    
    // 8. 準備展示座標軸數值縮放功能：將單位改為 mm 並更新標題
    // h2->SetTitle("2D Gassian Distribution[mm];position X;position[mm] Y;count");
    
    // 9. 使用自定義的 "scale(X,Y)" 選項，這會觸發 ScaleTH2AxisInPlace 函式
    // 在此例中，將原本 0-100 的軸數值放大 10 倍變成 0-1000 (模擬 cm 轉 mm)
    // string drawOpt = Form("colz scale(%f,%f)", 10.0, 10.0);
    
    // 10. 建立新的佈局物件以套用縮放效果，並輸出縮放後的綜合圖與投影圖
    // LayoutProfile2DObj layoutBPADCW_mm(h2, 1080, drawOpt, "cTFit(Gaus)", "cTFit(Gaus)");
    // layoutBPADCW_mm.canvas->Print(Form("Example_2DAnd1D_mm.gif"));
    
    // layoutBPADCW_mm.Layout2x1D(600, "hep", "hep");
    // layoutBPADCW_mm.Print2x1D(Form("./Example_2x1D_mm.gif"));
}

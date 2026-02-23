#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include "RooRealVar.h"
#include "RooDataHist.h"
#include "RooCBShape.h"
#include "RooFitResult.h"
#include "RooAbsReal.h"
#include "RooPlot.h"
using namespace RooFit;

// 結構體用於存儲每個參數的初始值和邊界
struct ParSet {
  double initial;
  double min;
  double max;
};

// 結構體用於存儲擬合結果
struct CBFitResults {
  double mean, meanErr;
  double sigma, sigmaErr;
  double alpha, alphaErr;
  double n, nErr;
  double Amp, AmpErr;
};
void DisplayFitResults(CBFitResults res, TVirtualPad *pad, double X0, double X1, double Y0, double Y1) {
  // 設置 pad
  pad->cd();

  // 創建 TLatex 對象來顯示文本
  TLatex *latex = new TLatex();
  latex->SetNDC(true);  // 設置為相對座標系

  // 設定字型大小
  latex->SetTextSize(0.04);

  // 定義顯示文本的格式
  TString meanText = Form("#mu = %.3f #pm %.3f", res.mean, res.meanErr);
  TString sigmaText = Form("#sigma = %.3f #pm %.3f", res.sigma, res.sigmaErr);
  TString alphaText = Form("#alpha = %.3f #pm %.3f", res.alpha, res.alphaErr);
  TString nText = Form("n = %.3f #pm %.3f", res.n, res.nErr);
  TString ampText = Form("A = %.3f #pm %.3f", res.Amp, res.AmpErr);

  // 顯示文本在指定範圍內
  double Ychip = (Y1-Y0)/5.;
  latex->DrawLatex(X0, Y1 - 0.5*Ychip, meanText);
  latex->DrawLatex(X0, Y1 - 1.5*Ychip, sigmaText);
  latex->DrawLatex(X0, Y1 - 2.5*Ychip, alphaText);
  latex->DrawLatex(X0, Y1 - 3.5*Ychip, nText);
  latex->DrawLatex(X0, Y1 - 4.5*Ychip, ampText);

  // 更新 pad
  pad->Update();
}
// FitCrystalBall 函數接受各參數的設置並進行擬合
CBFitResults FitCrystalBall(TH1* hist,
  ParSet meanSet = {0, -10, 10},
  ParSet sigmaSet = {1, 0.1, 5},
  ParSet alphaSet = {1, 0.5, 5},
  ParSet nSet = {5, 0.1, 10},
  const bool SaveOpt = true
) {
    // 若未提供初始值，根據直方圖設置預設值
  if (meanSet.initial == 0 && meanSet.min == -10 && meanSet.max == 10) {
      meanSet.initial = hist->GetMean();
      meanSet.min = hist->GetXaxis()->GetXmin();
      meanSet.max = hist->GetXaxis()->GetXmax();
  }
  if (sigmaSet.initial == 1 && sigmaSet.min == 0.1 && sigmaSet.max == 5) {
      sigmaSet.initial = hist->GetRMS();
      sigmaSet.min = 0.1 * hist->GetRMS();
      sigmaSet.max = 2 * hist->GetRMS();
  }
  // 1. 定義變量
  RooRealVar x("x", "Observable", hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax());
  int NbinHist = hist->GetNbinsX ();
  double xMin = x.getMin();              // 提取最小值
  double xMax = x.getMax();              // 提取最大值
  double binWidth = (xMax - xMin) / NbinHist; // 計算 bin 寬度
  // 2. 創建 RooDataHist，將 TH1 的數據轉換為 RooFit 可以處理的格式
  RooDataHist data("data", "Data", x, Import(*hist));
  
  // 3. 使用參數設置來初始化 Crystal Ball 函數的參數
  RooRealVar mean("mean", "Mean", meanSet.initial, meanSet.min, meanSet.max);
  RooRealVar sigma("sigma", "Sigma", sigmaSet.initial, sigmaSet.min, sigmaSet.max);
  RooRealVar alpha("alpha", "Alpha", alphaSet.initial, alphaSet.min, alphaSet.max);
  RooRealVar n("n", "n", nSet.initial, nSet.min, nSet.max);
  
  // 4. 定義 Crystal Ball 函數
  RooCBShape cb("cb", "Crystal Ball Function", x, mean, sigma, alpha, n);
  
  // 5. 對數據進行擬合
  RooRealVar nsig("N_{SIG}","signal events",hist->Integral()*0.8,hist->Integral()*1.2);
  RooAddPdf all("all","model",RooArgList(cb),RooArgList(nsig));

  RooFitResult* fitResult = all.fitTo(data, Save());
  
  // 6. 畫圖
  TCanvas* canvas = new TCanvas("canvas", "Crystal Ball Fit", 800, 600);
  RooPlot* frame = x.frame();
  data.plotOn(frame);
  cb.plotOn(frame);
  frame->Draw();
  
  // Optional: 保存畫布為圖片
  if(SaveOpt) canvas->SaveAs("CrystalBallFit.png");
  
  // 7. 將擬合參數及其誤差保存到結構體並返回
  CBFitResults results;
  results.mean = mean.getVal();
  results.meanErr = mean.getError();
  results.sigma = sigma.getVal();
  results.sigmaErr = sigma.getError();
  results.alpha = alpha.getVal();
  results.alphaErr = alpha.getError();
  results.n = n.getVal();
  results.nErr = n.getError();
  
  x.setVal(mean.getVal());
  cout<<all.getVal(RooArgSet(x))<<"\t"<<nsig.getVal()<<endl;
  // throw;
  results.Amp =  all.getVal(RooArgSet(x)) * nsig.getVal() *binWidth;
  results.AmpErr = all.getVal(RooArgSet(x)) * nsig.getError() *binWidth;
  cout<<results.Amp<<endl;
  return results;
}

TF1* GetCBFit(
  const string name, const CBFitResults& fitResults, double xMin, double xMax
) {
    // 定義 Crystal Ball 函數的表達式
    // cout<<112<<" "<<fitResults.Amp<<endl;
    TF1* cbFunc = new TF1("cbFunc", [](double* x, double* par) {
        double xx = (x[0] - par[0]) / par[1];  // 計算 (x - mean) / sigma
        if (xx > -par[2]) {
            return exp(-0.5 * xx * xx)*par[4];  // Gaussian 部分
        } else {
            double A = pow(par[3] / fabs(par[2]), par[3]) * exp(-0.5 * par[2] * par[2]);
            double B = par[3] / fabs(par[2]) - fabs(par[2]);
            return A * pow(B - xx, -par[3])*par[4];  // Power law 部分
        }
    }, xMin, xMax, 8);  // 使用 8 個參數以便包含誤差範圍
    cbFunc->SetName(name.data());
    // 設置參數：mean, sigma, alpha, n 及其誤差範圍
    cbFunc->SetParameters(
      fitResults.mean, fitResults.sigma,
      fitResults.alpha, fitResults.n, fitResults.Amp,
      fitResults.meanErr, fitResults.sigmaErr,
      fitResults.alphaErr, fitResults.nErr, fitResults.AmpErr
    );
    // cout<<135<<" "<<cbFunc->GetParameter(0)<<endl;
    // cout<<136<<" "<<cbFunc->GetParameter(1)<<endl;
    // cout<<137<<" "<<cbFunc->GetParameter(2)<<endl;
    // cout<<138<<" "<<cbFunc->GetParameter(3)<<endl;
    // cout<<139<<" "<<cbFunc->GetParameter(4)<<endl;
    // TCanvas *c1
    return cbFunc;
}

void RunCrystalBallFit() {
  // 打開 ROOT 文件並讀取 TH1
  TFile* file = TFile::Open("your_file.root");
  TH1* hist = (TH1*)file->Get("your_histogram");

  // 確保直方圖存在
  if (!hist) {
    std::cerr << "Error: Histogram not found in the file." << std::endl;
    return;
  }

  // 定義各參數的初始值和邊界條件
  ParSet meanSet  = {hist->GetMean(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax()};
  ParSet sigmaSet = {hist->GetRMS(), 0.1 * hist->GetRMS(), 2 * hist->GetRMS()};
  ParSet alphaSet = {1.0, 0.5, 5.0};
  ParSet nSet     = {5.0, 0.1, 10.0};

  // 對 TH1 進行 Crystal Ball 擬合
  CBFitResults fitResults = FitCrystalBall(hist, meanSet, sigmaSet, alphaSet, nSet);

  // 輸出擬合參數及其誤差
  std::cout << "Crystal Ball Fit Results:" << std::endl;
  std::cout << "Mean: " << fitResults.mean << " ± " << fitResults.meanErr << std::endl;
  std::cout << "Sigma: " << fitResults.sigma << " ± " << fitResults.sigmaErr << std::endl;
  std::cout << "Alpha: " << fitResults.alpha << " ± " << fitResults.alphaErr << std::endl;
  std::cout << "n: " << fitResults.n << " ± " << fitResults.nErr << std::endl;

  // 關閉文件
  file->Close();
}

#ifndef DBM_TF1_H
#define DBM_TF1_H

#include <TH1.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <algorithm>
#include <iostream>

namespace FitInit {

// ======================================================
//  Overloaded: Estimate initial parameters
// ======================================================

// ---------- TH1 ----------
inline void EstimateBasicParams(TH1 *h, double &amp, double &mean, double &sigma)
{
    int ibin = h->GetMaximumBin();
    amp   = h->GetMaximum();
    mean  = h->GetBinCenter(ibin);
    sigma = h->GetRMS();
    if (sigma <= 0)
        sigma = (h->GetXaxis()->GetXmax() - h->GetXaxis()->GetXmin()) / 10.0;
}

// ---------- TGraph ----------
inline void EstimateBasicParams(TGraph *g, double &amp, double &mean, double &sigma)
{
    int n = g->GetN();
    if (n <= 0) { amp=1; mean=0; sigma=1; return; }

    double *x = g->GetX();
    double *y = g->GetY();

    int imax = std::max_element(y, y+n) - y;
    amp  = y[imax];
    mean = x[imax];

    // RMS estimation
    double sumw=0, sumx=0, sumxx=0;
    for (int i=0;i<n;i++){
        sumw  += y[i];
        sumx  += x[i]*y[i];
        sumxx += x[i]*x[i]*y[i];
    }
    double var = sumxx/sumw - (sumx*sumx)/(sumw*sumw);
    sigma = (var>0 ? sqrt(var) : 1);
}

// ---------- TGraphErrors （與 TGraph 相同邏輯） ----------
inline void EstimateBasicParams(TGraphErrors *g, double &amp, double &mean, double &sigma)
{
    EstimateBasicParams((TGraph*)g, amp, mean, sigma);
}

// ======================================================
//       Universal Gaussian creator (templated)
// ======================================================

template<typename T>
inline TF1* MakeGaus(T *obj, double Range = 3, const char *name = "")
{
    double amp, mean, sigma;
    EstimateBasicParams(obj, amp, mean, sigma);
    
    double xmin = mean - Range*sigma;
    double xmax = mean + Range*sigma;

    TF1 *f = new TF1(
        (strcmp(name,"") ? Form("fitGaus_%s", obj->GetName()) : name),
        "gaus", xmin, xmax
    );
    f->SetParameters(amp, mean, sigma);
    f->SetParNames("A", "#mu", "#sigma");
    f->SetParLimits(0, 0, 10*amp);
    f->SetParLimits(1, mean - sigma, mean + sigma);
    f->SetParLimits(2, sigma*0.2, sigma*5);
    f->SetLineColor(obj->GetLineColor());
    f->SetLineWidth(2);

    return f;
}

// ======================================================
// Landau
// ======================================================

template<typename T>
inline TF1* MakeLandau(T *obj, double Range = 5, const char *name = "")
{
    double amp, mean, sigma;
    EstimateBasicParams(obj, amp, mean, sigma);

    double xmin = mean - Range*sigma/1.2;
    double xmax = mean + Range*sigma*1.2;

    TF1 *f = new TF1(
        (strcmp(name,"") ? Form("fitLandau_%s", obj->GetName()) : name),
        "landau", xmin, xmax
    );
    f->SetParameters(amp, mean, sigma);
    f->SetParNames("Amp", "MPV", "#sigma");

    f->SetParLimits(0, 0, 10*amp);
    f->SetParLimits(1, mean - sigma, mean + 3*sigma);
    f->SetParLimits(2, sigma*0.2, sigma*5);
    f->SetLineColor(obj->GetLineColor());
    f->SetLineWidth(2);

    return f;
}

// ======================================================
// Exponential
// ======================================================

template<typename T>
inline TF1* MakeExpo(T *obj, double Range = 3, const char *name = "")
{
    double amp, mean, sigma;
    EstimateBasicParams(obj, amp, mean, sigma);
    // f->SetParNames("Amp", "#mu", "#sigma");

    // exp range
    double xmin = mean - Range*sigma;
    double xmax = mean + Range*sigma;

    TF1 *f = new TF1(
        (strcmp(name,"") ? Form("fitExpo_%s", obj->GetName()) : name),
        "expo", xmin, xmax
    );

    // simple initial param
    f->SetParameters(log(amp), -1.0 / sigma);
    f->SetLineColor(obj->GetLineColor());
    f->SetLineWidth(2);

    return f;
}

// ======================================================
// Wrapper: choose by string
// ======================================================

template<typename T>
inline TF1* MakeTF1Simple(T *obj, std::string FunName, double Range, const std::string &FitName="")
{
    if(FunName == "gaus")        return MakeGaus(obj,Range, FitName.c_str());
    if(FunName == "landau")      return MakeLandau(obj,Range, FitName.c_str());
    if(FunName == "expo")        return MakeExpo(obj,Range, FitName.c_str());

    std::cerr << "[ERROR] MakeTF1Simple(): Unknown function \"" << FunName << "\"\n";
    throw;
    // return nullptr;
}
void DrawFitInfo(
  TF1* f, double x0 = 0.60,double y0 = 0.88,double dy = 0.045,int bprint = 0b11011,int nDigits = 3
){
    if (!f) return;

    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(dy*0.9);
    latex.SetTextColor(f->GetLineColor());

    int nPars = f->GetNpar();

    // -------------------------------
    // 第一行：Formula (function title)
    // -------------------------------
    int nLs = 0;
    if(bprint>>4&1){
      latex.DrawLatex(x0, y0, Form("%s", f->GetTitle()));
      nLs++;
    }
    // -------------------------------------
    // 第二行：chi2/ndf
    // -------------------------------------
    double chi2 = f->GetChisquare();
    int ndf  = f->GetNDF();
    if(bprint>>3&1){
      latex.DrawLatex(x0, y0 - nLs*dy,
          Form("#scale[0.7]{#chi^{2}/NDF = %.2f / %d = %.2f}",
               chi2,  ndf, (ndf > 0 ? chi2 / ndf : 0)));
      nLs++;
    }
    // -------------------------------------
    // 後續：每個參數及其誤差
    // -------------------------------------
    for (int i = 0; i < nPars; i++) {
        double val  = f->GetParameter(i);
        double err  = f->GetParError(i);
        const char* name = f->GetParName(i);
        if(bprint>>(2-i)&1){
          latex.DrawLatex(
              x0, y0 - nLs * dy, // 往下排
              Form("%s = %.*g #pm %.*g",
                   name,  nDigits, val,  nDigits, err)
          );
          nLs++;
        }
    }
}


} // namespace FitInit
#endif

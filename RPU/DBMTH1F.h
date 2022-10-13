#include <limits.h>
#include "./DBMTPad.h"
#include "./DBMTLegend.h"
#include "./DBMTPDF.h"
#include "./DBMTCanvas.h"
#include "./DBMTH1F/TH1FAxis.h"
#include "./DBMTH1F/TH1FLine.h"
#include "./DBMTH1F/TH1FMark.h"
#include "./DBMTH1F/TH1FOper.h"
#include "./DBMTH1F/TH1FTitle.h"
using namespace std; 
void setTH1FDefault(TH1F* tH1FObject,TString option,int divi){
  option.ToLower();
  bool M=false, D=false, T=false, S=false, U=false, B=false;
  // M- Monticalro ; D- Data ; T- Two pad ; U- Normal ; B- base pad ; S- Single pad`; 
  int num = (int)option.Length();
  for (int i = 0;i < num ; i++){
    if (option[i]=='m') M = true;
    if (option[i]=='d') D = true;
    if (option[i]=='t') T = true;
    if (option[i]=='s') S = true;
    if (option[i]=='u') U = true;
    if (option[i]=='b') B = true;
  }
  if ( M == true ){
    setLineMonc(tH1FObject);
    setMarkMonc(tH1FObject);
  }else if( D == true ){
    setLineData(tH1FObject);
    setMarkData(tH1FObject);
  }
  if ( T == true ){
    if ( U == true ){
      setAxisTNormal(tH1FObject);
    }else if ( B == true ){
      setAxisTDivide(tH1FObject);
    }
  }else if( S == true ){
    setAxisSNormal(tH1FObject);
    if ( U == true || B == true ){
      cout<<"U & B option can't use at the same time"<<endl;
    }
  }
  setTitleNone(tH1FObject);
  tH1FObject->GetYaxis()->SetNdivisions(divi);
}

void FindYRangeTH1F(const TH1F* tH1FObj,float &ReturnYm,float &ReturnYM,const int precision=1,const float Trigger_MinY=0.1,const float Trigger_MaxY=2){
  
  float maxcon = INT_MIN*1., mincon = INT_MAX*1.;
  for(int i0=0;i0<tH1FObj->GetNbinsX();i0++){
    float contmp = tH1FObj->GetBinContent(i0+1);
    if(contmp>=Trigger_MinY && contmp<=Trigger_MaxY){
      if(maxcon<contmp)  maxcon = contmp;
      if(mincon>contmp)  mincon = contmp;
    }
  }
  
  ReturnYM = float(int(maxcon*pow(10,precision)*1.1)+pow(10,precision-1))
              /float(pow(10,precision));
  ReturnYm = float(int(mincon*pow(10,precision)*1))
              /float(pow(10,precision));
  
}



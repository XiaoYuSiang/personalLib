#include "TCanvas.h"

void DrawPdfPng(TCanvas* TCanvasObject,const char* FileTitle){
  TCanvasObject->cd();
  TCanvasObject->Print(Form("%s.pdf",FileTitle));
  TCanvasObject->Print(Form("%s.png",FileTitle));
  cout<<Form("%s.pdf    is saved",FileTitle)<<endl;
  cout<<Form("%s.png    is saved",FileTitle)<<endl;
  
}



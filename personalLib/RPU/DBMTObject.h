#include <TObject.h>
#include <TString.h>
#include <TDirectory.h>
#include <TH1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <iostream>

/**
 * @brief 檢查名稱是否被佔用，並克隆出一個具備唯一名稱的 TObject
 * * 此函數支援所有繼承自 TObject 的類別，如 TH1, TH2, TGraph, TCanvas, TTree 等。
 * * @param obj 來源物件指標 (例如直方圖、圖表或畫布)
 * @return TObject* 具備唯一名稱的新物件指標，需根據需要進行轉型 (Cast)
 */
TObject* CheckNameAndClone(TObject* obj) {
    if (!obj) return nullptr;

    TString baseName = obj->GetName();
    TString newName = baseName;
    
    // 檢查當前目錄 (gDirectory) 中是否已經存在該名稱
    // 如果 FindObject 回傳非空指標，表示名稱已被佔用
    if (gDirectory->FindObject(newName)) {
        int counter = 1;
        
        // 進入迴圈嘗試尋找後綴 _1, _2, _3...
        do {
            newName = TString::Format("%s_%d", baseName.Data(), counter);
            counter++;
        } while (gDirectory->FindObject(newName));
        
        // 找到可用名稱後，輸出提示資訊（可選）
        // std::cout << "名稱 " << baseName << " 已被占用，改用新名稱: " << newName << std::endl;
    }

    // 使用唯一名稱克隆物件
    // Clone 函數定義在 TObject 中，回傳 TObject*
    TObject* objClone = obj->Clone(newName);
    
    return objClone;
}
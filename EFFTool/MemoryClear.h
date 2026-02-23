#include <TROOT.h>
#include <TObject.h>
#include <TDirectory.h>
#include <TList.h>
#include <TCanvas.h>
#include <TH1.h>
#include <iostream>
#include <vector>

/**
 * @brief 掃描 ROOT 記憶體中的物件表，安全地刪除所有 TCanvas 和 TH1-TH3 直方圖。
 * * 這是防止在巨集 (Macro) 運行結束後記憶體洩漏的標準做法。
 */
void CleanupHistogramsAndCanvases() {
    std::cout << "--- Clear ROOT Memory objects ---" << std::endl;

    // --- 1. 清理所有 TCanvas ---
    // 遍歷 gROOT 上的畫布列表
    TList *canvases = (TList*)gROOT->GetListOfCanvases();
    if (canvases) {
        // 使用 TIter 迭代器
        TIter next_canvas(canvases);
        TObject *obj;
        while ((obj = next_canvas())) {
            // 檢查物件是否為 TCanvas
            if (obj->InheritsFrom(TCanvas::Class())) {
                // std::cout << "Free TCanvas: " << obj->GetName() << std::endl;
                delete obj; // TCanvas 刪除時會自動從列表中移除自己
            }
        }
    }
    
    // --- 2. 清理所有 TH1, TH2, TH3 (繼承自 TH1) ---
    // 遍歷當前目錄（通常是 gROOT/gDirectory）中的所有物件
    TList *list = gDirectory->GetList();
    if (list) {
        // 由於我們在循環中刪除元素，需要一個臨時列表來儲存要刪除的指針
        std::vector<TObject*> to_delete;
        TIter next_obj(list);
        TObject *obj;
        
        // 找出所有直方圖
        while ((obj = next_obj())) {
            // 檢查物件是否繼承自 TH1 (TH2, TH3 也包含在內)
            if (obj->InheritsFrom(TH1::Class())) {
                to_delete.push_back(obj);
            }
        }
        
        // 執行刪除
        // std::cout << "Free histogram: ";
        for (TObject* obj_to_delete : to_delete) {
            // std::cout << obj_to_delete->GetName()<<" ,";
            delete obj_to_delete;
        }
        // std::cout << std::endl;
    }
    
    std::cout << "--- ROOT Clear ROOT Memory ---" << std::endl;
}
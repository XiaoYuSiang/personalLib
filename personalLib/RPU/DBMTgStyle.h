void GSPadMargins(double left, double bottom, double right, double top) {
    gStyle->SetPadLeftMargin(left);
    gStyle->SetPadRightMargin(right);
    gStyle->SetPadTopMargin(top);
    gStyle->SetPadBottomMargin(bottom);
}
void GSPadMargins(vector<double> NDCS) {
    gStyle->SetPadLeftMargin(NDCS[0]);
    gStyle->SetPadBottomMargin(NDCS[1]);
    gStyle->SetPadRightMargin(NDCS[2]);
    gStyle->SetPadTopMargin(NDCS[3]);
}
vector<double> GSGetPadMargins() {
    vector<double> RET;
    RET.push_back( gStyle->GetPadLeftMargin()   );
    RET.push_back( gStyle->GetPadBottomMargin() );
    RET.push_back( gStyle->GetPadRightMargin()  );
    RET.push_back( gStyle->GetPadTopMargin()    );
    return RET;
}
void GSStatsPadNDC(double left, double bottom, double right, double top) {
    gStyle->SetStatH(top-bottom);
    gStyle->SetStatW(right-left);
    gStyle->SetStatX(right);
    gStyle->SetStatY(top);
}
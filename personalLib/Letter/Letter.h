#include <iostream>
#include <vector>
#include <fstream>
#include <typeinfo>
#include <algorithm>
using namespace std;


void FloToStr(double d, int befordigit=3, int Afterdigit=3){
  int Totaldigit=befordigit+Afterdigit;
  cout<< setw(Totaldigit) << setfill(' ') << fixed << setprecision(Afterdigit-1) << d;
}

vector<string> parseAndLowercase(const string option = "pe;histesame;pe;L") {
    std::vector<std::string> tokens;
    std::string delimiter = ";";
    std::string token;
    size_t start = 0, end;

    // 分割字串
    while ((end = option.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(option.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(option.substr(start)); // 加入最後一段

    // 準備返回的陣列
    for (size_t i = 0; i < tokens.size(); ++i) 
      std::transform(tokens[i].begin(), tokens[i].end(), tokens[i].begin(), ::tolower);
        
    return tokens;
}

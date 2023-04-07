#include <typeinfo>
#include <vector>
#include <fstream>
#include <algorithm>

#include "./UnaryOperators.h"
#include "./UnixTranslator.h"
#include "./DBMConT.h"
#include "./DBMFitFunction.h"
using namespace DDMConT;

namespace DBMMath
{
  double O[3]={0,0,0};
  double I[3]={1,1,1};
  
  void MinDIns(double &D){
    if(abs(D)<minDouble){
      D=0;
      // return 0;
    }
  }
  float findMax(float a,float b){
    float x=0;
    if (a>=b){
      x = a;
    }else{
      x = b;
    }
    return x;
  }
  float findMin(float a,float b){
    float x=0;
    if (a<=b){
      x = a;
    }else{
      x = b;
    }
    return x;
  }
  float findMax(vector<float> a){
    return *max_element(a.begin(), a.end());
  }
  float findMin(vector<float> a){
    return *min_element(a.begin(), a.end());
  }
  int findMax(int a,int b){
    int x=0;
    if (a>=b){
      x = a;
    }else{
      x = b;
    }
    return x;
  }
  int findMin(int a,int b){
    int x=0;
    if (a<=b){
      x = a;
    }else{
      x = b;
    }
    return x;
  }
  int findMax(vector<int> a){
    return *max_element(a.begin(), a.end());
  }
  int findMin(vector<int> a){
    return *min_element(a.begin(), a.end());
  }
  int Dot(vector<int> a,vector<int> b){
    int aSize = a.size();
    int bSize = b.size();
    int re = 0.;
    if (aSize != bSize){ 
      cout<< "error ouput -1" <<"\tVecA:\t"<<aSize <<"\tVecB:\t"<<bSize<<endl;
      return -1;
    };
    for (int i = 0 ; i < aSize ; i++){
      re += a[i]*b[i];
    }
    return re;
  }
  float Dot(vector<float> a,vector<float> b){
    int aSize = a.size();
    int bSize = b.size();
    float re = 0.;
    if (aSize != bSize){ 
      cout<< "error ouput -1" <<"\tVecA:\t"<<aSize <<"\tVecB:\t"<<bSize<<endl;
      return -1;
    };
    for (int i = 0 ; i < aSize ; i++){
      re += a[i]*b[i];
    }
    return re;
  }
  double Dot(vector<double> a,vector<double> b){
    int aSize = a.size();
    int bSize = b.size();
    double re = 0.;
    if (aSize != bSize){ 
      cout<< "error ouput -1" <<"\tVecA:\t"<<aSize <<"\tVecB:\t"<<bSize<<endl;
      return -1;
    };
    for (int i = 0 ; i < aSize ; i++){
      re += a[i]*b[i];
    }
    return re;
  }

  double distance(vector<double> Pa,vector<double> Pb){
    return sqrt(pow(Pa[0]-Pb[0],2)+pow(Pa[1]-Pb[1],2)+pow(Pa[2]-Pb[2],2));
  }
  double distance(double *Pa,double *Pb){
    return sqrt(pow(Pa[0]-Pb[0],2)+pow(Pa[1]-Pb[1],2)+pow(Pa[2]-Pb[2],2));
  }

  bool vectorCompare(double *Va,double *Vb){
    if(fabs(Va[0]-Vb[0])<minDouble&&fabs(Va[1]-Vb[1])<minDouble&&fabs(Va[2]-Vb[2])<minDouble){
      return true;
    }
    else // cout<<"TFV:"<<(Va[0]/Vb[0])-1<<"\t"<<(Va[1]/Vb[1])-1<<"\t"<<(Va[2]/Vb[2])-1<<endl;
    return false;
  }

  bool IntOrNot(double num){
    return (num*256.)-int(num*256.)<minDouble;
  }
  void Rxalpha(double &vector3D_0,double &vector3D_1,double &vector3D_2,double A){
    double v3DTmp[3]={vector3D_0,vector3D_1,vector3D_2};
    vector3D_0 =
       +v3DTmp[0]*(1)
       +v3DTmp[1]*(0)
       +v3DTmp[2]*(0);
    vector3D_1 =
       +v3DTmp[0]*(0)
       +v3DTmp[1]*cos(A)
       +v3DTmp[2]*sin(A);
    vector3D_2 =
       +v3DTmp[0]*(0)
       +v3DTmp[1]*-sin(A)
       +v3DTmp[2]*cos(A);
  }
  void Rybeta(double &vector3D_0,double &vector3D_1,double &vector3D_2,double B){
    double v3DTmp[3]={vector3D_0,vector3D_1,vector3D_2};
    vector3D_0 =
       +v3DTmp[0]*(cos(B))
       +v3DTmp[1]*(0)
       +v3DTmp[2]*(-sin(B));
    vector3D_1 =
       +v3DTmp[0]*(0)
       +v3DTmp[1]*(1)
       +v3DTmp[2]*(0);
    vector3D_2 =
       +v3DTmp[0]*(sin(B))
       +v3DTmp[1]*(0)
       +v3DTmp[2]*(cos(B));
  }
  void Rzgamma(double &vector3D_0,double &vector3D_1,double &vector3D_2,double G){
    double v3DTmp[3]={vector3D_0,vector3D_1,vector3D_2};
    vector3D_0 =
       +v3DTmp[0]*(cos(G))
       +v3DTmp[1]*(sin(G))
       +v3DTmp[2]*(0);
    vector3D_1 =
       +v3DTmp[0]*(-sin(G))
       +v3DTmp[1]*(cos(G))
       +v3DTmp[2]*(0);
    vector3D_2 =
       +v3DTmp[0]*(0)
       +v3DTmp[1]*(0)
       +v3DTmp[2]*(1);
  }
  void VectorRotate3D(double &vector3D_0,double &vector3D_1,double &vector3D_2,double A,double B,double G){
    if(IntOrNot(A)&&IntOrNot(B)&&IntOrNot(G)){
      A = A/180.*PI;
      B = B/180.*PI;
      G = G/180.*PI;
    }
    // cout<<"A "<<"B "<<"G "<<A<<" "<<B<<" "<<G<<"\n";
    // cout<<"A "<<"B "<<"G "<<vector3D_0<<" "<<vector3D_1<<" "<<vector3D_2<<"\n";
  
    Rzgamma(vector3D_0,vector3D_1,vector3D_2,G);
    Rybeta (vector3D_0,vector3D_1,vector3D_2,-B);
    Rxalpha(vector3D_0,vector3D_1,vector3D_2,A);
    double v3DTmp[3]={vector3D_0,vector3D_1,vector3D_2};
    MinDIns(vector3D_0);
    MinDIns(vector3D_1);
    MinDIns(vector3D_2);
    // if(vector3D_0==0&&vector3D_1==0) cout<<"VectorRotate3D not support Beta =0 and Gamma =0"<<endl;
    // cout<<endl;
  }
  void VectorRotate3D(double *vector3D,double A,double B,double G){
    VectorRotate3D(vector3D[0],vector3D[1],vector3D[2], A, B, G);
  }
  void VectorRotate3D(vector<double> vector3D,double A,double B,double G){
    VectorRotate3D(vector3D[0],vector3D[1],vector3D[2], A, B, G);
  }
}
// Cose di moduli di CMS

// Lifetime: systematic error
#include <iostream>
using namespace std;


void noise(TString file1, TString file2){

  TString ch = "SSA";

  TH1F * h[15];
  TFile *file1_open = TFile::Open(file1, "READ");
  for(int i=0; i<15; i++){
    if(i>7){
      ch="MPA";
      h[i] = (TH1F*)file1_open->Get(Form("Detector/Board_0/OpticalGroup_0/Hybrid_0/MPA_%d", ch, i)); 
    }
    else{
      ch="SSA"
      h[i] = (TH1F*)file1_open->Get(Form("Detector/Board_0/OpticalGroup_0/Hybrid_0/SSA_%d/D_B(0)_O(0)_H(0)_NoiseDistribution_Chip(%d)", i, i)); 
    }
  }
}

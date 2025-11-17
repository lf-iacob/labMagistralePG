#include <iostream>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
using namespace std;

void estrai(const char* file_name){
  //---- READ DATA AND SAVE IN TREE
  double y;
  TTree* V = new TTree("V", "Tensione");
  V -> ReadFile(file_name,"y/D");
  V -> SetBranchAddress("y",&y);
  int tot_ev = V->GetEntries();

  double parz_ev=0.3*tot_ev;
  cout<<"Dati iniziali per la baseline: "<<parz_ev<<endl;
  double ymean=0;
  for (int i=0; i<parz_ev; i++){
    V->GetEntry(i);
    ymean=ymean+y;
  }
  cout<<"Somma: "<<ymean<<endl;
  ymean=ymean/parz_ev;
  cout<<"Media: "<<ymean<<endl;

  // Dati shiftati
  double ys;
  TTree* v = new TTree("v", "Tensione shiftata");
  v->Branch("ys", &ys, "ys/D");

  for (int i=0; i<0.4*tot_ev; i++){
    V->GetEntry(i);
    ys=y-ymean;
    v->Fill();
  }
  

}

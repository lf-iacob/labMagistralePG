// --- Event analysis ---

#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
using namespace std;
const int N=100;

void estrai(
           TString file_name //file name for the data
           ){
  cout<<"-------- EVENT ANALYSIS --------"<<endl;

  //---- File ROOT
  TFile *f = new TFile("wfs.root", "RECREATE");

  //---- Read File
  ifstream file;         
  file.open(file_name);
  int a[1024];
  double b, c;
  TTree* data = new TTree("dd", "dd");
  data->Branch("A", a, "a[1024]/I");
  data->Branch("B", &b, "b/D");
  data->Branch("C", &c, "c/D");

  //da modificare perchè troppi for :)
  
  while(file.good()){
    b=0.;
    c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
      else if(i>=450 && i<=550){
	c+=(b-a[i])*2000./4096*4/50;
      }
    }
    data->Fill();
  }
  file.close();  

  f->Write();

}

//LAB: 24/11/2025
// --- Multiple event analysis (100, 10000) ---

#include <TTree.h>
#include <TFile.h>
using namespace std;
const int N=300; //data for baseline

void analizza(
			TString file_name, //file name for the data
			TString output="analysis.root" //TFile name as output
             ){

  //---- File ROOT
  TFile *f = new TFile(output, "RECREATE");

  //---- Read File
  ifstream file;         
  file.open(file_name);

  //---- Create TTree
  int a[1024];
  double b, c;
  TTree* data = new TTree("dd", "dd");
  data->Branch("A", a, "a[1024]/I");
  data->Branch("B", &b, "b/D");
  data->Branch("C", &c, "c/D");

  //---- TTree->Fill()
  while(file.good()){
    b=0.;
    c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
      else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50;
    }
    data->Fill();
  }
  file.close();  

  f->Write();
}

// -- Single File analysis dcr (different time window and output name) --

#include <iostream>
using namespace std;
const int N=100; //data for baseline
const int M=1024*8; //time window

void albero_dcr(TString filename, TString output="output.root"){

  cout<<endl<<"------> TTree single file: "<<filename<<endl<<endl;

  //---- TFile as output
  if (output == "output.root"){
    Ssiz_t pos = filename.First('_');
    TString suffix = filename(pos, filename.Length() - pos);
    suffix.ReplaceAll(".adc", ".root");
    output = "alberi/alb_dcr" + suffix;
  }
  TFile *f = new TFile(output, "RECREATE");

  //---- Create TTree
  int a[M];
  double b, c;
  //double amp;
  TTree *dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[8192]/I");         // ---------- MODIFICA TIME WINDOW!!!!!!!!!!!
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");
  //dd->Branch("AMP", &amp, "amp/D");

  //---- TTree->Fill()
  ifstream file;
  file.open(filename);
  while(file.good()){
    b=0.; c=0.; //amp=0;
    for(int i=0;i<M;i++){
      file>>a[i]; 
      if(i<N) b+=double(a[i])/N;
      //if((b-a[i])>amp) amp = b-a[i];
    }
    dd->Fill();
  }
  file.close();  

  
  //---- Saving in TFile
  f->Write();
}




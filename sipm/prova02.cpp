// --- Multiple event analysis: FIT for gain ---

#include <iostream>
#include <TTree.h>
#include <TFile.h>
using namespace std;
const int N=100; //data for baseline

void analizza(
              TString file_name, //file name for the data
              TString output="analysis.root" //TFile name as output
             ){
  cout<<endl<<"---- MULTIPLE EVENT ANALYSIS ----"<<endl<<endl;
    
  //---- File ROOT
  TFile *f = new TFile(output, "RECREATE");

  //---- Read File
  ifstream file;
  file.open(file_name);

  //---- Create TTree
  int a[1024];
  double b, c;
  TTree* dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[1024]/I");
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");

  //---- TTree->Fill()
  while(file.good()){
    b=0.;
    c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
      else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50;
    }
    dd->Fill();
  }
  file.close();

  int n = dd->GetEntries(); //#events collected

  //---- Charge THistogram
  TH1F *hc = new TH1F("hc", "Charge Histogram;Charge (pC);Counts",500,-50,300);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hc->Fill(c);
  }

  //---- Gaussian Fit
  double m0, m1;
  TF1 *g = new TF1("g", "gaus");
  hc->Fit(g, "N", "", -3.8, 6); //0pe
  m0 = g->GetParameter(1);
  hc->Fit(g, "N", "", 9.5, 20); //1pe
  m1 = g->GetParameter(1);
  cout<<endl<<"-- Charge estimation --"<<endl<<"0pe: "<<m0<<endl<<"1pe: "<<m1<<endl;

  //---- Gain
  double gain=(m1-m0)/1.6e-7/pow(10., 32./20.); //pre-amp:32dB
  cout<<"Gain: "<<gain<<endl<<endl;

  f->Write();
  f->Close();

}


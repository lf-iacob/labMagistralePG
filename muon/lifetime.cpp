// Vita media dei muoni

using namespace std;
#include <iostream>
const int L=8*1024;
const int N=100;

void lt(TString filename1, TString filename2, TString output="output.root"){
 
  TFile *f = new TFile(output, "RECREATE");
  
  int a1[1024*8], a2[1024*8], t1, t2, t, s=1200;
  double b1, b2;
  TTree *dd = new TTree("dd", "dd");
  dd->Branch("A1", a1, "a1[8192]/I");
  dd->Branch("B1", &b1, "b1/D");
  dd->Branch("T1", &t1, "t1/I");
  dd->Branch("A2", a2, "a2[8192]/I");
  dd->Branch("B2", &b2, "b2/D");
  dd->Branch("T2", &t2, "t2/I");
  dd->Branch("T", &t, "t/I");
  
  ifstream file1, file2;
  file1.open(filename1);
  file2.open(filename2);
  while(file1.good() && file2.good()){
    b1=0.; b2=0.;
    for(int i=0;i<L;i++){
      file1>>a1[i];
      file2>>a2[i];
      if(i<N){
	b1+=double(a1[i])/N;
        b2+=double(a2[i])/N;
      }
    }
    for(int i=1; i<L; i++){
      if(((b1-a1[i])>s)){
	t1=i*4;
	break;
      } 
    }
     for(int i=1; i<L; i++){
      if(((b2-a2[i])>s)){
	t2=i*4;
	break;
      } 
    } 
    t=abs(t2-t1);
    dd->Fill();
  }
  file1.close();
  file2.close();

  int n = dd->GetEntries();

  //time histogram
  TH1I *ht = new TH1I("ht", "Time Histogram;Time (ns);Entries",500,600,25000);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    ht->Fill(t);
  }

  TF1 *ee = new TF1("ee","[0]*exp(-x/[1])+[2]", 600, 25000);
  gStyle->SetOptFit(0111);
  ee->SetParameters(100, 2000, 10);
  double tau, tau_err;
  ht -> Fit(ee); 
  tau=ee->GetParameter(1);
  tau_err=ee->GetParError(1);
  
  cout << "Tau = " << tau << " +/- " << tau_err << endl;

  ht->Write();
  ee->Write();
  f->Write();

}

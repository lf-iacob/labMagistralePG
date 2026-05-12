// Tau stima finale con errori sistematici

using namespace std;
#include <iostream>
const int L=8*1024;
const int N=100;
const int P=5;

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
  TH1I *ht[P][P];
  for(int j=0; j<P; j++){
    for(int k=0; k<P; k++) {
      TString nameh = Form("ht%d%d",j,k);
      ht[j][k]= new TH1I(nameh, "Time Histogram;Time (ns);Entries",300+200*j,600+100*k,23400);
      for(int i=0; i<n; i++){
	dd->GetEntry(i);
	ht[j][k]->Fill(t);
      }
    }
  }

  //time fit
  TF1 *ee[P][P];
  double tau[P][P], tau_err[P][P];
  for(int j=0; j<P; j++){
    for(int k=0; k<P; k++){
      TString namef = Form("ee%d%d",j,k);
      ee[j][k]= new TF1(namef,"[0]*exp(-x/[1])+[2]", 600+100*k, 23400);
      gStyle->SetOptFit(0111);
      ee[j][k]->SetParameters(500, 2000, 100);  //down:500, 2000, 300
      ht[j][k]->Fit(ee[j][k],"Q0");
      tau[j][k]=ee[j][k]->GetParameter(1);
      tau_err[j][k]=ee[j][k]->GetParError(1);
      ee[j][k]->Write();
    }
  }

  //total hist
  TH1I *h_tau = new TH1I("htau", "Tau Histogram;Tau (ns);Entries",17,2100,2300);
  for(int l=0; l<P; l++) {
    for(int m=0; m<P; m++){
      h_tau->Fill(tau[l][m]);
    }
  }
  double err_sist = h_tau->GetStdDev();
  h_tau->Draw();

  h_tau->Write();
  f->Write();

}


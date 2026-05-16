// Muon lifetime
using namespace std;
#include <iostream>
const int L=8*1024;
const int N=100;

void lt(TString filename1, TString filename2, TString output="output.root"){

  TString ele_type = filename1;
  ele_type.ReplaceAll("1.adc", "");
  cout<<"DATATYPE: Electrons "<<ele_type<<endl<<endl;
  int par_norm = 100, par_plateau = 10, col_hist = kAzure-5, col_fit = kPink-3;
  if(ele_type=="down"){
    par_norm=500;
    par_plateau=300;
    col_hist=kSpring-1;
    col_fit= kViolet+1;
  }
    
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
  TCanvas *r = new TCanvas();
 
  TH1I *ht = new TH1I("ht", "Time Histogram;Time (ns);Entries",300,600,23400);
  gStyle->SetOptStat(0);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    ht->Fill(t);
  }

  TF1 *ee = new TF1("ee","[0]*exp(-x/[1])+[2]", 600, 23400);
  gStyle->SetOptFit(0);
  ee->SetParameters(par_norm, 2000, par_plateau);
  double tau, tau_err, norm, plateau, chi2, gdl;
  ht->SetLineColor(col_hist);
  //ht->SetLineWidth(2);
  ht->SetFillColor(col_hist);
  ht->SetFillStyle(3005);
  ee->SetLineColor(col_fit);
  ee->SetLineWidth(3);
  ht->Fit(ee);
  
  tau=ee->GetParameter(1);
  tau_err=ee->GetParError(1);
  norm = ee->GetParameter(0);
  plateau = ee->GetParameter(2);
  chi2 = ee->GetChisquare();
  gdl = ee->GetNDF();

  TLegend *leg = new TLegend(0.4, 0.65, 0.88, 0.88);
  leg->SetTextSize(0.03);
  leg->AddEntry(ht, Form("Electrons type: %s", ele_type.Data()), "");
  leg->AddEntry(ht, Form("Bins = %d ; Range (ns) = (%d, %d)", 300, 600, 23400), "f");
  leg->AddEntry(ee, Form("Lifetime #tau (#mus) = %.2f #pm %.2f", tau/1000, tau_err/1000), "l");
  leg->AddEntry(ee, Form("#chi^{2}/NDf = %.2f/%.0f", chi2, gdl), "");
  leg->AddEntry(ee, Form("Norm = %.2f ; Plateau = %.2f", norm, plateau), "");
  leg -> Draw();
  r->Update();
  
  cout<<"Tau = "<<tau<<" +/- "<<tau_err<<endl;

  r->Write();
  ht->Write();
  ee->Write();
  f->Write();

}

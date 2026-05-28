// Prova 2 exp nel fit
using namespace std;
#include <iostream>
const int L=8*1024;
const int N=100;

void lt(TString fileroot, TString output="output.root"){

  TString ele_type = fileroot;
  ele_type.ReplaceAll(".root", "");
  ele_type.ReplaceAll("tree_", "");
  cout<<"DATATYPE: Electrons "<<ele_type<<endl<<endl;
  if(output=="output.root") output="prova2exp_lt_" + ele_type + ".root";
  int par_plateau = 10, col_hist = kAzure-5, col_fit = kPink-3;
  if(ele_type=="down"){
    par_plateau=300;
    col_hist=kSpring-1;
    col_fit= kViolet+1;
  }

  TFile *file_aperto = TFile::Open(fileroot);
  TTree *dd = (TTree*)file_aperto->Get("dd");
  int a1[1024*8], a2[1024*8], t1, t2, t, s=1200;
  double b1, b2;
  dd->SetBranchAddress("A1", a1);
  dd->SetBranchAddress("B1", &b1);
  dd->SetBranchAddress("T1", &t1);
  dd->SetBranchAddress("A2", a2);
  dd->SetBranchAddress("B2", &b2);
  dd->SetBranchAddress("T2", &t2);
  dd->SetBranchAddress("T", &t);
  int n = dd->GetEntries();

  TFile *f = new TFile(output, "RECREATE");

  //time histogram
  TCanvas *r = new TCanvas();
 
  TH1I *ht = new TH1I("ht", "Time Histogram;Time (ns);Entries",300,600,23400);
  gStyle->SetOptStat(0);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    ht->Fill(t);
  }
  ht->SetLineColor(col_hist);
  ht->SetFillColor(col_hist);
  ht->SetFillStyle(3005);
  
  TF1 *ee = new TF1("ee","[0]*exp(-x/[1])+[2]*exp(-x/[3])", 600, 23400);
  gStyle->SetOptFit(0);
  ee->SetParameters(100, 2000, 100, 50000000);
  double tau1, tau1_err, norm1, norm2, tau2, tau2_err, chi2, gdl;
  ee->SetLineColor(col_fit);
  ee->SetLineWidth(3);
  ht->Fit(ee);
  
  tau1=ee->GetParameter(1);
  tau1_err=ee->GetParError(1);
  norm1 = ee->GetParameter(0);
  norm2 = ee->GetParameter(2);
  tau2=ee->GetParameter(3);
  tau2_err=ee->GetParError(3);
  chi2 = ee->GetChisquare();
  gdl = ee->GetNDF();

  TLegend *leg = new TLegend(0.4, 0.65, 0.88, 0.88);
  leg->SetTextSize(0.03);
  leg->AddEntry((TObject*)nullptr, Form("Electrons type: %s", ele_type.Data()), "");
  leg->AddEntry(ht, Form("Bins = %d ; Range (ns) = (%d, %d)", 300, 600, 23400), "f");
  leg->AddEntry(ee, Form("Lifetime #tau_1 (#mus) = %.2f #pm %.2f", tau1/1000, tau1_err/1000), "l");
  TLegendEntry *entry = leg->AddEntry((TObject*)nullptr, Form("Noise #tau_2 (ms) = %.2f #pm %.2f", tau2/1000000, tau2_err/1000000), "l");
  entry->SetLineColor(kMagenta+3);
  entry->SetLineWidth(3);
  leg->AddEntry((TObject*)nullptr, Form("#chi^{2}/NDf = %.2f/%.0f", chi2, gdl), "");
  leg->AddEntry((TObject*)nullptr, Form("N_1 = %.2f ; N_2 = %.2f", norm1, norm2), "");
  leg -> Draw();
  r->Update();
  
  cout<<"Tau_1 = "<<tau1<<" +/- "<<tau1_err<<endl;
  cout<<"Tau_2= "<<tau2<<" +/- "<<tau2_err<<endl;
  
  f->cd();
  r->Write();
  ht->Write();
  ee->Write();
  f->Write();
  file_aperto->Close();
}

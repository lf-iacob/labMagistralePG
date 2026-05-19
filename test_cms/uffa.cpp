// Noise evaluation CMS modules

#include <iostream>
using namespace std;
const int N = 16;

void noise(TString file1, TString file2, TString output="output.root"){

  TFile *f = new TFile(output, "RECREATE");
  
  double channels[N] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

  TString ch_type = "SSA";  
  TFile *file1_open = TFile::Open(file1, "READ");
  TH1F *h_f1_hy0[N], *h_f1_hy1[N];
  for(int i=0; i<N; i++){
    if(i==8) ch_type="MPA";
    h_f1_hy0[i] = (TH1F*)file1_open->Get(Form("Detector/Board_0/OpticalGroup_0/Hybrid_0/%s_%d/D_B(0)_O(0)_H(0)_NoiseDistribution_Chip(%d)", ch_type.Data(), i, i));
    h_f1_hy1[i] = (TH1F*)file1_open->Get(Form("Detector/Board_0/OpticalGroup_0/Hybrid_1/%s_%d/D_B(0)_O(0)_H(1)_NoiseDistribution_Chip(%d)", ch_type.Data(), i, i));
  }

  ch_type = "SSA";
  TFile *file2_open = TFile::Open(file2, "READ");
  TH1F *h_f2_hy0[N], *h_f2_hy1[N];
  for(int i=0; i<N; i++){
    if(i==8) ch_type="MPA";
    h_f2_hy0[i] = (TH1F*)file2_open->Get(Form("Detector/Board_0/OpticalGroup_0/Hybrid_0/%s_%d/D_B(0)_O(0)_H(0)_NoiseDistribution_Chip(%d)", ch_type.Data(), i, i));
    h_f2_hy1[i] = (TH1F*)file2_open->Get(Form("Detector/Board_0/OpticalGroup_0/Hybrid_1/%s_%d/D_B(0)_O(0)_H(1)_NoiseDistribution_Chip(%d)", ch_type.Data(), i, i));
  }

  double n_f1_hy0[N], n_f1_hy1[N], n_f2_hy0[N], n_f2_hy1[N],
    err_n_f1_hy0[N], err_n_f1_hy1[N], err_n_f2_hy0[N], err_n_f2_hy1[N];
  for(int j=0; j<N; j++){
    n_f1_hy0[j]=h_f1_hy0[j]->GetMean();
    n_f1_hy1[j]=h_f1_hy1[j]->GetMean();
    n_f2_hy0[j]=h_f2_hy0[j]->GetMean();
    n_f2_hy1[j]=h_f2_hy1[j]->GetMean();
    err_n_f1_hy0[j]=h_f1_hy0[j]->GetMeanError();
    err_n_f1_hy1[j]=h_f1_hy1[j]->GetMeanError();
    err_n_f2_hy0[j]=h_f2_hy0[j]->GetMeanError();
    err_n_f2_hy1[j]=h_f2_hy1[j]->GetMeanError();
  }

  f->cd();
  TCanvas *c = new TCanvas;
  TGraphErrors *n10 = new TGraphErrors(N, &channels[0], &n_f1_hy0[0], nullptr, &err_n_f1_hy0[0]);
  n10->SetMarkerColor(kRed);
  n10->SetLineColor(kRed);
  TGraphErrors *n11 = new TGraphErrors(N, &channels[0], &n_f1_hy1[0], nullptr, &err_n_f1_hy1[0]);
  n11->SetMarkerColor(kOrange+1);
  n11->SetLineColor(kOrange+1);
  TGraphErrors *n20 = new TGraphErrors(N, &channels[0], &n_f2_hy0[0], nullptr, &err_n_f2_hy0[0]);
  n20->SetMarkerColor(kAzure+2);
  n20->SetLineColor(kAzure+2);
  TGraphErrors *n21 = new TGraphErrors(N, &channels[0], &n_f2_hy1[0], nullptr, &err_n_f2_hy1[0]);
  n21->SetMarkerColor(kTeal+2);
  n21->SetLineColor(kTeal+2);

  TLegend *leg = new TLegend(0.65, 0.7, 0.88, 0.88);
  leg->AddEntry(n10, "File1, Hybrid0", "LEP");
  leg->AddEntry(n11, "File1, Hybrid1", "LEP");
  leg->AddEntry(n20, "File2, Hybrid0", "LEP");
  leg->AddEntry(n21, "File2, Hybrid1", "LEP");

  c->cd();
  n21->Draw("ALP");
  n11->Draw("LP SAME");
  n20->Draw("LP SAME");
  n10->Draw("LP SAME");
  leg->Draw();

  c->Write();
  f->Write();
  file1_open->Close();
  file2_open->Close();
}

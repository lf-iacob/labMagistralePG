// Lifetime: systematic error
#include <iostream>
using namespace std;
const int L = 8 * 1024;
const int N = 100;
const int P = 15;

void sys(TString fileroot, TString output="output.root") {

  TString ele_type = fileroot;
  ele_type.ReplaceAll(".root", "");
  ele_type.ReplaceAll("tree_", "");
  cout<<"DATATYPE: Electrons "<<ele_type<<endl<<endl;
  if(output=="output.root") output="sys_" + ele_type + ".root";
  int par_plateau=10, col_hist=kAzure-5, col_fit=kPink-3, col_3d=kDeepSea;
  if (ele_type == "down") {
    par_plateau=300;
    col_hist=kSpring-1;
    col_fit=kViolet+1;
    col_3d=kAvocado;
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

  //time histograms
  TH1I *ht[P][P];
  for(int j=0; j<P; j++){
    for(int k=0; k<P; k++) {
      TString nameh = Form("ht%d%d",j,k);
      ht[j][k]= new TH1I(nameh, "Time Histogram;Time (ns);Entries",300+200*j,600+50*k,23400);
      for(int i=0; i<n; i++){
	dd->GetEntry(i);
	ht[j][k]->Fill(t);
	ht[j][k]->SetDirectory(0);
      }
    }
  }

  //time fit
  TF1 *ee[P][P];
  double tau[P][P], tau_err[P][P];
  for(int j=0; j<P; j++){
    for(int k=0; k<P; k++){
      TString namef = Form("ee%d%d",j,k);
      ee[j][k]= new TF1(namef,"[0]*exp(-x/[1])+[2]", 600+50*k, 23400);
      ee[j][k]->SetParameters(400, 2000, par_plateau);
      ht[j][k]->Fit(ee[j][k],"Q0");
      tau[j][k]=ee[j][k]->GetParameter(1);
      tau_err[j][k]=ee[j][k]->GetParError(1);
    }
  }
  
  // 3D Graph
  TCanvas *c2d_1 = new TCanvas();
  TH2D *h2_tau = new TH2D("h2_tau", "#tau histogram;Binning;Width Range;#tau (ns)", 
			  P, 300, 3300,  
			  P, 22050, 22800);
  for(int j=0; j<P; j++) {
    for(int k=0; k<P; k++) {
      h2_tau->SetBinContent(j+1, k+1, tau[j][k]);
    }
  }
  gStyle->SetPalette(col_3d);
  c2d_1->cd();
  h2_tau->Draw("COLZ");
  h2_tau->Draw("LEGO2");
  // *****
  TGraph2D *g2_tau = new TGraph2D();
  g2_tau->SetName("g2_tau");
  g2_tau->SetTitle("#tau vs Binning e Range;Bins;Range width (ns);#tau (ns)");
  int nPoint = 0;
  for(int j=0; j<P; j++) {
    for(int k=0; k<P; k++) {
      g2_tau->SetPoint(nPoint, (300+200*j), (23400-600-50*k), tau[j][k]);
      nPoint++;
    }
  }
  TCanvas *c2d_2 = new TCanvas();
  g2_tau->SetMarkerStyle(20);
  g2_tau->SetMarkerColor(kBlue);
  gStyle->SetPalette(col_3d);
  c2d_2->cd();
  g2_tau->Draw("SURF1");
  g2_tau->Draw("P SAME");
  g2_tau->Write();
  
  //total hist
  TCanvas *c = new TCanvas();
  TH1I *h_tau = new TH1I("htau", "Tau Histogram;Tau (ns);Entries",100,2000,2400);
  for(int l=0; l<P; l++) {
    for(int m=0; m<P; m++){
      h_tau->Fill(tau[l][m]);
    }
  }
  c->cd();
  h_tau->Draw();
  double err_syst = h_tau->GetStdDev();
  cout<<"Systematic error (ns) = "<<err_syst<<endl;

  f->cd();
  h_tau->Write();
  h2_tau->Write();
  g2_tau->Write();
  c2d_1->Write();
  c2d_2->Write();
  c->Write();
  f->Write();
  file_aperto->Close();
}


// Lifetime: systematic error
#include <iostream>
using namespace std;
const int L = 8 * 1024;
const int N = 100;
const int P = 10;

void ProgressBar(Long64_t current, Long64_t total) {
    if (total <= 0) return;
    int barWidth = 70;
    float progress = (float)current / total;

    fprintf(stdout, "\r[");
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) fprintf(stdout, "=");
        else if (i == pos) fprintf(stdout, ">");
        else fprintf(stdout, " ");
    }
    fprintf(stdout, "] %d%% ", int(progress * 100));
    fflush(stdout);
}

void sys(TString fileroot, TString output="output.root") {

  TString ele_type = fileroot;
  ele_type.ReplaceAll(".root", "");
  ele_type.ReplaceAll("tree_", "");
  cout<<"DATATYPE: Electrons "<<ele_type<<endl<<endl;
  if(output=="output.root") output="sys_" + ele_type + "_P" + P + ".root";
  int par_plateau=10, col_hist=kAzure-5, col_3d=kDeepSea, col_mark=kBlue;
  if (ele_type == "down") {
    par_plateau=300;
    col_hist=kSpring-1;
    col_3d=kAvocado;
    col_mark=kGreen;
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

  cout << "Riempimento istogrammi in corso..." << endl;
  Long64_t total_ops = (Long64_t)P * P * n;
  Long64_t current_op = 0;

  //time histograms
  TH1I *ht[P][P];
  for(int j=0; j<P; j++){
    for(int k=0; k<P; k++) {
      TString nameh = Form("ht%d%d",j,k);
      ht[j][k]= new TH1I(nameh, "Time Histogram;Time (ns);Entries",300+400*j,600+150*k,23400); //200, 50
      for(int i=0; i<n; i++){
	dd->GetEntry(i);
	ht[j][k]->Fill(t);

	current_op++;   
	if (current_op % 500 == 0) ProgressBar(current_op, total_ops);
      }
      ht[j][k]->SetDirectory(0);
    }
  }
  ProgressBar(total_ops, total_ops);
  cout << endl << "Riempimento completato." << endl << endl;
  cout << "Esecuzione dei Fit..." << endl;

  //time fit
  TF1 *ee[P][P];
  double tau[P][P], tau_err[P][P];
  for(int j=0; j<P; j++){
    for(int k=0; k<P; k++){
      TString namef = Form("ee%d%d",j,k);
      ee[j][k]= new TF1(namef,"[0]*exp(-x/[1])+[2]", 600+150*k, 23400);
      ee[j][k]->SetParameters(400, 2000, par_plateau);
      ht[j][k]->Fit(ee[j][k],"Q0");
      tau[j][k]=ee[j][k]->GetParameter(1);
      tau_err[j][k]=ee[j][k]->GetParError(1);
    }
  }
  
  // 3D Graph
  /*TCanvas *c2d_1 = new TCanvas();
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
  f->cd();
  h2_tau->Write();
  c2d_1->Write();*/
  // *****
  TGraph2D *g2_tau = new TGraph2D();
  g2_tau->SetName("g2_tau");
  g2_tau->SetTitle("#tau vs Binning et Range;Bins;Range width (ns);#tau (ns)");
  int nPoint = 0;
  for(int j=0; j<P; j++) {
    for(int k=0; k<P; k++) {
      if(tau[j][k]>1800){
	g2_tau->SetPoint(nPoint, (300+400*j), (23400-600-150*k), tau[j][k]);
	nPoint++;
      }
    }
  }
  TCanvas *c2d_2 = new TCanvas();
  g2_tau->SetMarkerStyle(20);
  g2_tau->SetMarkerColor(col_mark);
  gStyle->SetPalette(col_3d);
  c2d_2->cd();
  g2_tau->Draw("SURF1");
  g2_tau->Draw("P SAME");
  g2_tau->Write();
  
  //total hist
  TCanvas *c = new TCanvas();
  TH1I *h_tau = new TH1I("htau", "Tau Histogram;Tau (ns);Entries",70,1800,2400);
  for(int l=0; l<P; l++) {
    for(int m=0; m<P; m++){
      if(tau[l][m]>1800) h_tau->Fill(tau[l][m]);
    }
  }
  h_tau->SetLineWidth(3);
  h_tau->SetLineColor(col_hist);
  h_tau->SetFillColor(col_hist);
  h_tau->SetFillStyle(3005);
  c->cd();
  h_tau->Draw();
  double err_syst = h_tau->GetStdDev();
  cout << "------------------------------------------" << endl;
  cout << "Systematic error (ns) = "<<err_syst<<endl;
  cout << "------------------------------------------" << endl;

  f->cd();
  h_tau->Write();
  g2_tau->Write();
  c2d_2->Write();
  c->Write();
  f->Write();
  file_aperto->Close();
}


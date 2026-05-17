// Systematic error on tau
using namespace std;
#include <iostream>
const int L=8*1024;
const int N=100;
const int P=15;

void lt(TString filename1, TString filename2, TString output="output.root"){

  TString ele_type = filename1;
  ele_type.ReplaceAll("1.adc", "");
  cout<<"DATATYPE: Electrons "<<ele_type<<endl<<endl;
  int par_norm = 100, par_plateau = 10, col_hist = kAzure-5, col_fit = kPink-3, col_3d = kDeepSea;
  if(ele_type=="down"){
    par_norm=500;
    par_plateau=300;
    col_hist=kSpring-1;
    col_fit= kViolet+1;
    col_3d = kAvocado;
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
  TH1I *ht[P][P];
  for(int j=0; j<P; j++){
    for(int k=0; k<P; k++) {
      TString nameh = Form("ht%d%d",j,k);
      ht[j][k]= new TH1I(nameh, "Time Histogram;Time (ns);Entries",300+200*j,600+50*k,23400);
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
      ee[j][k]= new TF1(namef,"[0]*exp(-x/[1])+[2]", 600+50*k, 23400);
      ee[j][k]->SetParameters(par_norm, 2000, par_plateau);
      ht[j][k]->Fit(ee[j][k],"Q0");
      tau[j][k]=ee[j][k]->GetParameter(1);
      tau_err[j][k]=ee[j][k]->GetParError(1);
    }
  }
  
  // 3D Graph
  TH2D *h2_tau = new TH2D("h2_tau", "#tau histogram;Binning;Width Range;#tau (ns)", 
			  P, 200, 1200,  
			  P, 22750, 23350);
  for(int j=0; j<P; j++) {
    for(int k=0; k<P; k++) {
      h2_tau->SetBinContent(j+1, k+1, tau[j][k]);
    }
  }
  gStyle->SetPalette(col_3d);
  h2_tau->Draw("COLZ");
  h2_tau->Draw("LEGO2");
  h2_tau->Write();
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
  TCanvas *c2D = new TCanvas();
  g2_tau->SetMarkerStyle(20);
  g2_tau->SetMarkerColor(kBlue);
  gStyle->SetPalette(col_3d);
  g2_tau->Draw("SURF1");
  g2_tau->Draw("P SAME");
  g2_tau->Write();
  
  //total hist
  TH1I *h_tau = new TH1I("htau", "Tau Histogram;Tau (ns);Entries",17,2100,2300);
  for(int l=0; l<P; l++) {
    for(int m=0; m<P; m++){
      h_tau->Fill(tau[l][m]);
    }
  }
  h_tau->Draw();
  double err_syst = h_tau->GetStdDev();
  cout<<"Systematic error (ns) = "<<err_syst<<endl;
  
  h_tau->Write();
  f->Write();

}


// --- Multiple event analysis: fit for snr, sspe ---
// snr: signal-to-noise ratio
// sspe: sensibilità a singolo fotoelettrone

#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
using namespace std;
const int N=100; //data for baseline

void snr(
         TString file_name1, TString file_name2, TString file_name3, TString file_name4, TString file_name5,
         TString output="analysis.root" //TFile name as output
        ){
  cout<<endl<<"------ MULTIPLE EVENT ANALYSIS: SNR, SSPE ------"<<endl<<endl;

  //---- Data in TTree
  TFile *f = new TFile(output, "RECREATE");

  int a[1024];
  double b, c;
  TTree* dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[1024]/I");
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");

  ifstream file;
  TString files[5] = {file_name1, file_name2, file_name3, file_name4, file_name5};
  for(int k=0; k<5; k++){
    file.open(files[k]);
    while(file.good()){
      b=0.; c=0.;
      for(int i=0;i<1024;i++){
	file>>a[i];
	if(i<N) b+=double(a[i])/N;
	else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50; //hard-coded
      }
      dd->Fill();
    }
    file.close();
  }

  int n = dd->GetEntries(); //#events collected

  //---- Charge THistogram
  TH1F *hc = new TH1F("hc", "Charge Histogram;Charge (pC);Counts",500,-50,300); //hard-coded
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hc->Fill(c);
  }
  
  TCanvas *hc_fit = new TCanvas();
  gPad->SetTicks(1,1);
  hc->Draw();
  hc->SetLineColor(kBlack);
  hc->SetLineWidth(3);
  
  //---- Gaussian Fit
  cout<<"--- Gaussian fit ---"<<endl;
  double ch[7], s[7], chirGauss[7], pv[7];
  
  double range_i[7] = {-3.4, 12, 28, 44, 59, 75, 90};
  double range_f[7] = { 3.4, 18, 34, 49, 66, 81, 96};

  TF1 *gfit[7];
  for(int j=0; j<7; j++){
    gfit[j] = new TF1("gfit", "gaus", -20, 140); //hard-coded
    hc->Fit(gfit[j], "QS", " ", range_i[j], range_f[j]); //j_pe
    ch[j] = gfit[j]->GetParameter(1);
    s[j] = gfit[j]->GetParameter(2);
    chirGauss[j] = gfit[j]->GetChisquare()/gfit[j]->GetNDF();
    pv[j] = TMath::Prob(gfit[j]->GetChisquare(),gfit[j]->GetNDF());
    cout<<j<<"pe => Charge (pC): "<<ch[j]<<", Sigma (pC): "<<s[j]<<", X2r: "<<chirGauss[j]<<" -> p_value: "<<pv[j]<<" -- ["<<range_i[j]<<";"<<range_f[j]<<"] --"<<endl;

    gfit[j]->SetLineColor(kTeal + j);
    gfit[j]->SetLineWidth(3);
    gfit[j]->SetNpx(750);
    gfit[j]->Draw("same");
    hc_fit->Update();
  }
  hc_fit->Write();
  cout<<endl;

  //---- SNR, SSPE
  double snr = (ch[1]-ch[0])/s[0];
  double sspe = (ch[1]-ch[0])/ sqrt(s[0]*s[0]+s[1]*s[1]);
  cout<<"SNR: "<<snr<<"; SSPE: "<<sspe<<endl<<endl;
  
  //---- Saving in TFile
  f->Write();
  //f->Close();

}


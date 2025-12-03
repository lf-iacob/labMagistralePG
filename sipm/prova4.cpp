// --- Multiple event analysis: FIT for gain ---

#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TGraph.h>
#include <TCanvas.h>
using namespace std;
const int N=100; //data for baseline

void analizza(
              TString file_name1, TString file_name2, TString file_name3, TString file_name4, TString file_name5,
              TString output="analysis.root" //TFile name as output
             ){
  cout<<endl<<"------ MULTIPLE EVENT ANALYSIS ------"<<endl<<endl;
    
  //---- File ROOT
  TFile *f = new TFile(output, "RECREATE");

  //---- Create TTree
  int a[1024];
  double b, c;
  TTree* dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[1024]/I");
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");

  //---- TTree->Fill()
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
  TH1F *hc = new TH1F("hc", "Charge Histogram;Charge (pC);Counts",500,-50,300);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hc->Fill(c);
  }

  TH1F *hcf = (TH1F*)hc->Clone("hc_f");
  
  TCanvas *hc_fit = new TCanvas();
  gPad->SetTicks(1,1);
  hcf->Draw();
  hcf->SetLineColor(kBlack);
  hcf->SetLineWidth(3);
  
  //---- Gaussian Fit
  cout<<"--- Gaussian fit ---"<<endl;
  double ch[7], ech[7], m[7], em[7], chirGauss[7], pv[7];
  double pe[7]={0,1,2,3,4,5,6}, epe[7]={0,0,0,0,0,0,0}; //TGraph vuole i double :/
  
  double range_i[7] = {-3.4, 12, 28, 44, 59, 75, 90};
  double range_f[7] = { 3.4, 18, 34, 49, 66, 81, 96};

  TF1 *gfit[7];
  for(int j=0; j<7; j++){
    gfit[j] = new TF1("gfit", "gaus", -20, 140); //hard-coded
    hcf->Fit(gfit[j], "QS", " ", range_i[j], range_f[j]); //j_pe
    ch[j] = gfit[j]->GetParameter(1);
    ech[j] = gfit[j]->GetParError(1);
    m[j] = ch[j]/1.6e-7/pow(10, 32./20.); //gain
    em[j] = ech[j]/1.6e-7/pow(10, 32./20.); //error gain
    chirGauss[j] = gfit[j]->GetChisquare()/gfit[j]->GetNDF();
    pv[j] = TMath::Prob(gfit[j]->GetChisquare(),gfit[j]->GetNDF());
    cout<<j<<"pe => Charge (pC): "<<ch[j]<<"+-"<<ech[j]<<", Gain: "<<m[j]<<"+-"<<em[j]<<", X2r: "<<chirGauss[j]<<" -> p_value: "<<pv[j]<<" -- ["<<range_i[j]<<";"<<range_f[j]<<"] --"<<endl;
    //drawings
    gfit[j]->SetLineColor(kTeal + j);
    gfit[j]->SetLineWidth(3);
    gfit[j]->SetNpx(750); //numero di punti in cui sono valutate le gaussiane
    gfit[j]->Draw("same");
    hc_fit->Update();
  }
  hc_fit->Write();
  cout<<endl;
  
  //---- Plot (linear)
  cout<<"--- Linear fit ---"<< endl;
  TGraphErrors *gr = new TGraphErrors(6, pe, m, epe, em);
  gr->SetTitle("Gain plot;pe;Gain");
  gr->SetMarkerStyle(8);
  gr->SetMarkerSize(0.1);
  gr->SetMarkerColor(kBlue);

  TCanvas *cgain = new TCanvas();
  gStyle->SetOptFit(1111);
  gPad->SetTicks(1,1);
  cgain->SetGrid();
  gr->Draw("AP");

  //---- Fit plot (linear)
  TF1 *l = new TF1("l", "[0]+[1]*x", -0.5, 6.5);
  l->SetLineColor(kRed);
  l->SetLineWidth(2);
  gr->Fit(l, "R");

  double A = l->GetParameter(0);
  double eA = l->GetParError(0);
  double B = l->GetParameter(1);
  double eB = l->GetParError(1);
  gr->Draw("AP");
  l->Draw("same");

  cout<<endl<<"Coefficiente angolare (Gain) = "<<B<<" +- "<<eB<<endl;
  cout<<"Intercetta = "<<A<<" +- "<<eA<<endl<<endl;

  cgain->Write();
  
  //---- Saving in TFile
  f->Write();
  //f->Close();

}



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
  cout<<endl<<"---- MULTIPLE EVENT ANALYSIS ----"<<endl<<endl;
    
  //---- File ROOT
  TFile *f = new TFile(output, "RECREATE");

  //---- Read File
  ifstream file;
  file.open(file_name1);

  //---- Create TTree
  int a[1024];
  double b, c;
  TTree* dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[1024]/I");
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");

  
  //---- TTree->Fill()
  while(file.good()){
    b=0.;
    c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
      else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50;
    }
    dd->Fill();
  }
  file.close();

  file.open(file_name2);
  while(file.good()){
    b=0.;
    c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
      else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50;
    }
    dd->Fill();
  }
  file.close();


  file.open(file_name3);
  while(file.good()){
    b=0.;
    c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
      else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50;
    }
    dd->Fill();
  }
  file.close();

  file.open(file_name4);
  while(file.good()){
    b=0.;
    c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
      else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50;
    }
    dd->Fill();
  }
  file.close();


  file.open(file_name5);
  while(file.good()){
    b=0.;
    c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
      else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096*4/50;
    }
    dd->Fill();
  }
  file.close();

  int n = dd->GetEntries(); //#events collected

  
  //---- Gain THistogram
  TH1F *hg = new TH1F("hg", "Gain Histogram;Gain;Counts",5000,-50*pow(10,6),300*pow(10,6));
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hg->Fill(c/1.6e-7/pow(10, 32./20.));
  }


  //---- Gaussian Fit
  double m[7], em[7];
  TF1 *g = new TF1("g", "gaus");
  hg->Fit(g, "N", "", -500000, 500000); //0pe
  m[0] = g->GetParameter(1);
  em[0] = g->GetParError(1);
  hg->Fit(g, "N", "", 2000000, 3000000); //1pe
  m[1] = g->GetParameter(1);
  em[1] = g->GetParError(1);
  hg->Fit(g, "N", "", 4500000, 5300000);  //2pe
  m[2] = g->GetParameter(1);
  em[2]= g->GetParError(1);
  hg->Fit(g, "N", "", 7000000, 7700000);  //3pe
  m[3] = g->GetParameter(1);
  em[3] = g->GetParError(1);
  hg->Fit(g, "N", "", 9400000, 10200000);  //4pe
  m[4] = g->GetParameter(1);
  em[4] = g->GetParError(1);
  hg->Fit(g, "N", "", 11800000, 12700000);  //5pe
  m[5] = g->GetParameter(1);
  em[5]=g->GetParError(1);
  hg->Fit(g, "N", "", 14200000, 15000000);  //6pe
  m[6] = g->GetParameter(1);
  em[6]=g->GetParError(1);  
  double pe[7], epe[7];
  cout<<endl<<"-- Gain estimation --"<<endl;
  for(int i=0; i<7; i++){
    pe[i]=i;
    epe[i]=0;  //boh, errore nullo(?)
    m[i]=m[i];
    em[i]=em[i];
    cout<<i<<"pe: "<<m[i]<<"+-"<<em[i]<<endl;
  }

  //---- Plot (linear)
  TGraphErrors *gr = new TGraphErrors(7, pe, m, epe, em);
  gr->SetTitle("Gain plot;pe;Gain");
  gr->SetLineColor(kBlue+1);
  gr->SetLineWidth(3);
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

  cout << endl << "---- Fit lineare ----" << endl;
  cout << "Coefficiente angolare (Gain) = " << B << " +- " << eB << endl;
  cout << "Intercetta = " << A << " +- " << eA<< endl;


  //---- Saving in TFile
  f->cd();
  cgain->Write();
  
  f->Write();
  f->Close();

}




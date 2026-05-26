// Visualizzazione segnali da oscilloscopio

#include <iostream>
using namespace std;
const int N=100; //data for baseline
const int M=1024*8; //time window

void osc(int indice_wf, TString filename1, TString filename2, TString output="osc_sign.root"){

  TFile *f = new TFile(output, "RECREATE");
  
  int a1[1024*8], a2[1024*8];
  double b1, b2;
  TTree *dd = new TTree("dd", "dd");
  dd->Branch("A1", a1, "a1[8192]/I");
  dd->Branch("B1", &b1, "b1/D");
  dd->Branch("A2", a2, "a2[8192]/I");
  dd->Branch("B2", &b2, "b2/D");
  
  ifstream file1, file2;
  file1.open(filename1);
  file2.open(filename2);
  while(file1.good() && file2.good()){
    for(int i=0; i<M; i++) {
      file1 >> a1[i];
      file2 >> a2[i];
      if(i < N) {
	b1 += double(a1[i])/N;
	b2 += double(a2[i])/N;
      }
    }
    dd->Fill();
  }
  file1.close();
  file2.close();

  int ev=dd->GetEntries();

  double x[M];
  double y1[M];
  double y2[M];
  dd->GetEntry(indice_wf);
  
  for(int i=0; i<M; i++) {
    x[i]  = i*4;   // tempo in ns se campionamento = 4 ns
    y1[i] = a1[i];
    y2[i] = a2[i];
  }

  TCanvas *c1 = new TCanvas("c1","Waveforms",1200,800);
  c1->SetGrid();
  TGraph *g1 = new TGraph(M,x,y1);
  TGraph *g2 = new TGraph(M,x,y2);
  g1->SetLineColor(kAzure+5);
  g1->SetLineWidth(3);
  g1->GetXaxis()->SetRangeUser(16000,17000);
  g2->SetLineColor(kTeal+2);
  g2->SetLineWidth(3);
  g2->GetXaxis()->SetRangeUser(16000,17000);
  g2
    ->SetTitle("Waveforms;Time (ns);ADC");
  g2->Draw("AL");
  g1->Draw("L SAME");

  TLegend *leg = new TLegend(0.6,0.15,0.88,0.3);
  leg->AddEntry(g1,"Signal waveform","l");
  leg->AddEntry(g2,"Square waveform","l");
  leg->Draw();

  c1->Write();
  f->Write();
}

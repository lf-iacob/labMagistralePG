// Visualizzazione segnali da oscilloscopio

#include <iostream>
using namespace std;
const int N=100; //data for baseline
const int M=1024*8; //time window

void osc(TString filename1, TString filename2, int indice_wf, TString output="osc_sign.root"){

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

  TCanvas *c1 = new TCanvas();
  c1->SetGrid();
  dd->Draw("a2:Iteration$",Form("Entry$==%d", indice_wf),"AL");
  TGraph *g2 = (TGraph*)gPad->GetListOfPrimitives()->Last();
  g2->SetLineColor(kTeal+2);
  g2->SetLineWidth(3);
  g2->GetXaxis()->SetRangeUser(4010,4230);
  g2->SetTitle("Waveforms;Ticks;ADC");
  dd->Draw("a1:Iteration$",Form("Entry$==%d", indice_wf),"L SAME");
  TGraph *g1 = (TGraph*)gPad->GetListOfPrimitives()->Last();
  g1->SetLineColor(kAzure+5);
  g1->SetLineWidth(3);
  gPad->Modified();
  gPad->Update();
  TLegend *leg = new TLegend(0.6,0.15,0.88,0.3);
  leg->AddEntry(g1,"PMT Signal","l");
  leg->AddEntry(g2,"Square waveform","l");
  leg->Draw();
  c1->Write();
  
  f->Write();
}

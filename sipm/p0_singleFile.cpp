// -- Single File analysis --

#include <iostream>
using namespace std;
const int N=100; //data for baseline
const int L=1024; //time window

void albero(TString filename, TString output="output.root"){

  cout<<endl<<"------> TTree single file: "<<filename<<endl<<endl;

  //---- TFile as output
  if (output == "output.root"){
    Ssiz_t pos = filename.First('_');
    TString suffix = filename(pos, filename.Length() - pos);
    suffix.ReplaceAll(".adc", ".root");
    output = "alberi/alb" + suffix;
  }
  TFile *f = new TFile(output, "RECREATE");

  //---- Create TTree
  int a[L];
  double b, c, amp;
  TTree *dd = new TTree("dd", "dd");
  dd->Branch("A", a, "a[1024]/I");
  dd->Branch("B", &b, "b/D");
  dd->Branch("C", &c, "c/D");
  dd->Branch("AMP", &amp, "amp/D");

  //---- TTree->Fill()
  ifstream file;
  file.open(filename);
  while(file.good()){
    b=0.; c=0.; amp=0;
    for(int i=0;i<L;i++){
      file>>a[i]; 
      if(i<N) b+=double(a[i])/N;
      if((b-a[i])>amp) amp = b-a[i];
      else if(i>=450 && i<=550) c+=(b-a[i])*2000./4096.*4./50.;
    }
    dd->Fill();
  }
  file.close();
 
  int n = dd->GetEntries();

  //---- Charge THistogram
  TH1F *hc = new TH1F("hc", "Charge Histogram;Charge (pC);Entries",500,-50,300);
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hc->Fill(c);
  }

  TH1F *hcf = (TH1F*)hc->Clone("hc_f");

  TCanvas *hc_fit = new TCanvas();
  gPad->SetTicks(1,1);
  hcf->Draw();
  hcf->SetLineColor(kBlack);
  //hcf->SetLineWidth(2);
  hcf->Sumw2();
  

  //---- Amplitude THistogram
  TH1F *hamp = new TH1F("hamp", "Amplitude Histogram;Amplitude (ADC);Entries",500,-20,1000); //range to be modified
  for(int i=0; i<n; i++) {
    dd->GetEntry(i);
    hamp->Fill(amp);
  }

  //---- Saving in TFile
  f->Write();
}


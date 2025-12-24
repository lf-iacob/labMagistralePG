// -- Single File analysis --

#include <iostream>
using namespace std;
const int N=100; //data for baseline

void albero(TString filename, TString output="output.root"){

  cout<<endl<<"------> Analysis single file: "<<filename<<endl<<endl;

  //---- TFile as output
  if (output == "output.root"){
    Ssiz_t pos = filename.First('_');
    TString suffix = filename(pos, filename.Length() - pos);
    suffix.ReplaceAll(".adc", ".root");
    output = "ana" + suffix;
  }
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
  file.open(filename);
  while(file.good()){
    b=0.; c=0.;
    for(int i=0;i<1024;i++){
      file>>a[i];
      if(i<N) b+=double(a[i])/N;
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

  //---- Saving in TFile
  f->Write();
}

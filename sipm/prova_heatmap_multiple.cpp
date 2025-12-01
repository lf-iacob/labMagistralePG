#include <fstream>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TStyle.h>
const int ARRAY_SIZE = 1024;  
const int N = 100;
const int NUM_WFS = 10000;

void analizza(TString file_name1, TString file_name2, TString file_name3, TString file_name4, TString file_name5,
	      TString output="analysis.root") {

    // ---- File ROOT in scrittura
    TFile *f = new TFile(output, "RECREATE");

    // ---- Creazione TTree
    int a[ARRAY_SIZE];
    double b, c;
    TTree* dd = new TTree("dd", "dd");
    dd->Branch("A", a, "a[1024]/I");
    dd->Branch("B", &b, "b/D");
    dd->Branch("C", &c, "c/D");

    // ---- Lettura file
    ifstream file;
    file.open(file_name1);
    while(file.good()){
      for(int i=0;i<1024;i++) file>>a[i];
      dd->Fill();
    }
    file.close();

    file.open(file_name2);
    while(file.good()){
      for(int i=0;i<1024;i++) file>>a[i];
      dd->Fill();
    }
    file.close();

    file.open(file_name3);
    while(file.good()){
      for(int i=0;i<1024;i++) file>>a[i];
      dd->Fill();
    }
    file.close();

    file.open(file_name4);
    while(file.good()){
      for(int i=0;i<1024;i++) file>>a[i];
      dd->Fill();
    }
    file.close();

    file.open(file_name5);
    while(file.good()){
      for(int i=0;i<1024;i++) file>>a[i];
      dd->Fill();
    }
    file.close();

    // ---- Heatmap
    int nEvents = dd->GetEntries();
    int xBins = ARRAY_SIZE;    // numero di campioni
    double xmin = 0;
    double xmax = ARRAY_SIZE;
    int yBins = 300;         
    double ymin = 0;        
    double ymax = 2400;  

    TH2D* h2 = new TH2D("h2", "Heatmap;Time (ticks);Amplitude [ADC];Counts", 
			xBins, xmin, xmax, yBins, ymin, ymax);

    for(int i=0; i<nEvents; i++){
      dd->GetEntry(i);
      for(int j=0; j<ARRAY_SIZE; j++){
        h2->Fill(j, a[j]); 
      }
    }

    gStyle->SetPalette(kSolar);
    TCanvas *chp = new TCanvas("chp","Heatmap");
    chp->SetLogz();
    h2->Draw("COLZ");

    f->Write();

}

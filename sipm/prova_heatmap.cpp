#include <iostream>
#include <fstream>
#include <TTree.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH2D.h>
#include <TStyle.h>
using namespace std;

const int ARRAY_SIZE = 1024;  // dimensione array
const int N = 100;             // punti per calcolare la baseline
const int NUM_WFS = 100;       // waveform per file

void analizza(TString file_name, TString output="analysis.root") {
    cout << endl << "---- HEATMAP ----" << endl << endl;

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
    ifstream file(file_name);

    while(file.good()){
      for(int i=0;i<1024;i++) file>>a[i];
      dd->Fill();
    }
    file.close();

    // ---- Heatmap
    int nEvents = dd->GetEntries();
    int xBins = 200;    // numero di campioni
    double xmin = 0;
    double xmax = ARRAY_SIZE;

    int yBins = 200;           // numero di bin per l'ampiezza
    double ymin = 0;        // minimo ampiezza (da adattare ai tuoi dati)
    double ymax = 2400;         // massimo ampiezza (da adattare ai tuoi dati)

    TH2D* h2 = new TH2D("h2", "Heatmap;Index;Amplitude;Counts", 
			xBins, xmin, xmax, yBins, ymin, ymax);

    // Loop sugli eventi
    for(int i=0; i<nEvents; i++){
      dd->GetEntry(i);
      for(int j=0; j<ARRAY_SIZE; j++){
        h2->Fill(j, a[j]);  // X = indice del campione, Y = ampiezza
      }
    }

    // ---- Disegno
    gStyle->SetPalette(kSolar);
    TCanvas *chp = new TCanvas("chp","Heatmap");
    h2->Draw("COLZ");

    // ---- Salvataggio
    f->Write();

}


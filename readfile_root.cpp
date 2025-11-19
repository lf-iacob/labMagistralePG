// --- Different ways to read a file for ROOT ---

// ---------- METODO 1: Baioletti
//Quasi identico a quello di Rossi ma con più cose da scrivere... ignoralo
#include <iostream>
#include <fstream>
#include <vector>

void baioletti(string filename){
  fstream f;
  f.open(filename, ios::in);
  if(f.fail()==true)
    cerr<<"File can't be opened!"<<endl;
  else{
    vector<double>vx;
    while(f.eof()==false){
      double x;
      f>>x>>ws;
      vx.push_back(x);
    } 
    f.close();
    cout<<"Operation successfully completed"<<endl;
  }
}


// ---------- METODO 2: Graziani
//Legge il file con un metodo del TTree (immediato) ma con poco controllo su ripartizione dei dati (mh)

void graziani(const char* file_name){
  double x;
  TTree* T = new TTree("T", "correlated_events");
  T->ReadFile(file_name,"x/D");      //<-
  T->SetBranchAddress("x",&x);
}


// ---------- METODO 3: Rossi
//Legge il file con metodo di C++ (usa più comandi) ma hai più controllo sulla gestione dei dati in ingresso (the best)
 
void rossi(TString filename){
  
  ifstream file;         //<-
  file.open(filename);   //<-
  
  double v;
  TTree* ch = new TTree("ch", "wf");
  ch->Branch("v", &v, "v/D");
  while(file.good()){    //<-
    file>>v;             //<-
    ch->Fill();
  }
  
  file.close();          //<-
}

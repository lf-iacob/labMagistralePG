// Seconda versione -> uso array
// --- Esercizio 5 ---

// Criamo tree con i dati

void ExampleTree(TString fname, TString outname){

  ifstream file;
  file.open(fname);

  Double_t a[20];

  TFile* out = new TFile(outname, "recreate");
  TTree* tree = new TTree("tree", "tree");

  tree->Branch("A", a, "A[20]/D");

  while(file.good()){
    for(int i=0; i<20; i++)
      file>>a[i];
    
    tree->Fill();
  }

  tree->Write();
  out->Write();

  return;
}

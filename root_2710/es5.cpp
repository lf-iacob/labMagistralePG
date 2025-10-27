// --- Esercizio 5 ---

// Criamo tree con i dati

void ExampleTree(TString fname, TString outname){

  ifstream file;
  file.open(fname);

  Double_t a,b,c,d,e,f,g,h,i,l,m,n,o,p,q,r,t,u,v,z;

  TFile* out = new TFile(outname, "recreate");
  TTree* tree = new TTree("tree", "tree");

  tree->Branch("A", &a, "A/D");
  tree->Branch("B", &b, "B/D");
  tree->Branch("C", &c, "C/D");
  tree->Branch("D", &d, "D/D");
  tree->Branch("E", &e, "E/D");
  tree->Branch("F", &f, "F/D");
  tree->Branch("G", &g, "G/D");
  tree->Branch("H", &h, "H/D");
  tree->Branch("I", &i, "I/D");
  tree->Branch("L", &l, "L/D");
  tree->Branch("M", &m, "M/D");
  tree->Branch("N", &n, "N/D");
  tree->Branch("O", &o, "O/D");
  tree->Branch("P", &p, "P/D");
  tree->Branch("Q", &q, "Q/D");
  tree->Branch("R", &r, "R/D");
  tree->Branch("T", &t, "T/D");
  tree->Branch("U", &u, "U/D");
  tree->Branch("V", &v, "V/D");
  tree->Branch("Z", &z, "Z/D");

  while(file.good()){
    file>>a>>b>>c>>d>>e>>f>>g>>h>>i>>l>>m>>n>>o>>p>>q>>r>>t>>u>>v>>z;
    tree->Fill();
  }

  tree->Write();
  out->Write();

  return;
}

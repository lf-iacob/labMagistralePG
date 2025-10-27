// ----- Esercizio 1 -----


// -- LETTURA DEL FILE + RIEMPIMENTO ISTOGRAMMI --
//vogliamo leggere un file - usiamo modalità di cpp
void Example1(TString nomefile, TString outname){
	      //TString mi permette di gestire come in python le stringhe
              //outname permette che non sia hard coded in fase di esecuzione la creazione del TFile (se magari voglio rileggere altri dati e non sovrascrivere file)
  ifstream file;
    //ifstream - sta per input file stream, creo la variabile file e poi lo vado ad aprire con .open()
    //in file ho lo stream di dati
  file.open(nomefile);
    //dobbiamo leggere le due colonne di dati, però prima devo definire le due variabili
  Double_t a,b; //usiamo i double che poi mi serviranno in ROOT


  // voglio salvare i risultati degli istrogrammi in un file, da definire primaaaaa degli istogrammi 
  TFile* fout = new TFile(outname, "recreate"); //"update" se volgio solo aggiornare

  
  TH1F* h1 = new TH1F("h1", "Histogram 1", 1000, -100, 100);  //il binnaggio all'inizio è a caso perchè non conosco i dati, si devono aggiustare le cose dopo
  TH1F* h2 = new TH1F("h2", "Histogram 2", 1000, -100, 100);
  TH2F* jj = new TH2F("2D", "D histogram", 1000, -20, 20, 1000, -20, 20);
  //leggo i dati attraverso un ciclo while con file.good() che controlla che il file sia buono per la lettura
  while(file.good()){
    //ifstream adatta i metodi di cpp che parte da iostream per cout e cin per fare le stesse operazioni ma sul file che sto leggendo/scrivendo
    file>>a>>b; //due insieme leggiamo con stesso comando (pipe line)
    h1->Fill(a); //riempo istogrammi
    h2->Fill(b);
    jj->Fill(a,b);
  }
  file.close();
  
// -- RAPPRESENTAZIONE ISTOGRAMMI --
  
  TCanvas* c1 = new TCanvas();
  c1->cd();
  h1->Draw();
  TCanvas* c2 = new TCanvas();
  c2->cd();
  h2->Draw();
  TCanvas* c3 = new TCanvas();
  c3->cd();
  jj->Draw("COLZ");


  fout->Write(); //scrive su disco i file con questo comando

  //chiusura è automatica, ma per maggiore ordine meglio forzarlo noi
  //fout->Close();
  /* PROBLEMA
  Se creo il file, non mi mostra più i canvas quando chiudo il file.
  Questo è dovuto al fatto che oggetti non sono più salvati nella memoria temporanea del programma.
  Allora, problema si risolve se tolgo comando di chiusura del TFile.
  */
  
  
  return;
}

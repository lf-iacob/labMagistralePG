#include <iostream>
using namespace std;
const int L=1024*8;

void dcr(TString file1, TString file2="", TString file3="", TString file4="", //.root (alberi)
	 TString output="DCRanalysis.root"
	 ){

  cout<<endl<<"------> Dark Count Rate Analysis "<<endl<<endl;

  int first, second, third;
  sscanf(file1.Data(), "alberi/alb_dcr_%d_%d_%d", &first, &second, &third);
  double vbias = first + second / 100.0;
  cout<<"--> VBias (V): "<<vbias<<endl<<endl;

  /* --------- MODIFICA A MANO: VBias (intensità) [V] -> THreshold [ADC]
     54.51 (250) -> 30
     55.11 (40) -> 30
     55.81 (70) -> 50
     56.31 (200) -> 60
   */
  double th=30.0;        
  cout<<"Threshold: "<<th<<endl<<endl;

  TFile *file1_aperto = TFile::Open(file1);
  TTree *dd = (TTree*)file1_aperto->Get("dd");
  int a[L];
  double b,c,amp;
  dd->SetBranchAddress("A", a);
  dd->SetBranchAddress("B", &b);
  dd->SetBranchAddress("C", &c);
  int n = dd->GetEntries();

  
  TFile *f = new TFile(output, "RECREATE");
  TTree *outTree = new TTree("dcr", "DCR analysis");
  int dce;
  outTree->Branch("dce", &dce, "dce/I");

  for(int i=0; i<n; i++){
    dd->GetEntry(i);
    dce = 0;
    for(int j=0; j<L; j++){
      if((b-a[j]) > th){
	dce += 1;
	j+=100; //jump
      }
    }
    outTree->Fill();
  }

  TH1I *h_dce = new TH1I("h_dce", "DC Events Hist;DC Events;Events", 12, -1.5, 10.5);
  for(int i=0; i<n; i++) {
    outTree->GetEntry(i);
    h_dce->Fill(dce);
  }


  TF1 *pp = new TF1("pp","[1]*TMath::PoissonI(x,[0])",-0.5, 10.5);
                                                      //TO DO: AGGIUSTA FIT CON COLONNA CENTRATA IN INTERO
  pp->SetParameters(2,1000);
  double dceN, dceN_err, t, dcr, dcr_err;
  h_dce -> Fit(pp);
  dceN=pp->GetParameter(0);
  dceN_err=pp->GetParError(0);
  t=L*4; //ns
  dcr=dce/t;
  dcr_err=dce; //CORREGGI, QUI HO MESSO A CASO
  cout<<endl<<"VBias: "<<vbias<<" (t = "<<t<<" ns) -> DC Events: "<<dceN<<" +- "<<dceN_err<<endl;
  cout<<"    -----> DCR = "<<dcr<<" +- "<<dcr_err<<endl<<endl;


  //TO DO: PRENDI DATI PER ALTRI VBIAS
  //TO DO: STAMPA VALORI DCR E FAI GRAFICO LINEARE
  //TO DO: PROPAGA L'ERRORE SUL DCR

  
  
  f->Write();

}

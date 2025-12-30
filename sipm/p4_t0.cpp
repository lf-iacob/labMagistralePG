// --- Risoluzione temporale ---

#include <iostream>
using namespace std;

void analizza(TString fileroot, TString output="output.root"){
  
  cout<<endl<<"------> Analysis TR Single File"<<endl<<endl;

  if(output == "output.root"){
    output = fileroot;
    output.ReplaceAll("alberi/alb_", "t0_");
  }
  TFile *f = new TFile(output, "RECREATE");


  //---- Read TFile
  TFile *input = TFile::Open(fileroot);
  TTree *tree = (TTree*)input->Get("dd");
  int a[1024];  
  tree->SetBranchAddress("A", a);

  //---- t0
  TTree *dd_tr = new TTree("dd_tr", "dd_tr");
  double t0;
  dd_tr->Branch("t0", &t0, "t0/D");

  double n=tree->GetEntries();
  double semi_ampl, inf, sup, m, q;
  int tsup=450;
  
  for(int j=0; j<n; j++){
    tree->GetEntry(j);
    semi_ampl = (*max_element(a + 450, a + 490)+ *min_element(a + 450, a + 490))/2;
    sup=a[450];
    for(int i=451; i<475; i++){
      if(a[i]>semi_ampl){
	sup=a[i];
	tsup=i;
      }
      else break;
    }
    inf=a[tsup+1];
    m=inf-sup;
    q=sup-m*tsup;
    t0=(semi_ampl-q)/m;
    dd_tr->Fill();
    if(j==0){
      cout<<"Max: "<<*max_element(a + 450, a + 490)<<" Min: "<<*min_element(a + 450, a + 490)<<endl;
      cout<<"TMax: "<<distance(a, max_element(a + 450, a + 490))<<" TMin: "<<distance(a, min_element(a + 450, a + 490))<<endl;
      cout<<"Semi_amp: "<<semi_ampl<<endl;
      cout<<"Sup (tsup = "<<tsup<<"): "<<sup<<" Inf: "<<inf<<endl;
      cout<<"m: "<<m<<" q: "<<q<<endl;
      cout<<"t0: "<<t0<<endl;
    }
  }

  TH1F *ht0 = new TH1F("ht0", "ht0;t0;Entries", 100, 460, 470);
  for(int i=0; i<n; i++) {
    dd_tr->GetEntry(i);
    ht0->Fill(t0);
  }
  ht0->Draw();

  f->cd();
  ht0->Write();
  f->Write();
  //f->Close();

}

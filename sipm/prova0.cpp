// --- Single event analysis ---

#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TH1D.h>
#include <TCanvas.h>
using namespace std;

void estrai(
           TString file_name //file name for the data
           ){
  cout<<"-------- SINGLE EVENT ANALYSIS --------"<<endl;

  //---- File ROOT
  TFile *f = new TFile("single_wf.root", "RECREATE");

  
  //---- ORIGINAL DATA
      //(units: ADC channels)
  ifstream file;         
  file.open(file_name); 
  
  double v_b; //single value in the tree (with baseline)
  TTree* ch_b = new TTree("ch_b", "adc_channels_baseline");
  ch_b->Branch("v_b", &v_b, "v_b/D");

  while(file.good()){   
    file>>v_b;            
    ch_b->Fill();
  }
  file.close();  
  int ntot = ch_b -> GetEntries();
  

  //---- BASELINE EVALUATION
  double nparz=int(0.3*ntot); //post_trigger:50%, data_for_baseline:30% (out of 1024=2^10)
  cout<<"Number of data for the baseline evaluation: "<<nparz<<endl;
  double bline=0;
  for (int i=0; i<nparz; i++){
    ch_b->GetEntry(i);
    bline=bline+v_b;
  }
  bline=bline/nparz;
  cout<<"Baseline (ADC): "<<bline<<endl;


  //---- SHIFTED DATA (BASELINE=0)
  double v; //single value in the tree (baseline=0)
  TTree* ch = new TTree("ch", "adc_channels");
  ch->Branch("v", &v, "v/D");

  for (int i=0; i<ntot; i++){
    ch_b->GetEntry(i);
    v=v_b-bline;
    ch->Fill();
  }

  /* //Check if success
  cout<<"Original data -> Shifted data"<<endl;
  for (int i=0; i<ntot; i++){
    ch_b->GetEntry(i);
    ch->GetEntry(i);
    cout<<v_b<<" -> "<<v<<endl;
  }
  */


  //---- INTEGRAL (CHARGE)
  double charge = 0;
  for(int i=0.5*ntot; i<ntot; i++){ //(peak selection -?-)
    ch -> GetEntry(i);
    charge+=v;
  }
  charge=(-1)*charge*4; //Delta_t=4ns (sampling rate=250Ms/s)
  cout<<"Charge: "<<charge<<endl;

}

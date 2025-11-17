// --- Single event analysis ---

#include <iostream>
#include <TTree.h>
#include <TFile.h>
using namespace std;

void estrai(
           const char* file_name //file name for the data
           ){
  cout<<"-------- SINGLE EVENT ANALYSIS --------"<<endl;

  //---- File ROOT
  TFile *f = new TFile("single_wf.root", "RECREATE");
  
  //---- ORIGINAL DATA
      //(units: ADC channels)
  double v_b; //single value in the tree (with baseline)
  TTree* ch_b = new TTree("ch_b", "adc_channels_baseline");
  ch_b -> ReadFile(file_name,"v_b/D");
  ch_b -> SetBranchAddress("v_b",&v_b);
  int ntot = ch_b -> GetEntries();

  //---- BASELINE EVALUATION
  double nparz=0.3*ntot; //post_trigger:50%, data_for_baseline:30% (out of 1024=2^10)
  cout<<"Data for the baseline evaluation: "<<int(nparz)+1<<endl;
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

  for (int i=0; i<nparz; i++){
    ch_b->GetEntry(i);
    v=v_b-bline;
    ch->Fill();
  }

  

}

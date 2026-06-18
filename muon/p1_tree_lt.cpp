// Muon lifetime
using namespace std;
#include <iostream>
const int L=8*1024;
const int N=100;

void tree_lt(TString filename1, TString filename2, TString output="output.root"){

  TString ele_type = filename1;
  ele_type.ReplaceAll("1.adc", "");
  cout<<"DATATYPE: Electrons "<<ele_type<<endl<<endl;
  if(output=="output.root") output = "tree_" + ele_type + ".root";
  TFile *f = new TFile(output, "RECREATE");
  
  int a1[1024*8], a2[1024*8], t1, t2, t, s=1200;
  double b1, b2;
  TTree *dd = new TTree("dd", "dd");
  dd->Branch("A1", a1, "a1[8192]/I");
  dd->Branch("B1", &b1, "b1/D");
  dd->Branch("T1", &t1, "t1/I");
  dd->Branch("A2", a2, "a2[8192]/I");
  dd->Branch("B2", &b2, "b2/D");
  dd->Branch("T2", &t2, "t2/I");
  dd->Branch("T", &t, "t/I");
  
  ifstream file1, file2;
  file1.open(filename1);
  file2.open(filename2);
  while(file1.good() && file2.good()){
    b1=0.; b2=0.;
    for(int i=0;i<L;i++){
      file1>>a1[i];
      file2>>a2[i];
      if(i<N){
		  b1+=double(a1[i])/N;
		  b2+=double(a2[i])/N;
      }
    }
    for(int i=1; i<L; i++){
      if(((b1-a1[i])>s)){
		  t1=i*4;
		  break;
      } 
    }
     for(int i=1; i<L; i++){
      if(((b2-a2[i])>s)){
		  t2=i*4;
		  break;
      } 
    } 
    t=abs(t2-t1);
    dd->Fill();
  }
  file1.close();
  file2.close();

  int n = dd->GetEntries();

  cout<<"--> Tree creation completed !"
  f->Write();
}

